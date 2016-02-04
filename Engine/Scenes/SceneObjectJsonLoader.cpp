//
//  SceneJsonLoader.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/5/15.
//
//

#include "SceneObjectJsonLoader.hpp"
#include "SceneDataContext.hpp"

#include "CKGfx/ModelJsonSerializer.hpp"

#include <bx/fpumath.h>

namespace cinek {
    namespace ove {
    
template<> float* loadVectorFromJSON(float* vec, const JsonValue& vecObj)
{
    CK_ASSERT_RETURN_VALUE(vecObj.IsObject(), vec);
    vec[0] = (float)vecObj["x"].GetDouble();
    vec[1] = (float)vecObj["y"].GetDouble();
    vec[2] = (float)vecObj["z"].GetDouble();
    return vec;
}

template<> double* loadVectorFromJSON(double* vec, const JsonValue& vecObj)
{
    CK_ASSERT_RETURN_VALUE(vecObj.IsObject(), vec);
    vec[0] = vecObj["x"].GetDouble();
    vec[1] = vecObj["y"].GetDouble();
    vec[2] = vecObj["z"].GetDouble();
    return vec;
}

template<> btVector3* loadVectorFromJSON(btVector3* vec, const JsonValue& vecObj)
{
    CK_ASSERT_RETURN_VALUE(vecObj.IsObject(), vec);
    
    (*vec).setValue((btScalar)vecObj["x"].GetDouble(),
        (btScalar)vecObj["y"].GetDouble(),
        (btScalar)vecObj["z"].GetDouble());
    
    return vec;
}


btBvhTriangleMeshShape* SceneObjectJsonLoader::operator()
(
    const JsonValue& jsonNode
)
{
    btBvhTriangleMeshShape* shape = nullptr;

    auto jsonNodeHullsArrayIt = jsonNode.FindMember("hulls");
    if (jsonNodeHullsArrayIt == jsonNode.MemberEnd())
        return nullptr;
    
    auto& jsonNodeHulls = jsonNodeHullsArrayIt->value;
    
    //  only supporting first shape in hulls array
    //  we may need to support multiple hulls per shape - but for now
    //  just support one.
    
    int hullIndex = jsonNodeHulls[0U].GetInt();
    if (hullIndex >= hulls.size()) {
        hulls.resize(hullIndex+1);
    }
    if (!hulls[hullIndex]) {
        auto& jsonHulls = jsonHullsArrayIt->value;
        if (hullIndex < jsonHulls.Size()) {
            auto& jsonHull = jsonHulls[hullIndex];
            
            const char* hullName = jsonHull["name"].GetString();
            auto hull = context->acquireFixedBodyHull(hullName);
            if (!hull) {
                hull = loadSceneFixedBodyHullFromJSON(*context, jsonHull);
            }
            
            hulls[hullIndex] = hull;
        }
    }
  
    //  get local scaling from our node's transform (affline, decomposition)
    gfx::Matrix4 transform;
    gfx::loadMatrixFromJSON(transform, jsonNode["matrix"]);
    
    btVector3 localScaling;
    localScaling.setX(bx::vec3Length(&transform.comp[0]));
    localScaling.setY(bx::vec3Length(&transform.comp[4]));
    localScaling.setZ(bx::vec3Length(&transform.comp[8]));
    
    shape = context->allocateTriangleMeshShape(hulls[hullIndex], localScaling);
      
    return shape;
}

SceneFixedBodyHull* loadSceneFixedBodyHullFromJSON
(
    SceneDataContext& context,
    const JsonValue& root
)
{
    SceneFixedBodyHull* hull = nullptr;
    
    const JsonValue& jsonVertices = root["vertices"];
    const JsonValue& jsonTris = root["tris"];

    SceneFixedBodyHull::VertexIndexCount vertIdxCounts;
    vertIdxCounts.numFaces = jsonTris.Size();
    vertIdxCounts.numVertices = jsonVertices.Size();
    
    const char* name = root["name"].GetString();
    
    hull = context.allocateFixedBodyHull(vertIdxCounts, name);
    if (hull) {
        float* vertices = hull->pullVertices(vertIdxCounts.numVertices);
        JsonValue::ConstValueIterator jsonVertexIt = jsonVertices.Begin();
        for (; jsonVertexIt != jsonVertices.End(); ++jsonVertexIt) {
            loadVectorFromJSON<float>(vertices, *jsonVertexIt);
            vertices += 3;
        }
       
        int* indices = hull->pullFaceIndices(vertIdxCounts.numFaces);
    
        JsonValue::ConstValueIterator jsonTriIt = jsonTris.Begin();
        for (; jsonTriIt != jsonTris.End(); ++jsonTriIt) {
            const JsonValue& jsonIdxArray = *jsonTriIt;
            CK_ASSERT(jsonIdxArray.IsArray() && jsonIdxArray.Size() == 3);
            indices[0] = jsonIdxArray[0U].GetInt();
            indices[1] = jsonIdxArray[1U].GetInt();
            indices[2] = jsonIdxArray[2U].GetInt();
            indices += 3;
        }
        
        hull->finalize();
    }
    
    return hull;
}


    
    } /* namespace ove */
} /* namespace cinek */
