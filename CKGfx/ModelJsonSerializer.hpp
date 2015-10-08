//
//  ModelJsonSerializer.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/29/15.
//
//

#ifndef CK_Graphics_ModelJsonSerializer_hpp
#define CK_Graphics_ModelJsonSerializer_hpp

#include "GfxTypes.hpp"
#include "NodeGraph.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

#include <ckjson/json.hpp>
#include <vector>

namespace cinek {
    namespace gfx {


NodeGraph loadNodeGraphFromJSON(Context& context, const JsonValue& root);
Mesh loadMeshFromJSON(Context& context, const JsonValue& root);
Material loadMaterialFromJSON(Context& context, const JsonValue& root);

Matrix4& loadMatrixFromJSON(Matrix4& mtx, const JsonValue& mtxArray);
ckm::AABB<Vector3>& loadAABBFromJSON(ckm::AABB<Vector3>& aabb, const JsonValue& aabbObj);
Vector3& loadVectorFromJSON(Vector3& vec, const JsonValue& vecObj);
Vector4& loadVectorFromJSON(Vector4& vec, const JsonValue& vecObj);
Color3& loadColorFromJSON(Color3& vec, const JsonValue& colObj);
Color4& loadColorFromJSON(Color4& vec, const JsonValue& colObj);
Vector2& loadUVFromJSON(Vector2& vec, const JsonValue& vecObj);

    }   //  namespace gfx
}   //  namespace cinek

#endif