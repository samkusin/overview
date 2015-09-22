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

struct Point
{
    typedef float value_type;
    value_type x;
    value_type y;
    value_type z;
};

typedef Point Vector3;

struct Matrix4
{
    typedef float value_type;
    value_type m[16];
};

struct MeshTriFace
{
    int indices[3];
    Vector3 normal;
};

struct MeshNode
{
public:
    std::string name;
    std::vector<Point> points;
    std::vector<MeshTriFace> triangles;
};

struct SceneNode
{
    std::string name;
    Matrix4 mtx;
    std::unique_ptr<MeshNode> mesh;
};

struct Scene
{
    std::string name;
    std::unique_ptr<SceneNode> root;
};

enum SceneParseType
{
    kSceneParseInvalid,
    kSceneParseNode
};


Scene parseColladaToData(const pugi::xml_document& collada, SceneParseType type,
                         const std::string& typeName);

} /* namespace ovtools */ } /* namespace cinek */

#endif
