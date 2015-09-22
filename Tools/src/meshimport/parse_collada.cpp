//
//  parse_collada.cpp
//  meshimport
//
//  Created by Samir Sinha on 9/19/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "parse_common.hpp"

#include "pugixml/pugixml.hpp"

#include <stdexcept>
#include <string>
#include <cstring>
#include <cstdlib>

#include <iostream>
#include <numeric>

namespace cinek { namespace ovtools {

namespace collada {

    static pugi::xml_node xml_find_node_from_ref
    (
        pugi::xml_node parent,
        const char* ref,
        const char* nodename=""
    )
    {
        if (!parent || !ref[0])
            return pugi::xml_node();
        
        pugi::xml_node node;
        if (ref[0] == '#')
            ++ref;
        
        if (!nodename || !nodename[0]) {
            node = parent.find_child_by_attribute("id", ref);
        }
        else {
            node = parent.find_child_by_attribute(nodename, "id", ref);
        }
        
        if (!node) {
            return xml_find_node_from_ref(parent.parent(), ref, nodename);
        }
        
        return node;
    }

    static std::vector<float> xml_parse_float_array
    (
        pugi::xml_node array,
        size_t sz = 0
    )
    {
        std::vector<float> outValues;
        
        if (!strcmp(array.name(), "float_array")) {
            sz = array.attribute("count").as_int();
        }
        
        outValues.resize(sz);
        
        const char* text = array.text().get();
        for (auto& value : outValues) {
            char* textend;
            value = strtof(text, &textend);
            text = textend;
        }
        
        return outValues;
    }
    
    static std::vector<int> xml_parse_int_array
    (
        pugi::xml_node array,
        size_t sz = 0
    )
    {
        std::vector<int> outValues;
        
        if (!strcmp(array.name(), "int_array")) {
            sz = array.attribute("count").as_int();
        }
        
        outValues.resize(sz);
        
        const char* text = array.text().get();
        for (auto& value : outValues) {
            char* textend;
            value = (int)strtol(text, &textend, 10);
            text = textend;
        }
        
        return outValues;
    }
    
    static std::vector<Point> copy_floats_into_points_vector
    (
        const std::vector<float>& values,
        int count,
        int offset,
        int stride
    )
    {
        std::vector<Point> outPoints;
        outPoints.reserve(count);
        
        int idx = offset;
        
        while (idx <= values.size() - stride) {
            outPoints.emplace_back();
            
            auto& pt = outPoints.back();
            if (stride >= 1)
                pt.x = values[idx];
            if (stride >= 2)
                pt.y = values[idx+1];
            if (stride >= 3)
                pt.z = values[idx+2];
            
            idx += stride;
        }
        
        return outPoints;
    }

    static std::vector<Point> parse_source_into_points_vector
    (
        pugi::xml_node source
    )
    {
        std::vector<Point> outMeshPoints;
        
        //  TODO - check for technique node first?
        auto technique = source.child("technique_common");
        if (!technique)
            return outMeshPoints;
        
        //  TODO - possble to have more than one accessor if more than one array
        //         in the source?
        auto accessor = technique.child("accessor");
        const char* arrayRef = accessor.attribute("source").value();
        int objcount = accessor.attribute("count").as_int();
        int objoffset = accessor.attribute("offset").as_int();
        int objstride = accessor.attribute("stride").as_int();
        
        auto array = xml_find_node_from_ref(source, arrayRef);
        
        if (array) {
            auto floats = xml_parse_float_array(array);
            outMeshPoints = copy_floats_into_points_vector(floats,
                objcount, objoffset, objstride);
        }
        
        return outMeshPoints;
    }

}   /* namespace collada */

using namespace collada;

std::unique_ptr<MeshNode> parseColladaGeometryToData
(
    pugi::xml_node geometry
)
{
    auto mesh = geometry.child("mesh");
    if (!mesh)
        mesh = geometry.child("convex_mesh");
    
    if (!mesh)
        return nullptr;
    
    MeshNode outMeshNode;
    outMeshNode.name = geometry.attribute("name").value();
    const std::string runtimeContext = std::string("geometry['") + outMeshNode.name + "'].mesh ";
    
    auto polylist = mesh.child("polylist");
    
    //  construct triangles from positions and vertex normals
    auto vcounts = collada::xml_parse_int_array( polylist.child("vcount"),
                    polylist.attribute("count").as_int() );
    
    if (vcounts.empty() ||
        std::find_if_not(vcounts.begin(),
                         vcounts.end(),
                        [](const int& v) -> bool { return v == 3; }) != vcounts.end()) {
        throw std::runtime_error(runtimeContext + "must be triangulated.");
    }
    
    size_t faceCount = vcounts.size();
    outMeshNode.triangles.resize(faceCount);
    
    
    //  read vertices
    auto vertexInput =
        polylist.find_child_by_attribute("input", "semantic", "VERTEX");
    int vertexInputIndexOffset = vertexInput.attribute("offset").as_int();
    int indicesPerVertex = 1;
    
    //  retrieve vertex positions (and possibly other vertex components
    //  except normals, which are handled separately?)
    auto vertexFmt = collada::xml_find_node_from_ref(mesh,
        vertexInput.attribute("source").value(),
        "vertices");
    auto positions = vertexFmt.find_child_by_attribute("input", "semantic", "POSITION");
    positions = collada::xml_find_node_from_ref(mesh,
        positions.attribute("source").value(),
        "source");
    
    if (!vertexFmt || !positions) {
        throw std::runtime_error(std::string(runtimeContext) + "has no vertices");
    }
    
    outMeshNode.points = collada::parse_source_into_points_vector(positions);
    
    //  import face normals
    auto normalInput =
        polylist.find_child_by_attribute("input", "semantic", "NORMAL");
    int normalInputIndexOffset = normalInput.attribute("offset").as_int();
    
    if (normalInput) {
        ++indicesPerVertex;
    }
    
    std::vector<Vector3> faceNormals;
    
    if (normalInput) {
        auto normals = collada::xml_find_node_from_ref(mesh,
            normalInput.attribute("source").value(),
            "source");
        faceNormals = collada::parse_source_into_points_vector(normals);
    }
    
    //  create faces
    
    auto indices = collada::xml_parse_int_array(polylist.child("p"),
            std::accumulate(vcounts.begin(), vcounts.end(), 0) * indicesPerVertex);

    for (int faceIdx = 0, indicesIdx = 0;
         faceIdx < faceCount;
         ++faceIdx) {
        auto& triangle = outMeshNode.triangles[faceIdx];
        triangle.indices[0] = indices[indicesIdx + 0 * indicesPerVertex + vertexInputIndexOffset];
        triangle.indices[1] = indices[indicesIdx + 1 * indicesPerVertex + vertexInputIndexOffset];
        triangle.indices[2] = indices[indicesIdx + 2 * indicesPerVertex + vertexInputIndexOffset];
        int faceNormalIdx = indices[indicesIdx + 0 * indicesPerVertex + normalInputIndexOffset];
        triangle.normal = faceNormals[faceNormalIdx];
        indicesIdx += vcounts[faceIdx] * indicesPerVertex;
    }
    
    
    //  finish node and add it to our mesh tree
    auto meshNodePtr = std::unique_ptr<MeshNode>( new MeshNode );
    *meshNodePtr = std::move(outMeshNode);
    
    return meshNodePtr;
}

std::unique_ptr<SceneNode> parseColladaSceneNodeToData
(
    pugi::xml_node node,
    pugi::xml_node geometries,
    pugi::xml_node materials,
    pugi::xml_node effects
)
{
    SceneNode outNode;
    outNode.name = node.attribute("name").value();

    std::vector<float> mtxcells(sizeof(Matrix4::m)/sizeof(Matrix4::value_type));
    
    for (auto instance = node.first_child();
         instance;
         instance = instance.next_sibling())
    {
        const char* elemName = instance.name();
        if (!strcmp(elemName, "matrix")) {
            mtxcells = collada::xml_parse_float_array(instance, mtxcells.size());
            memcpy(outNode.mtx.m, mtxcells.data(), mtxcells.size() * sizeof(mtxcells[0]));
        }
        else if (!strcmp(elemName, "instance_geometry")) {
            const char* geomUrl = instance.attribute("url").value();
            auto geometry = collada::xml_find_node_from_ref(geometries, geomUrl, "geometry");
            
            //  geometry has to exist
            if (!geometry) {
                throw std::runtime_error(
                    std::string("node['") + outNode.name + "']." \
                                "instance_geometry['" + geomUrl + "'] not found"
                );
            }
            
            const char* matUrl = "";
            auto material = instance.child("bind_material");
            if (material) {
                material = material.child("technique_common").child("instance_material");
                
                //  NOTE: we're making an assumption that our material here will
                //  be the same as the material referenced by the geometry mesh
                //  node.  According to the spec for <instance_material>, this
                //  *should* be the case.
                matUrl = material.attribute("target").value();
                
                material = collada::xml_find_node_from_ref(materials, matUrl);
            }
            
            outNode.mesh = parseColladaGeometryToData(geometry);
        }
    }
    
    auto outNodePtr = std::unique_ptr<SceneNode>(new SceneNode);
    *outNodePtr = std::move(outNode);
    
    return outNodePtr;
}


//  Using the COLLADA 1.4 Reference and Specification
//  Tested with the following exporters:
//      Blender 2.75
//
Scene parseColladaToData
(
    const pugi::xml_document& collada,
    SceneParseType parseType,
    const std::string& typeName
)
{
    Scene outScene;
    
    if (parseType == kSceneParseInvalid) {
        throw std::runtime_error("Parse method is not supported");
    }
    
    //  validate we're reading a collada 1.4 doc
    auto docRoot = collada.document_element();
    if (strcasecmp(docRoot.name(), "COLLADA") != 0)
        throw std::runtime_error("Not a COLLADA document");
    const char* docVer = docRoot.attribute("version").value();
    if (strncmp(docVer, "1.4", 3) != 0) {
        throw std::runtime_error(
            std::string("Unsupported COLLADA version.  Required: 1.4, but found ")
                        + docVer
        );
    }
 
    //  parse scene nodes
    auto libraryScenes = collada.document_element().child("library_visual_scenes");
    if (!libraryScenes)
        throw std::runtime_error("No library_visual_scenes node found");

    auto libraryGeometries = collada.document_element().child("library_geometries");
    if (!libraryGeometries)
        throw std::runtime_error("No library_geometries node found");

    auto scene = libraryScenes.child("visual_scene");
    if (!scene)
        throw std::runtime_error("No visual_scene found in scene library");
    
    //  "optional" if missing, TODO : give warnings.
    auto libraryMaterials = collada.document_element().child("library_materials");
    auto libraryEffects = collada.document_element().child("library_effects");
    
    outScene.name = scene.attribute("name").value();
    
    if (parseType == kSceneParseNode) {
        auto sceneNode = scene.find_child_by_attribute("node", "name", typeName.c_str());
        if (sceneNode) {
            outScene.root = std::move(
                parseColladaSceneNodeToData(sceneNode,
                    libraryGeometries,
                    libraryMaterials,
                    libraryEffects)
            );
        }
    }
    
    
    //  parse geometries
    
    for (auto geometry = libraryGeometries.first_child();
         geometry;
         geometry = geometry.next_sibling())
    {
        auto mesh = geometry.child("mesh");
        if (!mesh)
            mesh = geometry.child("convex_mesh");
        
        if (mesh) {
            MeshNode outMeshNode;
            outMeshNode.name = geometry.attribute("name").value();
            const std::string runtimeContext = std::string("geometry['") + outMeshNode.name + "'].mesh ";
            
            auto polylist = mesh.child("polylist");
            
            //  construct triangles from positions and vertex normals
            auto vcounts = collada::xml_parse_int_array( polylist.child("vcount"),
                            polylist.attribute("count").as_int() );
            
            if (vcounts.empty() ||
                std::find_if_not(vcounts.begin(),
                                 vcounts.end(),
                                [](const int& v) -> bool { return v == 3; }) != vcounts.end()) {
                throw std::runtime_error(runtimeContext + "must be triangulated.");
            }
            
            size_t faceCount = vcounts.size();
            outMeshNode.triangles.resize(faceCount);
            
            
            //  read vertices
            auto vertexInput =
                polylist.find_child_by_attribute("input", "semantic", "VERTEX");
            int vertexInputIndexOffset = vertexInput.attribute("offset").as_int();
            int indicesPerVertex = 1;
            
            //  retrieve vertex positions (and possibly other vertex components
            //  except normals, which are handled separately?)
            auto vertexFmt = collada::xml_find_node_from_ref(mesh,
                vertexInput.attribute("source").value(),
                "vertices");
            auto positions = vertexFmt.find_child_by_attribute("input", "semantic", "POSITION");
            positions = collada::xml_find_node_from_ref(mesh,
                positions.attribute("source").value(),
                "source");
            
            if (!vertexFmt || !positions) {
                throw std::runtime_error(std::string(runtimeContext) + "has no vertices");
            }
            
            outMeshNode.points = collada::parse_source_into_points_vector(positions);
            
            //  import face normals
            auto normalInput =
                polylist.find_child_by_attribute("input", "semantic", "NORMAL");
            int normalInputIndexOffset = normalInput.attribute("offset").as_int();
            
            if (normalInput) {
                ++indicesPerVertex;
            }
            
            std::vector<Vector3> faceNormals;
            
            if (normalInput) {
                auto normals = collada::xml_find_node_from_ref(mesh,
                    normalInput.attribute("source").value(),
                    "source");
                faceNormals = collada::parse_source_into_points_vector(normals);
            }
            
            //  create faces
            
            auto indices = collada::xml_parse_int_array(polylist.child("p"),
                    std::accumulate(vcounts.begin(), vcounts.end(), 0) * indicesPerVertex);
        
            for (int faceIdx = 0, indicesIdx = 0;
                 faceIdx < faceCount;
                 ++faceIdx) {
                auto& triangle = outMeshNode.triangles[faceIdx];
                triangle.indices[0] = indices[indicesIdx + 0 * indicesPerVertex + vertexInputIndexOffset];
                triangle.indices[1] = indices[indicesIdx + 1 * indicesPerVertex + vertexInputIndexOffset];
                triangle.indices[2] = indices[indicesIdx + 2 * indicesPerVertex + vertexInputIndexOffset];
                int faceNormalIdx = indices[indicesIdx + 0 * indicesPerVertex + normalInputIndexOffset];
                triangle.normal = faceNormals[faceNormalIdx];
                indicesIdx += vcounts[faceIdx] * indicesPerVertex;
            }
            
            
            //  finish node and add it to our mesh tree
            auto meshNodePtr = std::unique_ptr<MeshNode>( new MeshNode );
            *meshNodePtr = std::move(outMeshNode);
            
        }
    }
    
    return outScene;
}

} /* namespace ovtools */ } /* namespace cinek */