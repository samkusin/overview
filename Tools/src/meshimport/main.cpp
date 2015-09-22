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

#include "parse_common.hpp"

#include "optionparser.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

#include "common/jsonstreambuf.hpp"

#include "pugixml/pugixml.hpp"


/*
    {
        "name": <name>,
        "root": {
            "name": <node name>,
            "matrix" : [],
            "mesh" : {
                "materials" : [<name0>,<name1>,...],
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
            }
        },
        "materials" : {
            "<name>" : {
 
            
            }
        },
        "textures" : {
            "<name>" : <file path>
        }
    }
*/

rapidjson::Value createMeshNodeJSON
(
    const cinek::ovtools::MeshNode& node,
    rapidjson::Document::AllocatorType& allocator
)
{
    rapidjson::Value output(rapidjson::kObjectType);
    
    output.AddMember("name", node.name, allocator);
    
    rapidjson::Value vertices(rapidjson::kArrayType);
    vertices.Reserve((uint32_t)node.points.size(), allocator);
    
    for (auto& point : node.points) {
        rapidjson::Value vertex(rapidjson::kObjectType);
        vertex.AddMember("x", point.x, allocator);
        vertex.AddMember("y", point.y, allocator);
        vertex.AddMember("z", point.z, allocator);
        vertices.PushBack(vertex, allocator);
    }
    
    output.AddMember("vertices", std::move(vertices), allocator);
    
    rapidjson::Value normals(rapidjson::kArrayType);
    normals.Reserve((uint32_t)node.triangles.size(), allocator);
    
    rapidjson::Value faces(rapidjson::kArrayType);
    faces.Reserve((uint32_t)node.triangles.size(), allocator);
    
    for (auto& tri : node.triangles) {
        rapidjson::Value normal(rapidjson::kObjectType);
        normal.AddMember("x", tri.normal.x, allocator);
        normal.AddMember("y", tri.normal.y, allocator);
        normal.AddMember("z", tri.normal.z, allocator);
        normals.PushBack(normal, allocator);
    
        rapidjson::Value face(rapidjson::kArrayType);
        face.Reserve(4, allocator); // x,y,z + index for normal
        face.PushBack(tri.indices[0], allocator);
        face.PushBack(tri.indices[1], allocator);
        face.PushBack(tri.indices[2], allocator);
        face.PushBack((int)(&tri - node.triangles.data()), allocator);
        faces.PushBack(face, allocator);
    }
    
    output.AddMember("normals", std::move(normals), allocator);
    output.AddMember("tris", std::move(faces), allocator);
    
    return output;
}

rapidjson::Value createSceneNodeJSON
(
    const cinek::ovtools::SceneNode& node,
    rapidjson::Document::AllocatorType& allocator
)
{
    rapidjson::Value output(rapidjson::kObjectType);
    
    output.AddMember("name", node.name, allocator);
    
    rapidjson::Value matrix(rapidjson::kArrayType);
    matrix.Reserve(sizeof(node.mtx.m)/sizeof(node.mtx.m[0]), allocator);
    
    for (int i = 0; i < matrix.Capacity(); ++i) {
        matrix.PushBack(node.mtx.m[i], allocator);
    }
    
    output.AddMember("matrix", matrix, allocator);
    
    if (node.mesh) {
        output.AddMember("mesh", std::move(createMeshNodeJSON(*node.mesh, allocator)),
                         allocator);
    }
    
    return output;
}

rapidjson::Document writeToJSON(const cinek::ovtools::Scene& scene)
{
    rapidjson::Document output(rapidjson::kObjectType);
    rapidjson::Document::AllocatorType& allocator = output.GetAllocator();
    
    output.AddMember("name", scene.name, allocator);
    output.AddMember("node", std::move(createSceneNodeJSON(*scene.root, allocator)),
                     allocator);
    
    return output;
}

////////////////////////////////////////////////////////////////////////////////


enum OptionArg
{
    kOptionUnknown,
    kOptionHelp,
    kOptionNode
};

static const option::Descriptor sArgHelp[] = {
    { kOptionUnknown,   0, ""       , ""        , option::Arg::None,
      "usage: meshimport <source filename> [<dest>]\n" },
    { kOptionHelp,      0, ""       , "help"    , option::Arg::None,
      "  --help  \t\t command line usage." },
    { kOptionNode,      0, ""       , "node"    , option::Arg::Optional,
      "  --node=<scene_node_name> \t name of the scene node to filter." },
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
            destPathStr += ".ckm";
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
        std::ifstream inputStream(srcPathStr.c_str());
        if (inputStream.fail())
            throw std::runtime_error("Failed to open file " + srcPathStr);
        pugi::xml_document colladaDoc;
        pugi::xml_parse_result result = colladaDoc.load(inputStream);
        if (!result)
            throw std::runtime_error("Failed to parse file " + srcPathStr);
        
        std::string parseTypeName;
        cinek::ovtools::SceneParseType parseType = cinek::ovtools::kSceneParseInvalid;
        
        if (argOpts[kOptionNode]) {
            parseTypeName = argOpts[kOptionNode].arg;
            parseType = cinek::ovtools::kSceneParseNode;
        }
        
        cinek::ovtools::Scene scene = cinek::ovtools::parseColladaToData(
            colladaDoc,
            parseType,
            parseTypeName
        );
        rapidjson::Document ckmJson = writeToJSON(scene);
     
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
