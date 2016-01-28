//
//  ModelJsonSerializer.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/29/15.
//
//

#include "ModelJsonSerializer.hpp"
#include "Context.hpp"
#include "Geometry.hpp"

#include <cinek/debug.h>
#include <bx/fpumath.h>

#include <vector>

namespace cinek {
    namespace gfx {
    
static const char* sKeyframeTypes[Keyframe::kTypeCount] = {
    "tx", "ty", "tz",
    "qw", "qx", "qy", "qz",
    "rx", "ry", "rz",
    "sx", "sy", "sz"
};

static Keyframe::Type keyframeTypeFromString(const char* str)
{
    for (int i = 0; i < Keyframe::kTypeCount; ++i) {
        if (!strcmp(sKeyframeTypes[i], str))
            return static_cast<Keyframe::Type>(i);
    }
    return Keyframe::kInvalid;
}


NodeElementCounts& enumerateNodeResourcesFromJSON
(
    NodeElementCounts& counts,
    const JsonValue& node
)
{
    if (node.HasMember("meshes") && node["meshes"].IsArray()) {
        if (!node["meshes"].Empty()) {
            counts.meshNodeCount += node["meshes"].Size();
        }
    }
    else if (node.HasMember("animation")) {
        ++counts.armatureNodeCount;
    }
    else if (node.HasMember("light")) {
        ++counts.lightNodeCount;
    }
    else {
        ++counts.transformNodeCount;
    }
    if (node.HasMember("children") && node["children"].IsArray()) {
        const JsonValue& children = node["children"];
        for (auto it = children.Begin(); it != children.End(); ++it) {
            enumerateNodeResourcesFromJSON(counts, *it);
        }
    }
    return counts;
}

/*
struct NodeJsonLoader
{
    gfx::Context* context;
    const JsonValue* jsonMeshes;
    const JsonValue* jsonMaterials;
    const JsonValue* jsonAnimations;
    const JsonValue* jsonLights;
    
    std::vector<std::pair<MeshHandle, MaterialHandle>> meshes;
    std::vector<LightHandle> lights;
    
    NodeHandle createNodeFromJSON(NodeGraph& nodeGraph, const JsonValue& jsonNode);    
};
*/

NodeHandle NodeJsonLoader::operator()
(
    NodeGraph& nodeGraph,
    const JsonValue &jsonNode
)
{
    NodeHandle thisNode;
    
    const char* nodeName = jsonNode["name"].GetString();
    const char* nodeType = nullptr;
    
    if (jsonNode.HasMember("type")) {
        nodeType = jsonNode["type"].GetString();
    }
    
    //  create our leaf child element nodes (meshes for example)
    if (jsonNode.HasMember("meshes")) {
        const JsonValue& elements = jsonNode["meshes"];
        if (elements.IsArray() && !elements.Empty()) {
            thisNode = nodeGraph.createMeshNode(elements.Size());
            MeshElement* meshElement = thisNode->mesh();
            for (auto it = elements.Begin(); it != elements.End(); ++it) {
                CK_ASSERT(meshElement);
                if (meshElement) {
                    unsigned meshIndex = it->GetInt();
                    if (meshIndex >= meshes.size()) {
                        meshes.resize(meshIndex+1);
                    }
                    //  create new mesh/material entry, which involves
                    //  parsing the requisite elements if necessary
                    //
                    auto& jsonMesh = jsonMeshes->value[meshIndex];
                    auto& meshPair = meshes.at(meshIndex);
                    if (!meshPair.second) {
                        // find or create material
                        const char* materialName = jsonMesh["material"].GetString();
                        auto materialHandle = context->findMaterial(materialName);
                        if (!materialHandle) {
                            auto jsonMaterialIt = jsonMaterials->value.FindMember(materialName);
                            if (jsonMaterialIt != jsonMaterials->value.MemberEnd()) {
                                materialHandle = context->registerMaterial(
                                    std::move(loadMaterialFromJSON(*context, jsonMaterialIt->value)),
                                    jsonMaterialIt->name.GetString());
                            }
                        }
                        meshPair.second = materialHandle;
                    }
                    if (!meshPair.first) {
                        meshPair.first =  context->registerMesh(
                            std::move(loadMeshFromJSON(*context, jsonMesh))
                        );
                    }
                    meshElement->mesh = meshPair.first;
                    meshElement->material = meshPair.second;
                    meshElement = meshElement->next;
                }
            }
        }
    }
    else if (jsonNode.HasMember("animation")) {
        const char* animationSetName = jsonNode["animation"].GetString();
        thisNode = nodeGraph.createArmatureNode();
        
        auto animationSetHandle = context->findAnimationSet(animationSetName);
        if (!animationSetHandle) {
            auto objIt = jsonAnimations->value.FindMember(animationSetName);
            auto& jsonObject = objIt->value;
            const char* name = objIt->name.GetString();
            animationSetHandle = context->registerAnimationSet(
                    std::move(loadAnimationSetFromJSON(*context, jsonObject)),
                    name
                );
        }

        thisNode->armature()->animSet = animationSetHandle;
    }
    else if (jsonNode.HasMember("light")) {
        thisNode = nodeGraph.createLightNode();
        
        unsigned lightIndex = jsonNode["light"].GetInt();
        if (lightIndex >= lights.size()) {
            lights.resize(lightIndex+1);
        }
        //  create new light entry, which involves parsing the requisite elements
        //  if necessary
        //
        auto lightHandle = lights.at(lightIndex);
        if (!lightHandle) {
            auto& jsonLight = jsonLights->value[lightIndex];
            lightHandle = context->registerLight(
                    std::move(loadLightFromJSON(*context, jsonLight)));
            lights[lightIndex] = lightHandle;
        }
        thisNode->light()->light = lightHandle;
    }
    else {
        thisNode = nodeGraph.createObjectNode(0);
    }
    
    loadMatrixFromJSON(thisNode->transform(), jsonNode["matrix"]);
    
    if (jsonNode.HasMember("obb")) {
        loadAABBFromJSON(thisNode->obb(), jsonNode["obb"]);
    }

    if (nodeType && (!strcasecmp(nodeType, "entity") || !strcasecmp(nodeType, "model"))) {
        modelNodes.emplace_back(nodeName, thisNode);
    }

    return thisNode;
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
        NodeJsonLoader& loader
    ) :
        _loader(&loader)
    {
    }

    NodeGraph& operator()(NodeGraph& model, const JsonValue& node)
    {
        auto root = build(model, node);
    
        model.setRoot(root);
        
        return model;
    }

private:
    NodeJsonLoader* _loader;

    NodeHandle build
    (
        NodeGraph& model,
        const JsonValue& node
    )
    {
        NodeHandle thisNode;
        
        thisNode = (*_loader)(model, node);
        
        if (node.HasMember("children")) {
            const JsonValue& children = node["children"];
            if (children.IsArray() && !children.Empty()) {

                for (auto it = children.Begin(); it != children.End(); ++it) {
                    NodeHandle child = build(model, *it);
                    if (child) {
                        model.addChildNodeToNode(child, thisNode);
                        thisNode->obb().merge(child->calculateAABB());
                    }
                }
            }
        }
        
        return thisNode;
    }
};


NodeGraph loadNodeGraphFromJSON
(
    NodeJsonLoader& loader,
    const JsonValue& root
)
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
    loader.jsonAnimations = root.FindMember("animations");
    loader.jsonMeshes = root.FindMember("meshes");
    loader.jsonMaterials = root.FindMember("materials");
    loader.jsonLights = root.FindMember("lights");
    
    loader.meshes.reserve(loader.jsonMeshes->value.Size());
    if (loader.jsonLights != root.MemberEnd() && !loader.jsonLights->value.Empty()) {
        loader.lights.reserve(loader.jsonLights->value.Size());
    }

    ModelBuilderFromJSONFn buildFn(loader);
    
    buildFn(model, modelNode);
    
    return model;
}

ModelSet loadModelSetFromJSON
(
    Context& context,
    const JsonValue& root
)
{
    ModelSet modelSet;
    
    NodeJsonLoader loader;
    loader.context = &context;
    
    NodeGraph nodeGraph = loadNodeGraphFromJSON(loader, root);
    
    //  retranslate all model nodes to X,Z = (0,0) Y is maintained
    for (auto& modelNode : loader.modelNodes) {
        gfx::Matrix4& nodeTransform = modelNode.second->transform();
        nodeTransform.comp[12] = 0.0f;
        nodeTransform.comp[14] = 0.0f;
    }
    
    
    modelSet = std::move(ModelSet(std::move(nodeGraph), std::move(loader.modelNodes)));
    
    return std::move(modelSet);
}

NodeGraph loadNodeGraphFromJSON
(
    Context& context,
    const JsonValue& root
)
{
    NodeGraph nodeGraph;
    
    NodeJsonLoader loader;
    loader.context = &context;
    
    nodeGraph = loadNodeGraphFromJSON(loader, root);
    
    return std::move(nodeGraph);
}

Mesh loadMeshFromJSON
(
    Context& context,
    const JsonValue& root
)
{
    Mesh mesh;
    
    const JsonValue& vertices = root["vertices"];
    
    JsonValue::ConstMemberIterator normalArrayIt = root.FindMember("normals");
    JsonValue::ConstMemberIterator texArrayIt = root.FindMember("tex0");
    JsonValue::ConstMemberIterator weightsArrayIt = root.FindMember("weights");
    
    bool hasTexCoords = texArrayIt != root.MemberEnd();
    bool hasWeights = weightsArrayIt != root.MemberEnd();
    bool hasNormals = normalArrayIt != root.MemberEnd();
    
    CK_ASSERT(normalArrayIt == root.MemberEnd() || normalArrayIt->value.Size() == vertices.Size());
    CK_ASSERT(texArrayIt == root.MemberEnd() || texArrayIt->value.Size() == vertices.Size());
    CK_ASSERT(weightsArrayIt == root.MemberEnd() || weightsArrayIt->value.Size() == vertices.Size());
    
    const JsonValue& triangles = root["tris"];
    
    VertexTypes::Format vertexType = VertexTypes::kInvalid;
    if (hasNormals) {
        if (hasTexCoords) {
            if (hasWeights) {
                vertexType = VertexTypes::kVNormal_Tex0_Weights;
            }
            else {
                vertexType = VertexTypes::kVNormal_Tex0;
            }
        }
        else {
            vertexType = VertexTypes::kVPositionNormal;
        }
    }
    else {
        if (hasTexCoords) {
            vertexType = VertexTypes::kVTex0;
        }
        else {
            vertexType = VertexTypes::kVPosition;
        }
    }
    
    if (vertexType == VertexTypes::kInvalid) {
        return mesh;
    }
    
    MeshBuilder::BuilderState meshBuilder;
    
    int32_t indexLimit = (int)triangles.Size() * 3;
    CK_ASSERT(indexLimit <= UINT16_MAX);
    
    meshBuilder.vertexDecl = &VertexTypes::declaration(vertexType);
    meshBuilder.indexType = VertexTypes::kIndex16;
    
    MeshBuilder::create(meshBuilder, { (int)vertices.Size(), indexLimit });
    
    JsonValue::ConstValueIterator vertexIt = vertices.Begin();
    JsonValue::ConstValueIterator normalIt = nullptr;
    JsonValue::ConstValueIterator texIt = nullptr;
    JsonValue::ConstValueIterator weightsIt = nullptr;

    if (hasNormals) {
        normalIt = normalArrayIt->value.Begin();
    }
    if (hasTexCoords) {
        texIt = texArrayIt->value.Begin();
    }
    if (hasWeights) {
        weightsIt = weightsArrayIt->value.Begin();
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
     
        if (hasWeights) {
            const JsonValue& weights = *weightsIt;
            for(JsonValue::ConstValueIterator weightIt = weights.Begin();
                weightIt != weights.End();
                ++weightIt)
            {
                const JsonValue& weight = *weightIt;
                meshBuilder.addweight((uint16_t)weight["bidx"].GetUint(),
                                      (float)weight["weight"].GetDouble());
            }
            meshBuilder.endweights();
            ++weightsIt;
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
                    meshBuilder.indexMemory,
                    PrimitiveType::kTriangles);
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
                texhandle = context.loadTexture(texname);
            }
            material.diffuseTex = texhandle;
        }
    }
    
    if (root.HasMember("specular")) {
        auto& specular = root["specular"];
        loadColorFromJSON(material.specularColor, specular);
        material.specularPower = (float)specular["power"].GetDouble();
        material.specularIntensity = (float)specular["intensity"].GetDouble();
    }
    
    return material;
}

int loadAnimationSkeletonFromJSON
(
    const JsonValue& node,
    std::vector<Bone, std_allocator<Bone>>& bones
)
{
    unsigned index = node["bone_index"].GetInt();
    if (index >=bones.size())
        bones.resize(index);

    bones[index].name = node["name"].GetString();
    loadMatrixFromJSON(bones[index].mtx, node["matrix"]);
    
    bx::mtxInverse(bones[index].invMtx, bones[index].mtx);
    
    auto& children = node["children"];
    int lastChildIndex = -1;
    for (auto it = children.Begin(); it != children.End(); ++it) {
        auto& child = *it;
        int childIndex = loadAnimationSkeletonFromJSON(child, bones);
        if (lastChildIndex < 0) {
            bones[index].firstChild = childIndex;
            lastChildIndex = bones[index].firstChild;
        }
        else {
            bones[lastChildIndex].nextSibling = childIndex;
            lastChildIndex = bones[lastChildIndex].nextSibling;
        }
        bones[lastChildIndex].parent = index;
    }
    
    return index;
}

SequenceChannel loadSequenceChannelFromJSON
(
    const JsonValue& boneAnim,
    float* duration
)
{
    SequenceChannel sequenceChannel;
    
    for (auto jsonSeqIt = boneAnim.MemberBegin();
         jsonSeqIt != boneAnim.MemberEnd();
         ++jsonSeqIt) {
        auto& value = jsonSeqIt->value;
        if (!value.IsArray()) {
            continue;
        }
        Keyframe::Type kfType = keyframeTypeFromString(jsonSeqIt->name.GetString());
        if (kfType != Keyframe::kInvalid) {
            Sequence& seq = sequenceChannel.sequences[kfType];
            if (!seq.empty()) {
                CK_LOG_WARN("gfx", "loadSequenceChannelFromJson - "
                    "sequence '%s' already loaded",
                    jsonSeqIt->name.GetString());
            }
            seq.reserve(value.Size());
            for (auto jsonKfIt = value.Begin(); jsonKfIt != value.End(); ++jsonKfIt) {
                auto& kf = *jsonKfIt;
                float t = (float)kf["t"].GetDouble();
                seq.emplace_back(Keyframe{
                    t, (float)kf["v"].GetDouble()
                });
                if (t > *duration)
                    *duration = t;
            }
            if (seq.size() > 1) {
                //  more than one keyframe indicates animation
                sequenceChannel.animatedSeqMask |= (1 << kfType);
            }
        }
    }
    
    return sequenceChannel;
}


static int enumerateBonesInAnimationSet
(
    const JsonValue& bone,
    int maxIndex
)
{
    auto& children = bone["children"];
    for (auto it = children.Begin(); it != children.End(); ++it) {
        maxIndex = enumerateBonesInAnimationSet(*it, maxIndex);
    }

    int index = bone["bone_index"].GetInt();
    return index > maxIndex ? index : maxIndex;
}

AnimationSet loadAnimationSetFromJSON(Context& context, const JsonValue& root)
{
    std::vector<Bone, std_allocator<Bone>> bones;
    std::vector<AnimationSet::StateDefinition,
                std_allocator<AnimationSet::StateDefinition>> stateDefs;
    
    if (root.HasMember("skeleton")) {
        auto& rootBone = root["skeleton"][0U];
        
        int sz = enumerateBonesInAnimationSet(rootBone, -1) + 1;
        if (sz > 0) {
            bones.resize(sz);
            loadAnimationSkeletonFromJSON(rootBone, bones);
        }
    }
    
    if (root.HasMember("states")) {
        auto& states = root["states"];
        auto stateCnt = states.MemberEnd() - states.MemberBegin();
        stateDefs.reserve(stateCnt);
        for (auto it = states.MemberBegin(); it != states.MemberEnd(); ++it) {
            auto& state = it->value;
            const char* name = it->name.GetString();
            
            
            Animation animation;
            //  channel = animation for a bone
            animation.channels.resize(bones.size());
            animation.duration = 0.f;
            

            for (auto nodeIt = state.MemberBegin(); nodeIt != state.MemberEnd(); ++nodeIt) {
                auto& boneAnim = nodeIt->value;
                const char* boneName = nodeIt->name.GetString();
                auto boneIt = std::find_if(bones.begin(), bones.end(),
                    [&boneName](const Bone& b) -> bool {
                        return b.name == boneName;
                    });
                if (boneIt != bones.end()) {
                    animation.channels[boneIt - bones.begin()] =
                        std::move(loadSequenceChannelFromJSON(boneAnim, &animation.duration));
                }
            }
            
            stateDefs.emplace_back(name, std::move(animation));
        }
    }
    
    return AnimationSet(std::move(bones), std::move(stateDefs));
}

Light loadLightFromJSON(Context& context, const JsonValue& root)
{
    Light light;
    
    const char* type = root["type"].GetString();
    if (!strcmp(type, "spot")) {
        light.type = LightType::kSpot;
    }
    else if (!strcmp(type, "directional")) {
        light.type = LightType::kDirectional;
    }
    else if (!strcmp(type, "point")) {
        light.type = LightType::kPoint;
    }
    else if (!strcmp(type, "ambient")) {
        light.type = LightType::kAmbient;
    }
    else {
        CK_LOG_WARN("gfx", "loadLightFromJSON - Invalid Type %s", type);
        light.type = LightType::kNone;
    }
    
    if (light.type == LightType::kNone)
        return light;
    
    if (light.type == LightType::kAmbient) {
        light.ambientComp = 1.0f;
        light.diffuseComp = 0.0f;
    } else {
        light.ambientComp = 0.0f;
        light.diffuseComp = 1.0f;
    }
    
    Color4 color;
    loadColorFromJSON(color, root["color"]);
    float intensity = (float)root["intensity"].GetDouble();
    
    color.r *= intensity;
    color.g *= intensity;
    color.b *= intensity;
    light.color = color.toABGR();
    
    if (light.type == LightType::kPoint || light.type == LightType::kSpot) {
        light.distance = (float)root["distance"].GetDouble();
        light.coeff.x = 1.0;
        auto& falloff = root["falloff"];
        light.coeff.y = (float)falloff["l"].GetDouble();
        light.coeff.z = (float)falloff["q"].GetDouble();
        
        if (light.type == LightType::kSpot) {
            light.cutoff = (float)root["cutoff"].GetDouble();
        }
    }
    
    return light;
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
