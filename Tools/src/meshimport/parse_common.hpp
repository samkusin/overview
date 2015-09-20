//
//  parse_common.hpp
//  meshimport
//
//  Created by Samir Sinha on 9/19/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Tools_MeshImp_ParseCommon_h
#define Overview_Tools_MeshImp_ParseCommon_h

#include <vector>
#include <memory>
#include <string>

namespace pugi {
    class xml_document;
}

namespace cinek { namespace ovtools {

struct MeshPoint
{
    float x;
    float y;
    float z;
};

typedef MeshPoint MeshVector;
typedef int MeshIndex;

struct MeshTri
{
    MeshIndex indices[3];
    MeshVector normal;
};


struct MeshNode
{
public:
    std::string id;
    std::string name;
    std::vector<MeshPoint> points;
    std::vector<MeshTri> triangles;
};

struct MeshTree
{
    std::unique_ptr<MeshNode> root;
};


MeshTree parseColladaToData(const pugi::xml_document& collada);

} /* namespace ovtools */ } /* namespace cinek */

#endif
