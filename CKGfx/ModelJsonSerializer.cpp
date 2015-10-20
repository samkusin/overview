//
//  ModelJsonSerializer.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/29/15.
//
//

#include "ModelJsonSerializer.hpp"
#include "Context.hpp"

#include <vector>

namespace cinek {
    namespace gfx {


NodeElementCounts& enumerateNodeResourcesFromJSON
(
    NodeElementCounts& counts,
    const JsonValue& node
)
{
    ++counts.nodeCount;
    if (node.HasMember("matrix")) {
        ++counts.transformCount;
    }
    if (node.HasMember("meshes") && node["meshes"].IsArray()) {
        if (!node["meshes"].Empty()) {
            counts.meshElementCount += node["meshes"].Size();
            ++counts.nodeCount;
        }
    }
    if (node.HasMember("children") && node["children"].IsArray()) {
        const JsonValue& children = node["children"];
        for (auto it = children.Begin(); it != children.End(); ++it) {
            enumerateNodeResourcesFromJSON(counts, *it);
        }
    }
    return counts;
}


struct ModelBuilderFromJSONFn
{
    //  builder may add to meshes and materials per iteration.
    //  passing these vectors in on construction conveys that we want our
    //  builder to reference a common set of resources when generating
    //  references to meshes, materials, etc.
    //
    ModelBuilderFromJSONFn
    (
        Context& context,
        std::vector<std::pair<MeshHandle, MaterialHandle>>& meshes    ) :
        _context(&context),
        _meshes(&meshes)
    {
    }

    NodeGraph& operator()(NodeGraph& model, const JsonValue& node)
    {
        model.setRoot(build(model, node));
        
        return model;
    }

private:
    Context* _context;
    std::vector<std::pair<MeshHandle, MaterialHandle>>* _meshes;

    NodeHandle build(NodeGraph& model, const JsonValue& node)
    {
        NodeHandle thisNode = model.createTransformNode();
        loadMatrixFromJSON(thisNode->transform()->mtx, node["matrix"]);
        if (node.HasMember("obb")) {
            loadAABBFromJSON(thisNode->transform()->obb, node["obb"]);
        }
        
        //  create our leaf child element nodes (meshes for example)
        if (node.HasMember("meshes")) {
            const JsonValue& elements = node["meshes"];
            if (elements.IsArray() && !elements.Empty()) {
                NodeHandle meshNode = model.createMeshNode(elements.Size());
                MeshElement* meshElement = meshNode->mesh();
                for (auto it = elements.Begin(); it != elements.End(); ++it) {
                    CK_ASSERT(meshElement);
                    if (meshElement) {
                        auto& meshPair = _meshes->at(it->GetInt());
                        meshElement->mesh = meshPair.first;
                        meshElement->material = meshPair.second;
                        meshElement = meshElement->next;
                    }
                }
                
                model.addChildNodeToNode(meshNode, thisNode);
            }
        }
        
        if (node.HasMember("children")) {
            const JsonValue& children = node["children"];
            if (children.IsArray() && !children.Empty()) {

                for (auto it = children.Begin(); it != children.End(); ++it) {
                    model.addChildNodeToNode(build(model, *it), thisNode);
                }
            }
        }
        
        return thisNode;
    }
};


NodeGraph loadNodeGraphFromJSON(Context& context, const JsonValue& root)
{
    NodeGraph model;
    
    if (!root.IsObject() || !root.HasMember("nodes"))
        return model;
    
    const JsonValue& modelNode = root["nodes"];
    
    if (!modelNode.IsObject())
        return model;
    
    //  precalculate resources needed for the Model object.  to do this we need
    //  to run an enumeration pass on the model document before generating our
    //  model's graph
    
    NodeElementCounts modelInitParams = {};
    modelInitParams = enumerateNodeResourcesFromJSON(modelInitParams, modelNode);
    
    model = std::move(NodeGraph(modelInitParams));
    
    //  Build model's graph by visiting our json nodes.
    //
    std::vector<std::pair<MeshHandle, MaterialHandle>> meshesByIndex;
    
    //  create meshes
    if (root.HasMember("meshes")) {
        //  create new materials or lookup existing ones and add them to our
        //  local indexed array of materials
        if (root.HasMember("materials")) {
            const JsonValue& materialsJson = root["materials"];
            
            for (auto materialIt = materialsJson.MemberBegin();
                 materialIt != materialsJson.MemberEnd();
                 ++materialIt) {
                
                auto& materialJson = materialIt->value;
                const char* name = materialIt->name.GetString();
                auto material = context.findMaterial(name);
                if (!material) {
                    material =
                        context.registerMaterial(
                            std::move(loadMaterialFromJSON(context, materialJson)),
                            name
                        );
                }
            }
        }
        
        const JsonValue& meshesJson = root["meshes"];
        
        for (auto meshIt = meshesJson.Begin();
             meshIt != meshesJson.End();
             ++meshIt) {
            const JsonValue& meshJson = *meshIt;
            MaterialHandle material;
            if (meshJson.HasMember("material")) {
                material = context.findMaterial(meshJson["material"].GetString());
            }
            
            auto meshHandle = context.registerMesh(
                std::move(loadMeshFromJSON(context, meshJson))
            );
        
            meshesByIndex.emplace_back(
                meshHandle,
                material
            );
        }
    }
    
    ModelBuilderFromJSONFn buildFn(context, meshesByIndex);
    
    buildFn(model, modelNode);
    
    return model;
}

Mesh loadMeshFromJSON
(
    Context& context,
    const JsonValue& root
)
{
    Mesh mesh;
    
    const JsonValue& vertices = root["vertices"];
    const JsonValue& normals = root["normals"];
    
    JsonValue::ConstMemberIterator texArrayIt = root.FindMember("tex0");
   
    CK_ASSERT(vertices.Size() == normals.Size());
    CK_ASSERT(texArrayIt == root.MemberEnd() || texArrayIt->value.Size() == vertices.Size());
    
    const JsonValue& triangles = root["tris"];
    
    MeshBuilder::BuilderState meshBuilder;
    
    //  TODO - configurable based on mesh JSON format
    VertexTypes::Format vertexType = VertexTypes::kVec3_Normal_Tex0;
    meshBuilder.vertexDecl = &VertexTypes::declaration(vertexType);
    meshBuilder.indexLimit = triangles.Size() * 3;
    meshBuilder.indexType = VertexTypes::kIndex16;
    meshBuilder.vertexLimit = vertices.Size();
    
    MeshBuilder::create(meshBuilder);
    
    JsonValue::ConstValueIterator vertexIt = vertices.Begin();
    JsonValue::ConstValueIterator normalIt = normals.Begin();
    JsonValue::ConstValueIterator texIt = nullptr;
    bool hasTexCoords = texArrayIt != root.MemberEnd();
    if (hasTexCoords) {
        texIt = texArrayIt->value.Begin();
    }
    for (; vertexIt != vertices.End(); ++vertexIt, ++normalIt) {
        Vector3 vec3;
        meshBuilder.position(loadVectorFromJSON(vec3, *vertexIt))
                   .normal(loadVectorFromJSON(vec3, *normalIt));
        
        if (hasTexCoords) {
            Vector2 uv;
            meshBuilder.uv2(loadUVFromJSON(uv, *texIt));
            ++texIt;
        }
        
        meshBuilder.next();
    }
    
    JsonValue::ConstValueIterator triIt = triangles.Begin();
    for (; triIt != triangles.End(); ++triIt) {
        const JsonValue& triangleArray = *triIt;
        CK_ASSERT(triangleArray.IsArray() && triangleArray.Size() == 3);
        uint16_t i0 = (uint16_t)triangleArray[0U].GetInt();
        uint16_t i1 = (uint16_t)triangleArray[1U].GetInt();
        uint16_t i2 = (uint16_t)triangleArray[2U].GetInt();
        meshBuilder.triangle<uint16_t>(i0,i1,i2);
    }
    
    return Mesh(vertexType, meshBuilder.indexType,
                    meshBuilder.vertexMemory,
                    meshBuilder.indexMemory);
}

Material loadMaterialFromJSON(Context& context, const JsonValue& root)
{
    Material material;
 
    if (root.HasMember("diffuse")) {
        auto& diffuse = root["diffuse"];
        loadColorFromJSON(material.diffuseColor, diffuse);
        
        auto& textures = diffuse["textures"];
        if (textures.IsArray() && textures.Size() > 0) {
            const char* texname = textures.Begin()->GetString();
            auto texhandle = context.findTexture(texname);
            if (!texhandle) {
                texhandle = context.loadTexture(texname, texname);
            }
            material.diffuseTex = texhandle;
        }
    }
    
    if (root.HasMember("specular")) {
        auto& specular = root["specular"];
        loadColorFromJSON(material.specularColor, specular);
        material.specularPower = (float)specular["power"].GetDouble();
    }
    
    return material;
}


Matrix4& loadMatrixFromJSON(Matrix4& mtx, const JsonValue& mtxArray)
{
    //  column major
    CK_ASSERT_RETURN_VALUE(mtxArray.IsArray(), mtx);
    int i = 0;
    for (auto it = mtxArray.Begin(); it != mtxArray.End() && i < 16; ++it, ++i) {
        mtx[i] = (Matrix4::value_type)it->GetDouble();
    }
    
    return mtx;
}

ckm::AABB<Vector3>& loadAABBFromJSON(ckm::AABB<Vector3>& aabb, const JsonValue& aabbObj)
{
    CK_ASSERT_RETURN_VALUE(aabbObj.IsObject(), aabb);
    
    const JsonValue& minObj = aabbObj["min"];
    const JsonValue& maxObj = aabbObj["max"];
    
    aabb.min = loadVectorFromJSON(aabb.min, minObj);
    aabb.max = loadVectorFromJSON(aabb.max, maxObj);
    
    return aabb;
}

Vector3& loadVectorFromJSON(Vector3& vec, const JsonValue& vecObj)
{
    CK_ASSERT_RETURN_VALUE(vecObj.IsObject(), vec);
    vec.x = (Vector3::value_type)vecObj["x"].GetDouble();
    vec.y = (Vector3::value_type)vecObj["y"].GetDouble();
    vec.z = (Vector3::value_type)vecObj["z"].GetDouble();
    return vec;
}

Vector4& loadVectorFromJSON(Vector4& vec, const JsonValue& vecObj)
{
    CK_ASSERT_RETURN_VALUE(vecObj.IsObject(), vec);
    vec.x = (Vector3::value_type)vecObj["x"].GetDouble();
    vec.y = (Vector3::value_type)vecObj["y"].GetDouble();
    vec.z = (Vector3::value_type)vecObj["z"].GetDouble();
    vec.w = (Vector3::value_type)vecObj["w"].GetDouble();

    return vec;
}

Color3& loadColorFromJSON(Color3& vec, const JsonValue& colObj)
{
    CK_ASSERT_RETURN_VALUE(colObj.IsObject(), vec);
    vec.r = (Color3::value_type)colObj["r"].GetDouble();
    vec.g = (Color3::value_type)colObj["g"].GetDouble();
    vec.b = (Color3::value_type)colObj["b"].GetDouble();
    return vec;
}

Color4& loadColorFromJSON(Color4& vec, const JsonValue& colObj)
{
    CK_ASSERT_RETURN_VALUE(colObj.IsObject(), vec);
    vec.r = (Color3::value_type)colObj["r"].GetDouble();
    vec.g = (Color3::value_type)colObj["g"].GetDouble();
    vec.b = (Color3::value_type)colObj["b"].GetDouble();
    if (colObj.HasMember("a")) {
        vec.a = (Color3::value_type)colObj["a"].GetDouble();
    }
    else {
        vec.a = 1.0f;
    }
    return vec;
}

Vector2& loadUVFromJSON(Vector2& vec, const JsonValue& uvObj)
{
    CK_ASSERT_RETURN_VALUE(uvObj.IsObject(), vec);
    vec.u = (Color3::value_type)uvObj["u"].GetDouble();
    vec.v = (Color3::value_type)uvObj["v"].GetDouble();
    return vec;
}

    }   //  namespace gfx
}   //  namespace cinek
