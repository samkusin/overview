//
//  main.cpp
//  ckmeshimport
//
//  Created by Samir Sinha on 9/19/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// yes, we need ordering and don't care about overhead for a tool
#include <set>

#include "optionparser.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

#include "jsonstreambuf.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


/*
    {
        "name": <name>,
        "meshes" : [
            {
                "material" : <name>,
                "vertices": [
                    { "x":<x>,"y":<y>,"z":<z> },
                    ...
                ],
                "normals: [
                    { "x":<x>,"y":<y>,"z":<z> }
                ],
                "tris": [
                    [ idx0, idx1, idx2, normalIdx ],
                    ...
                ]
            },
            ...
        ],
        "node": {
            "name": <node name>,
            "matrix" : [],
            "meshes" : [],
            "children": [
                { <node> },
                ...
            ]
        },
        "materials" : [
            {
                "name": <name>
            },
            ...
        ]
    }
*/

struct SceneOutputOptions
{
    enum
    {
        kSceneOutputDefault,
        kSceneOutputModel
    }
    method;
    std::string filterNodeName;
    const aiNode* rootNode;
    
    std::set<unsigned int> materialsUsed;
    std::set<unsigned int> meshesUsed;

};


template<typename JsonAllocator>
rapidjson::Value createJSONArrayFromMat4x4(const aiMatrix4x4& m, JsonAllocator& allocator)
{
    rapidjson::Value matrix(rapidjson::kArrayType);
    matrix.Reserve(16, allocator);
    
    for (int i = 0; i < 4; ++i) {
        matrix.PushBack(m[i][0], allocator);
        matrix.PushBack(m[i][1], allocator);
        matrix.PushBack(m[i][2], allocator);
        matrix.PushBack(m[i][3], allocator);
    }
    
    return matrix;
}

template<typename JsonAllocator>
rapidjson::Value createJSONColor(const aiColor3D& color, JsonAllocator& allocator)
{
    rapidjson::Value matColor(rapidjson::kObjectType);
    
    matColor.AddMember("r", color.r, allocator);
    matColor.AddMember("g", color.g, allocator);
    matColor.AddMember("b", color.b, allocator);
    
    return matColor;
}

template<typename JsonAllocator>
rapidjson::Value createJSONColor(const aiColor4D& color, JsonAllocator& allocator)
{
    rapidjson::Value matColor(rapidjson::kObjectType);

    matColor.AddMember("r", color.r, allocator);
    matColor.AddMember("g", color.g, allocator);
    matColor.AddMember("b", color.b, allocator);
    matColor.AddMember("a", color.a, allocator);
    
    return matColor;
}

rapidjson::Value createMaterialJSON
(
    const aiMaterial& material,
    rapidjson::Document::AllocatorType& allocator
)
{
    rapidjson::Value output(rapidjson::kObjectType);
    
    aiString propString;
    aiColor3D propColor;

    rapidjson::Value matColor(rapidjson::kObjectType);
    
    //  keeping the name for reference when scanning a JSON.  but materials are
    //  never referenced by name (for now?) while scanning the output scene
    if (material.Get(AI_MATKEY_NAME, propString) == AI_SUCCESS) {
        rapidjson::Value str(rapidjson::kStringType);
        str.SetString(propString.C_Str(), allocator);
        output.AddMember("name", std::move(str), allocator);
    }
    
    //  Shading constants and textures
    //  DIFFUSE -
    if (material.Get(AI_MATKEY_COLOR_DIFFUSE, propColor) == AI_SUCCESS) {
        output.AddMember("diffuse", createJSONColor(propColor, allocator), allocator);
    
        auto& component = output["diffuse"];
    
        rapidjson::Value textures(rapidjson::kArrayType);
        unsigned int numTextures = material.GetTextureCount(aiTextureType_DIFFUSE);
        textures.Reserve(numTextures, allocator);
        for (unsigned int texIdx = 0; texIdx < numTextures; ++texIdx) {
            aiString texPath;
            auto result = material.GetTexture(aiTextureType_DIFFUSE, texIdx, &texPath);
            if (result == AI_SUCCESS) {
                const char* texPathCStr = texPath.C_Str();
                
                //  skip leading slashes
                while (*texPathCStr == '/')
                    ++texPathCStr;
                rapidjson::Value str(rapidjson::kStringType);
                str.SetString(texPathCStr, allocator);
                textures.PushBack(std::move(str), allocator);
            }
        }
        
        component.AddMember("textures", textures, allocator);
    }
    if (material.Get(AI_MATKEY_COLOR_SPECULAR, propColor) == AI_SUCCESS) {
        output.AddMember("specular", createJSONColor(propColor, allocator), allocator);
    
        auto& component = output["specular"];
        
        float scalar = 0.0f;
        
        material.Get(AI_MATKEY_SHININESS, scalar);
        component.AddMember("power", scalar, allocator);
        
        //  requires custom assimp additions
        scalar = 0.0f;
        material.Get(AI_MATKEY_SHININESS_STRENGTH, scalar);
        component.AddMember("intensity", scalar, allocator);
    
        rapidjson::Value textures(rapidjson::kArrayType);
        unsigned int numTextures = material.GetTextureCount(aiTextureType_SPECULAR);
        textures.Reserve(numTextures, allocator);
        for (unsigned int texIdx = 0; texIdx < numTextures; ++texIdx) {
            aiString texPath;
            auto result = material.GetTexture(aiTextureType_SPECULAR, texIdx, &texPath);
            if (result == AI_SUCCESS) {
                const char* texPathCStr = texPath.C_Str();
                
                //  skip leading slashes
                while (*texPathCStr == '/')
                    ++texPathCStr;
                rapidjson::Value str(rapidjson::kStringType);
                str.SetString(texPathCStr, allocator);
                textures.PushBack(std::move(str), allocator);
            }
        }
        
        component.AddMember("textures", textures, allocator);
    }
    
    return output;
}

rapidjson::Value createMeshNodeJSON
(
    const aiMesh& mesh,
    rapidjson::Document::AllocatorType& allocator
)
{
    rapidjson::Value output(rapidjson::kObjectType);
    rapidjson::Value str(rapidjson::kStringType);
    
    str.SetString(mesh.mName.C_Str(), allocator);
 
    output.AddMember("name", std::move(str), allocator);
    output.AddMember("material", mesh.mMaterialIndex, allocator);

    rapidjson::Value vertices(rapidjson::kArrayType);
    vertices.Reserve(mesh.mNumVertices, allocator);
    
    rapidjson::Value normals(rapidjson::kArrayType);
    normals.Reserve(mesh.mNumVertices, allocator);
    
    for (unsigned int vi = 0; vi < mesh.mNumVertices; ++vi) {
        const aiVector3D& point = mesh.mVertices[vi];
        
        rapidjson::Value vertex(rapidjson::kObjectType);
        vertex.AddMember("x", point.x, allocator);
        vertex.AddMember("y", point.y, allocator);
        vertex.AddMember("z", point.z, allocator);
        vertices.PushBack(vertex, allocator);
        
        const aiVector3D& dir = mesh.mNormals[vi];
        
        rapidjson::Value normal(rapidjson::kObjectType);
        normal.AddMember("x", dir.x, allocator);
        normal.AddMember("y", dir.y, allocator);
        normal.AddMember("z", dir.z, allocator);
        normals.PushBack(normal, allocator);
    }
    
    output.AddMember("vertices", std::move(vertices), allocator);
    output.AddMember("normals", std::move(normals), allocator);
    
    for (unsigned int iUVSet = 0; iUVSet < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++iUVSet) {
        unsigned int numUVComp = mesh.mNumUVComponents[iUVSet];
        if (mesh.HasTextureCoords(iUVSet)) {
            rapidjson::Value uvs(rapidjson::kArrayType);
            uvs.Reserve(mesh.mNumVertices, allocator);
            
            const aiVector3D* uvwSet = mesh.mTextureCoords[iUVSet];
            
            for (unsigned int vi = 0; vi < mesh.mNumVertices; ++vi) {
                const aiVector3D& point = uvwSet[vi];
                rapidjson::Value uvw(rapidjson::kObjectType);
                
                uvw.AddMember("u", point.x, allocator);
                if (numUVComp > 1) {
                    uvw.AddMember("v", point.y, allocator);
                }
                if (numUVComp > 2) {
                    uvw.AddMember("w", point.z, allocator);
                }
                uvs.PushBack(uvw, allocator);
            }
            
            char uvwsetname[8] = { 't','e','x', (char)('0'+iUVSet), 0 };
            
            rapidjson::Value texSetLabel(rapidjson::kStringType);
            texSetLabel.SetString(uvwsetname, allocator);
            output.AddMember(std::move(texSetLabel), std::move(uvs), allocator);
        }
    }
    
    rapidjson::Value tris(rapidjson::kArrayType);
    
    for (unsigned int vf = 0; vf < mesh.mNumFaces; ++vf) {
        const aiFace& face = mesh.mFaces[vf];
    
        rapidjson::Value idxlist(rapidjson::kArrayType);
        idxlist.Reserve(face.mNumIndices, allocator);
        if (face.mNumIndices != 3) {
            std::cout << "mesh['" << mesh.mName.C_Str()
                      << "'] skipping face - not a triangle (cnt="
                      << face.mNumIndices
                      << ")" << std::endl;
            /*
            throw std::runtime_error(
                std::string("mesh['") + mesh.mName.C_Str() + "'] face is not a triangle."
            );
            */
            continue;
        }
        
        for (unsigned int vi = 0; vi < face.mNumIndices; ++vi) {
            idxlist.PushBack(face.mIndices[vi], allocator);
        }
        tris.PushBack(idxlist, allocator);
    }
    
    output.AddMember("tris", tris, allocator);
    
    return output;
}



rapidjson::Value createSceneNodeJSON
(
    const aiScene& scene,
    const aiNode& node,
    const SceneOutputOptions& opts,
    rapidjson::Document::AllocatorType& allocator
)
{
    rapidjson::Value output(rapidjson::kObjectType);
    rapidjson::Value str(rapidjson::kStringType);
    str.SetString(node.mName.C_Str(), allocator);
    
    output.AddMember("name", std::move(str), allocator);
    
    output.AddMember("matrix",
        createJSONArrayFromMat4x4(node.mTransformation, allocator),
        allocator);
    
    unsigned int numMeshes = node.mNumMeshes;
    rapidjson::Value meshes(rapidjson::kArrayType);
    
    if (numMeshes > 0) {
        meshes.Reserve(numMeshes, allocator);
        
        for (unsigned meshIdx = 0; meshIdx < numMeshes; ++meshIdx) {
            meshes.PushBack(node.mMeshes[meshIdx], allocator);
        }
    }
    output.AddMember("meshes", meshes, allocator);
    
    unsigned int numChildren = node.mNumChildren;
    rapidjson::Value children(rapidjson::kArrayType);
    
    if (numChildren) {
        children.Reserve(numChildren, allocator);
        
        for (unsigned childIdx = 0; childIdx < numChildren; ++childIdx) {
            const aiNode* child = node.mChildren[childIdx];
            children.PushBack(std::move(
                createSceneNodeJSON(scene, *child, opts, allocator)),
                allocator);
        }
    }
    
    output.AddMember("children", children, allocator);

    return output;
}


rapidjson::Document writeToJSON
(
    const aiScene& scene,
    const SceneOutputOptions& opts
)
{
    rapidjson::Document output(rapidjson::kObjectType);
    rapidjson::Document::AllocatorType& allocator = output.GetAllocator();
 
    //  Full list of Materials - for unsupported types, we leave the entry
    //  blank, *but* maintain order of the materials array so that our meshes
    //  can still point to the correct material without remapping.
    rapidjson::Value materials(rapidjson::kArrayType);
    materials.Reserve((unsigned)opts.materialsUsed.size(), allocator);
    for (auto matIt = opts.materialsUsed.begin();
         matIt != opts.materialsUsed.end();
         ++matIt) {
        const aiMaterial* material = scene.mMaterials[*matIt];
        materials.PushBack(std::move(createMaterialJSON(*material, allocator)),
                           allocator);
    }

    output.AddMember("materials", materials, allocator);
    
    //  Full list of Meshes
    //
    rapidjson::Value meshes(rapidjson::kArrayType);
    meshes.Reserve((unsigned)opts.meshesUsed.size(), allocator);
    for (auto meshIt = opts.materialsUsed.begin();
         meshIt != opts.materialsUsed.end();
         ++meshIt) {
        const aiMesh* mesh = scene.mMeshes[*meshIt];
        meshes.PushBack(std::move(createMeshNodeJSON(*mesh, allocator)),
                        allocator);
    }
    output.AddMember("meshes", meshes, allocator);
    
    //  Scene Graph
    //
    const aiNode* node = opts.rootNode;
    if (node)
    {
        output.AddMember("node", std::move(
            createSceneNodeJSON(scene, *node, opts, allocator)),
            allocator);
    }
    
    return output;
}

////////////////////////////////////////////////////////////////////////////////

struct SceneStats
{
    unsigned int numMeshes;
    unsigned int numMeshInstances;
    unsigned int numMaterials;
    unsigned int numVertices;
    unsigned int numInstancedVertices;
    unsigned int numFaces;
    unsigned int numInstancedFaces;
    unsigned int numNodes;
    unsigned int nodeDepth;
    unsigned int currentNodeDepth;
};

SceneStats& accumulateSceneNodeStats
(
    SceneStats& stats,
    SceneOutputOptions& opts,
    const aiScene& scene,
    const aiNode& node,
    bool accumulate
)
{
    if (!accumulate && opts.method != SceneOutputOptions::kSceneOutputDefault) {
        if (!strcmp(node.mName.C_Str(), opts.filterNodeName.c_str())) {
            accumulate = true;
            opts.rootNode = &node;
        }
    }
    
    if (accumulate) {
        stats.numMeshInstances += node.mNumMeshes;
        for (unsigned int idx = 0; idx < node.mNumMeshes; ++idx) {
            unsigned int meshIndex = node.mMeshes[idx];
            aiMesh* mesh = scene.mMeshes[meshIndex];
            
            stats.numInstancedVertices += mesh->mNumVertices;
            stats.numInstancedFaces += mesh->mNumFaces;
            opts.meshesUsed.emplace(meshIndex);
            
            //  materials
            opts.materialsUsed.emplace(mesh->mMaterialIndex);
        }
        
        ++stats.numNodes;
        ++stats.currentNodeDepth;
    }
    
    for (unsigned int childIdx = 0; childIdx < node.mNumChildren; ++childIdx) {
        const aiNode* child = node.mChildren[childIdx];
        accumulateSceneNodeStats(stats, opts, scene, *child, accumulate);
    }
    
    if (accumulate) {
        if (stats.currentNodeDepth > stats.nodeDepth)
            stats.nodeDepth = stats.currentNodeDepth;
        --stats.currentNodeDepth;
    }
    
    return stats;
}

SceneStats getSceneStats(const aiScene& scene, SceneOutputOptions& opts)
{
    SceneStats stats;
    
    stats.numFaces = 0;
    stats.numVertices = 0;
    stats.numMeshInstances = 0;
    stats.numInstancedFaces = 0;
    stats.numInstancedVertices = 0;
    stats.numNodes = 0;
    stats.currentNodeDepth = 0;
    stats.nodeDepth = 0;

    //   by default, accumulate all stats.  optionally filter by node tree name
    bool accumulate = true;
    if (opts.method == SceneOutputOptions::kSceneOutputModel) {
        //  accumulate results only for the specified node tree
        accumulate = false;
    }
    
    stats = accumulateSceneNodeStats(stats, opts, scene, *scene.mRootNode, accumulate);

    stats.numMaterials = (uint32_t)opts.materialsUsed.size();
    stats.numMeshes = (uint32_t)opts.meshesUsed.size();

    for (auto it = opts.meshesUsed.begin(); it != opts.meshesUsed.end(); ++it) {
        stats.numFaces += scene.mMeshes[*it]->mNumFaces;
        stats.numVertices += scene.mMeshes[*it]->mNumVertices;
    }

    return stats;
}


////////////////////////////////////////////////////////////////////////////////

enum OptionArg
{
    kOptionUnknown,
    kOptionHelp,
    kOptionModel
};

static const option::Descriptor sArgHelp[] = {
    { kOptionUnknown,   0, ""       , ""        , option::Arg::None,
      "usage: meshimport <source filename> [<dest>]\n" },
    { kOptionHelp,      0, ""       , "help"    , option::Arg::None,
      "  --help  \t\t command line usage." },
    { kOptionModel,      0, ""       , "model"  , option::Arg::Optional,
      "  --model=<node name>\t exports a model (vs a complete scene)" },
    { 0,                0, NULL     , NULL      , 0, NULL }
};


int main(int argc, const char * argv[])
{
    if (argc > 0) {
        --argc;
        ++argv;
    }
    
    option::Stats argStats(sArgHelp, argc, argv);
    std::vector<option::Option> argOpts(argStats.options_max);
    std::vector<option::Option> argBuf(argStats.buffer_max);
    option::Parser argParse(sArgHelp, argc, argv, argOpts.data(), argBuf.data());
    
    if (argParse.error())
        return 1;
    
    int numParams = argParse.nonOptionsCount();

    if (argOpts[kOptionHelp] || argc == 0 || numParams < 1 || numParams > 2) {
        option::printUsage(std::cout, sArgHelp);
        return 0;
    }
    
    //  parse source and target filenames
    //
    std::string srcPathStr = argParse.nonOptions()[0];
    std::string destPathStr;
    
    if (numParams < 2) {
        //  construct a target filename - using the source path minus the
        //  directory and extension (will target the current working dir)
        destPathStr = srcPathStr;
        size_t pos = destPathStr.find_last_of("\\/");
        if (pos != std::string::npos) {
            destPathStr.erase(0, pos+1);
        }
        pos = destPathStr.find_last_of('.');
        if (pos != std::string::npos) {
            destPathStr.erase(pos);
        }
    }
    else {
        destPathStr = argParse.nonOptions()[1];
    }
    if (!destPathStr.empty()) {
        size_t pos = destPathStr.find_last_of("\\/");
        pos = destPathStr.find('.', pos==std::string::npos ? 0: pos);
        if (pos == std::string::npos) {
            destPathStr += ".json";
        }
    }
    else {
        option::printUsage(std::cout, sArgHelp);
        return 0;
    }

    //  parse options
    //
    try
    {
        SceneOutputOptions outputType = { SceneOutputOptions::kSceneOutputDefault, };
        
        unsigned int importFlags = aiProcessPreset_TargetRealtime_Quality;
        
        if (argOpts[kOptionModel]) {
            importFlags |= aiProcess_OptimizeGraph;
            outputType = { SceneOutputOptions::kSceneOutputModel, argOpts[kOptionModel].arg, };
            std::cout << "Model Import Method for Node["
                      << outputType.filterNodeName
                      << "] ... "
                      << std::endl;
        }
        else {
            std::cout << "Scene Import Method ... "
                      << std::endl;
        }
        
        Assimp::Importer importer;
        
        const aiScene* scene = importer.ReadFile(srcPathStr,
            aiProcessPreset_TargetRealtime_Quality
        );
        
        if (!scene) {
            throw std::runtime_error(
                std::string("Failed to import scene: ") + importer.GetErrorString()
            );
        }
        
        if (outputType.method == SceneOutputOptions::kSceneOutputModel) {
            outputType.rootNode = nullptr;  // to be found
        }
        else {
            outputType.rootNode = scene->mRootNode;
        }
        
        
        SceneStats sceneStats = getSceneStats(*scene, outputType);
       
        //std::cout << "STATISTICS:"                                      << std::endl;
        std::cout << "Materials:\t\t"       << sceneStats.numMaterials  << std::endl;
        std::cout << "Meshes:\t\t\t"        << sceneStats.numMeshes     << "\t\tInstanced: " << sceneStats.numMeshInstances << std::endl;
        std::cout << "Triangles:\t\t"       << sceneStats.numFaces      << "\t\tInstanced: " << sceneStats.numInstancedFaces << std::endl;
        std::cout << "Vertices:\t\t"        << sceneStats.numVertices   << "\t\tInstanced: " << sceneStats.numInstancedVertices << std::endl;
        std::cout << "Scene Nodes:\t\t"     << sceneStats.numNodes      << std::endl;
        std::cout << "Scene Depth:\t\t"     << sceneStats.nodeDepth     << std::endl;
        
        rapidjson::Document ckmJson = writeToJSON(*scene, outputType);
     
        //  output file
        std::filebuf outbuf;
    
        if (!outbuf.open(destPathStr.c_str(), std::ios_base::out | std::ios_base::trunc)) {
            throw std::runtime_error("Failed to create output file " + destPathStr);
        }
        rapidjson::StdStreamBuf jsonStream(outbuf);
        rapidjson::PrettyWriter<rapidjson::StdStreamBuf> jsonWriter(jsonStream);
        ckmJson.Accept(jsonWriter.SetIndent(' ', 2));
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Unhandled exception\n";
    }
    return 0;
}
