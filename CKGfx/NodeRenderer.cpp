//
//  NodeRenderer.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/5/15.
//
//

#include "NodeRenderer.hpp"
#include "ShaderLibrary.hpp"
#include "VertexTypes.hpp"

#include "Material.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Animation.hpp"

#include "Shaders/ckgfx.sh"

#include <ckm/math.hpp>
#include <ckm/geometry.hpp>

#include <bgfx/bgfx.h>
#include <bgfx/bgfx_shader.sh>
#include <bx/fpumath.h>

namespace cinek {
    namespace gfx {

/*
 *  Rendering a Node Graph
 *  
 *  Given a root Node reference to the contents to be rendered, and a Camera
 *  used to select nodes for rendering, the renderNodeGraph function generates
 *  commands executed by the low-level renderer.
 *
 *  Summary
 *  --------------------------------------------------------------------------
 *  The function traverses the Node tree pointed to by 'root'.  Rendering State
 *  such as current Transform, Material, etc. is pushed to and popped from 
 *  various stacks during Node traversal.
 *
 *  A Node contributes to current state.  Leaf nodes usually point to mesh or
 *  other primitive data that's sent to the low-level renderer.  For example,
 *  one node may set the current transform, pushing the prior transform to the
 *  stack.  That Transform Node has two Mesh child Nodes.  The Render method
 *  draws each Mesh using the current Transform.
 *
 *  The above demonstrates a *very simple* use case.   As the renderer grows,
 *  we'll add more use-cases.
 *
 */
 
NodeRenderer::NodeRenderer()
{
}

NodeRenderer::NodeRenderer
(
    const ProgramMap& programs,
    const UniformMap& uniforms
)
{
    std::copy(programs.begin(), programs.end(), _programs.begin());
    std::copy(uniforms.begin(), uniforms.end(), _uniforms.begin());

    _nodeStack.reserve(32);
    _transformStack.reserve(32);
    _armatureStack.reserve(4);
    
    
    Vector4 zero = ckm::zero<Vector4>();
    _lightColors.resize(CKGFX_SHADERS_LIGHT_COUNT, zero);
    _lightParams.resize(CKGFX_SHADERS_LIGHT_COUNT, zero);
    _lightDirs.resize(CKGFX_SHADERS_LIGHT_COUNT, zero);
    _lightCoeffs.resize(CKGFX_SHADERS_LIGHT_COUNT, zero);
    _lightOrigins.resize(CKGFX_SHADERS_LIGHT_COUNT, zero);
}

void NodeRenderer::setCamera(const Camera& camera)
{
    _camera = camera;
    
    bx::mtxInverse(_viewMtx.comp, _camera.worldMtx.comp);
    
    bx::mtxProj(_projMtx.comp,
        180.0f * _camera.viewFrustrum.fovRadians()/ckm::pi<float>(),
        _camera.viewFrustrum.aspect(),
        _camera.viewFrustrum.nearZ(),
        _camera.viewFrustrum.farZ());
}

void NodeRenderer::operator()(NodeHandle root, uint32_t systemTimeMs)
{
    CK_ASSERT(_nodeStack.empty());
    
    NodeHandle node = root;
    
    bgfx::setViewTransform(0, _viewMtx.comp, _projMtx.comp);
    bx::mtxMul(_viewProjMtx, _viewMtx, _projMtx);
    
    //  reset uniforms generated during the stack traversal
    _lightColors.clear();
    _lightDirs.clear();
    _lightParams.clear();
    _lightCoeffs.clear();
    _lightOrigins.clear();
    
    //  todo - light uniforms will be generated during the render pass
    //  for now, hardcode them here and modify as we move to a data-driven
    //  approach.
    
    //  directional lights
    Vector3 light0Dir = { -0.20f, -0.75f, 0.55f };
    bx::vec3Norm(light0Dir, light0Dir);
    
    
    //  directional
    _lightParams.emplace_back(0.10f, 0.90f, 0.0f, 0.0f);
    _lightDirs.emplace_back(light0Dir.x, light0Dir.y, light0Dir.z, 1.0f);
    _lightColors.emplace_back(0.75f, 0.75f, 0.75f, 1.0f);
    _lightOrigins.emplace_back();
    _lightCoeffs.emplace_back();
    
    //  point
    _lightParams.emplace_back(0.0f, 1.0f, 100.0f, 0.0f);
    _lightDirs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
    _lightColors.emplace_back(1.0f, 0.5f, 1.0f, 1.0f);
    _lightOrigins.emplace_back(15.0f, 15.0f, -2.0f, 0.0f);
    _lightCoeffs.emplace_back(1.0f, 0.05, 0.001, 0.0f);
    
    //  spot with cutoff
    light0Dir = { 1.0, 0.0, -1.0 };
    bx::vec3Norm(light0Dir, light0Dir);
    _lightParams.emplace_back(0.10f, 1.0f, 100.0f, 0.2f);
    _lightDirs.emplace_back(light0Dir.x, light0Dir.y, light0Dir.z, 1.0f);
    _lightColors.emplace_back(0.0f, 1.0f, 1.0f, 1.0f);
    _lightOrigins.emplace_back(-12.0f, 1.0f, 10.0f, 0.0f);
    _lightCoeffs.emplace_back(1.0f, 0.025, 0.025, 0.0f);
    
    _bgfxTransformCacheIndex =
        bgfx::allocTransform(&_bgfxTransforms, BGFX_CONFIG_MAX_BONES);
    
    Matrix4 topTransform;
    bx::mtxIdentity(topTransform);
    
    pushTransform(topTransform);
    
    while (!_nodeStack.empty() || node) {
        if (node) {
            //  parse current node
            switch (node->elementType()) {
            case Node::kElementTypeArmature: {
                    const ArmatureElement* armature = node->armature();
                    ArmatureState state { armature };
                    bx::mtxMul(state.armatureToWorldMtx, node->transform(),
                               _transformStack.back());
                    bx::mtxInverse(state.worldToArmatureMtx, state.armatureToWorldMtx);
                
                    state.animation = node->armature()->animSet->find("idle");
                    if (state.animation) {
                        state.animTime = fmodf(systemTimeMs*.001f, state.animation->duration);
                    }
                    else {
                        state.animTime = 0.f;
                    }
                    _armatureStack.emplace_back(state);
                }
                break;
            
            case Node::kElementTypeMesh: {
                    const MeshElement* mesh = node->mesh();
                    while (mesh) {
                        renderMeshElement(node->transform(), *mesh);
                        mesh = mesh->next;
                    }
                }
                break;
            
            case Node::kElementTypeCustom:
                break;
                
            default:
                break;
            }
            
            pushTransform(node->transform());
            _nodeStack.emplace_back(node);
            
            node = node->firstChildHandle();
        }
        else {
            node = _nodeStack.back();
            _nodeStack.pop_back();
            
            popTransform();
            
            //  execute cleanup of the parent node
            switch (node->elementType()) {
            case Node::kElementTypeArmature:
                _armatureStack.pop_back();
                break;
            default:
                break;
            }
            
            node = node->nextSiblingHandle();
        }
    }
}

void NodeRenderer::pushTransform(const Matrix4& mtx)
{
    //  calculate new transform and set as current state.
    if (_transformStack.empty()) {
        _transformStack.emplace_back(mtx);
    }
    else {
        //  make space for our new transformation matrix and use our
        //  prior matrix as a basis modified by 'mtx'.
        _transformStack.emplace_back();
        
        bx::mtxMul(_transformStack.back().comp,
            mtx,
            _transformStack[_transformStack.size()-2].comp);
    }
}

void NodeRenderer::popTransform()
{
    CK_ASSERT(!_transformStack.empty());
    _transformStack.pop_back();
}

void NodeRenderer::renderMeshElement
(
    const Matrix4& localTransform,
    const MeshElement& element
)
{
    //  setup rendering state
    if (element.material->diffuseTex) {
        bgfx::TextureHandle texDiffuse = element.material->diffuseTex->bgfxHandle();
        bgfx::setTexture(0, _uniforms[kNodeUniformTexDiffuse], texDiffuse,
            BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_ANISOTROPIC);
    }
    Vector4 specular;
    specular.x = (element.material->specularColor.r +
                  element.material->specularColor.g +
                  element.material->specularColor.b) * 0.33f;
    specular.y = element.material->specularPower;
    specular.z = 0;
    specular.w = 0;
    bgfx::setUniform(_uniforms[kNodeUniformMatSpecular], specular);
    
    //  setup lighting
    if (!_lightColors.empty()) {
        bgfx::setUniform(_uniforms[kNodeUniformLightColor], _lightColors.data(), _lightColors.size());
    }
    if (!_lightParams.empty()) {
        bgfx::setUniform(_uniforms[kNodeUniformLightParam], _lightParams.data(), _lightParams.size());
    }
    if (!_lightDirs.empty()) {
        bgfx::setUniform(_uniforms[kNodeUniformLightDir], _lightDirs.data(), _lightDirs.size());
    }
    
    if (!_lightOrigins.empty()) {
        bgfx::setUniform(_uniforms[kNodeUniformLightOrigin], _lightOrigins.data(), _lightOrigins.size());
    }
    if (!_lightCoeffs.empty()) {
        bgfx::setUniform(_uniforms[kNodeUniformLightCoeffs], _lightCoeffs.data(), _lightCoeffs.size());
    }

    //  setup mesh rendering
    const Mesh* mesh = element.mesh.resource();
    bgfx::setVertexBuffer(mesh->vertexBuffer());
    bgfx::setIndexBuffer(mesh->indexBuffer());
    
    Matrix4 worldTransform;
    bx::mtxMul(worldTransform, localTransform, _transformStack.back());

    NodeProgramSlot programSlot;
    if (!_armatureStack.empty()) {
        programSlot = kNodeProgramBoneMesh;

        const ArmatureState& armatureState = _armatureStack.back();

        Matrix4 worldViewProjMtx;
        bx::mtxMul(worldViewProjMtx, armatureState.armatureToWorldMtx,
                                     _viewProjMtx);
        
        bgfx::setUniform(_uniforms[kNodeUniformWorldMtx],
                         armatureState.armatureToWorldMtx.comp, 1);
        bgfx::setUniform(_uniforms[kNodeUniformWorldViewProjMtx],
                         worldViewProjMtx.comp, 1);
   
        buildBoneTransforms(armatureState, 0, worldTransform);
        bgfx::setTransform(_bgfxTransformCacheIndex,
                           armatureState.armature->animSet->bones.size());
    }
    else
    {
        programSlot = kNodeProgramMesh;
        bgfx::setTransform(worldTransform);
    }

    
    bgfx::setState(0
						| BGFX_STATE_RGB_WRITE
						| BGFX_STATE_ALPHA_WRITE
						| BGFX_STATE_DEPTH_WRITE
						| BGFX_STATE_DEPTH_TEST_LESS
						| BGFX_STATE_MSAA
                        | BGFX_STATE_CULL_CCW
						);

    bgfx::submit(0, _programs[programSlot]);
}

void NodeRenderer::buildBoneTransforms
(
    const ArmatureState& armatureState,
    int boneIndex,
    const Matrix4& worldTransform
)
{
    const AnimationSet* animSet = armatureState.armature->animSet.resource();
    auto& bones = animSet->bones;
    auto& bone = bones[boneIndex];
    const Animation* animation = armatureState.animation;
    
    //  generate our "local (most likely a mesh)" space to bone transform
    //  local -> world -> armature -> bone_local -> armature = final bone transform
    
    //  animations exist on this bone
    if (animation && animation->channels[boneIndex].animatedSeqMask) {
        Matrix4 interMtx;
        bx::mtxMul(interMtx, worldTransform, armatureState.worldToArmatureMtx);
    
        Matrix4 meshToBoneMtx;
        bx::mtxMul(meshToBoneMtx, interMtx, bone.invMtx);
    
        const SequenceChannel& seqForBone = animation->channels[boneIndex];
    
        Matrix4 multMtx;
        bx::mtxIdentity(multMtx);      // TODO!
    
        Vector4 boneRotQuat;
        bx::quatIdentity(boneRotQuat);
        interpRotationFromSequenceChannel(boneRotQuat, seqForBone, armatureState.animTime);
    
        Matrix4 rotMtx;
        bx::mtxQuat(rotMtx, boneRotQuat);
        
        Vector3 translate;
        translate.x = 0;
        translate.y = 0;
        translate.z = 0;
        interpTranslateFromSequenceChannel(translate, seqForBone, armatureState.animTime);

        // Mint = Mrot * Mscale
        // Mint = Mint + translate
        bx::mtxMul(interMtx, multMtx, rotMtx);
        
        interMtx[12] = translate.x;
        interMtx[13] = translate.y;
        interMtx[14] = translate.z;
    
        // Transform our bone based on animation
        bx::mtxMul(multMtx, meshToBoneMtx, interMtx);
        // Transform back to armature space
        bx::mtxMul(_bgfxTransforms.data + boneIndex*16, multMtx, bone.mtx);
    }
    else {
        //  no animation - just use to mesh to armature matrix as our bone
        //  transform
        bx::mtxMul(_bgfxTransforms.data + boneIndex*16,
                   worldTransform, armatureState.worldToArmatureMtx);
    }
    
    for (int childBoneIndex = bone.firstChild;
         childBoneIndex >= 0;
         childBoneIndex = bones[childBoneIndex].nextSibling) {
        
        buildBoneTransforms(armatureState, childBoneIndex, worldTransform);
    }
}


    }   // namespace gfx
}   // namespace cinek

