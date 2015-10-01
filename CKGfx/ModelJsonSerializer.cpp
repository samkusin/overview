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
        counts.meshElementCount += node["meshes"].Size();
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
        std::vector<MeshHandle>& meshes,
        std::vector<MaterialHandle>& materials
    ) :
        _context(&context),
        _meshes(&meshes),
        _materials(&materials)
    {
    }

    Model& operator()(Model& model, const JsonValue& node)
    {
        build(model, node);
        
        return model;
    }

private:
    Context* _context;
    std::vector<MeshHandle>* _meshes;
    std::vector<MaterialHandle>* _materials;

    NodeHandle build(Model& model, const JsonValue& node)
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
                        meshElement->mesh = _meshes->at(it->GetInt());
                        meshElement->material = _materials->at(it->GetInt());
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


Model loadModelFromJSON(Context& context, const JsonValue& root)
{
    Model model;
    
    if (!root.IsObject() || !root.HasMember("node"))
        return model;
    
    const JsonValue& modelNode = root["node"];
    
    if (!modelNode.IsObject())
        return model;
    
    //  precalculate resources needed for the Model object.  to do this we need
    //  to run an enumeration pass on the model document before generating our
    //  model's graph
    
    NodeElementCounts modelInitParams = {};
    modelInitParams = enumerateNodeResourcesFromJSON(modelInitParams, modelNode);
    
    model = std::move(Model(modelInitParams));
    
    //  Build model's graph by visiting our json nodes.
    //
    std::vector<MeshHandle> meshesByIndex;
    std::vector<MaterialHandle> materialsByIndex;
    
    //  create meshes
    //  create materials only if there's no existing material with its name
    if (root.HasMember("meshes") && root.HasMember("materials")) {
        const JsonValue& meshes = root["meshes"];
        const JsonValue& materials = root["materials"];
        
        
    }
    
    ModelBuilderFromJSONFn buildFn(context, meshesByIndex, materialsByIndex);
    
    buildFn(model, modelNode);
    
    return model;
}

Mesh loadMeshFromJSON(Context& context, const JsonValue& root)
{
    Mesh mesh;
    
    return mesh;
}

Material loadMaterialFromJSON(Context& context, const JsonValue& root)
{
    Material material;
    
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

AABB<Vector3>& loadAABBFromJSON(AABB<Vector3>& aabb, const JsonValue& aabbObj)
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
    vec.z = (Vector3::value_type)vecObj["y"].GetDouble();
    return vec;
}

Vector4& loadVectorFromJSON(Vector4& vec, const JsonValue& vecObj)
{
    CK_ASSERT_RETURN_VALUE(vecObj.IsObject(), vec);
    vec.x = (Vector3::value_type)vecObj["x"].GetDouble();
    vec.y = (Vector3::value_type)vecObj["y"].GetDouble();
    vec.z = (Vector3::value_type)vecObj["y"].GetDouble();
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
    vec.a = (Color3::value_type)colObj["a"].GetDouble();
    return vec;
}


    }   //  namespace gfx
}   //  namespace cinek
