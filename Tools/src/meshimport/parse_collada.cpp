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
    
    static std::vector<MeshPoint> copy_floats_into_points_vector
    (
        const std::vector<float>& values,
        int count,
        int offset,
        int stride
    )
    {
        std::vector<MeshPoint> outPoints;
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

    static std::vector<MeshPoint> parse_source_into_points_vector
    (
        pugi::xml_node source
    )
    {
        std::vector<MeshPoint> outMeshPoints;
        
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

//  Using the COLLADA 1.4 Reference and Specification
//  Tested with the following exporters:
//      Blender 2.75
//
MeshTree parseColladaToData(const pugi::xml_document& collada)
{
    MeshTree outMeshTree;
    
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
 
    //  parse geometries
    auto libraryGeometries = collada.document_element().child("library_geometries");
    if (!libraryGeometries)
        throw std::runtime_error("No library_geometries node found");
    
    for (auto geometry = libraryGeometries.first_child();
         geometry;
         geometry = geometry.next_sibling())
    {
        auto mesh = geometry.child("mesh");
        if (!mesh)
            mesh = geometry.child("convex_mesh");
        
        if (mesh) {
            MeshNode outMeshNode;
            outMeshNode.id = geometry.attribute("id").value();
            outMeshNode.name = geometry.attribute("name").value();
            const std::string runtimeContext = std::string("geometry['") + outMeshNode.id + "'].mesh ";
            
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
            
            std::vector<MeshVector> faceNormals;
            
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
            
            if (!outMeshTree.root) {
                outMeshTree.root = std::move(meshNodePtr);
            }
        }
    }
    
    return outMeshTree;
}

} /* namespace ovtools */ } /* namespace cinek */