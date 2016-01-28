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
#include "AnimationController.hpp"
#include "Light.hpp"
#include "RenderTarget.hpp"

#include "Shaders/ckgfx.sh"

#include <ckm/math.hpp>
#include <ckm/geometry.hpp>

#include <bgfx/bgfx.h>
#include <bgfx/bgfx_shader.sh>
#include <bx/fpumath.h>

namespace cinek {
    namespace gfx {

	inline void mtxQuatRCS(float* __restrict _result, const float* __restrict _quat)
	{
		const float x = _quat[0];
		const float y = _quat[1];
		const float z = _quat[2];
		const float w = _quat[3];

		const float x2  =  x + x;
		const float y2  =  y + y;
		const float z2  =  z + z;
		const float x2x = x2 * x;
		const float x2y = x2 * y;
		const float x2z = x2 * z;
		const float x2w = x2 * w;
		const float y2y = y2 * y;
		const float y2z = y2 * z;
		const float y2w = y2 * w;
		const float z2z = z2 * z;
		const float z2w = z2 * w;

		_result[ 0] = 1.0f - (y2y + z2z);
		_result[ 1] =         x2y + z2w;
		_result[ 2] =         x2z - y2w;
		_result[ 3] = 0.0f;

		_result[ 4] =         x2y - z2w;
		_result[ 5] = 1.0f - (x2x + z2z);
		_result[ 6] =         y2z + x2w;
		_result[ 7] = 0.0f;

		_result[ 8] =         x2z + y2w;
		_result[ 9] =         y2z - x2w;
		_result[10] = 1.0f - (x2x + y2y);
		_result[11] = 0.0f;

		_result[12] = 0.0f;
		_result[13] = 0.0f;
		_result[14] = 0.0f;
		_result[15] = 1.0f;
	}

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
    _nodeStack.reserve(32);
    _transformStack.reserve(32);
    _armatureStack.reserve(4);
    
    Vector4 zero = ckm::zero<Vector4>();
    _lightColors.resize(CKGFX_SHADERS_LIGHT_COUNT, zero);
    _lightParams.resize(CKGFX_SHADERS_LIGHT_COUNT, zero);
    _lightDirs.resize(CKGFX_SHADERS_LIGHT_COUNT, zero);
    _lightCoeffs.resize(CKGFX_SHADERS_LIGHT_COUNT, zero);
    _lightOrigins.resize(CKGFX_SHADERS_LIGHT_COUNT, zero);
    
    _globalLights.reserve(8);
    _directionalLights.reserve(64);
}

void NodeRenderer::setPlaceholderDiffuseTexture(TextureHandle diffuseTexHandle)
{
    _placeholderDiffuseTex = diffuseTexHandle;
}

void NodeRenderer::operator()
(
    const ProgramMap& programs,
    const UniformMap& uniforms,
    const Camera& camera, 
    NodeHandle root,
    uint32_t stages /*=kStageAll */
)
{
    (*this)(programs, uniforms, RenderTarget(), camera, root, stages);
}

void NodeRenderer::operator()
(
    const ProgramMap& programs,
    const UniformMap& uniforms,
    const RenderTarget& renderTarget,
    const Camera& camera, 
    NodeHandle root,
    uint32_t stages /*=kStageAll */
)
{
    uint32_t currentStage = 1;
    
    _camera = &camera;
    
    while (stages) {
        if ((stages & 0x01)!=0) {
        
            NodeHandle node = root;

            Matrix4 topTransform;
            bx::mtxIdentity(topTransform);
            pushTransform(topTransform);
        
            switch (currentStage) {
            case kStageFlagRender: {
                    bgfx::setViewRect(_camera->viewIndex,
                        _camera->viewportRect.x, _camera->viewportRect.y,
                        _camera->viewportRect.w ,_camera->viewportRect.h);
                
                    if (renderTarget) {
                        bgfx::setViewFrameBuffer(_camera->viewIndex, renderTarget.bgfxHandle());
                    }

                    bgfx::setViewTransform(_camera->viewIndex,
                        _camera->viewMtx.comp,
                        _camera->projMtx.comp);
                    bx::mtxMul(_viewProjMtx, _camera->viewMtx, _camera->projMtx);
                }
                break;
            case kStageFlagLightEnum: {
                    _globalLights.clear();
                    _directionalLights.clear();
                }
                break;
            default:
                break;
            }
        
            while (!_nodeStack.empty() || node) {
                if (node) {
                    //  parse current node
                    if (currentStage == kStageFlagLightEnum) {
                        //
                        //  Lighting Pass
                        //
                        if (node->elementType() == Node::kElementTypeLight) {
                            const LightElement* e = node->light();
                            
                            Matrix4 lightMtx;
                            bx::mtxMul(lightMtx, node->transform(), _transformStack.back());
                            
                            if (e->light->type == LightType::kAmbient
                                || e->light->type == LightType::kDirectional) {
                                _globalLights.emplace_back(LightState{ lightMtx, e->light } );
                            }
                            else {
                                _directionalLights.emplace_back(LightState{ lightMtx, e->light });
                            }
                        }
                    }
                    else if (currentStage == kStageFlagRender) {
                        //
                        //  Render Pass
                        //
                        switch (node->elementType()) {
                        case Node::kElementTypeArmature: {
                                const ArmatureElement* armature = node->armature();
                                ArmatureState state { armature };
                                bx::mtxMul(state.armatureToWorldMtx, node->transform(),
                                           _transformStack.back());
                                bx::mtxInverse(state.worldToArmatureMtx, state.armatureToWorldMtx);
                                _armatureStack.emplace_back(state);
                            }
                            break;
                        
                        case Node::kElementTypeMesh: {
                                const MeshElement* mesh = node->mesh();
                                while (mesh) {
                                    renderMeshElement(programs, uniforms, node->transform(), *mesh);
                                    mesh = mesh->next;
                                }
                            }
                            break;
                        
                        case Node::kElementTypeCustom:
                            break;
                            
                        default:
                            break;
                        }
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
                    if (currentStage == kStageFlagRender) {
                        switch (node->elementType()) {
                        case Node::kElementTypeArmature:
                            _armatureStack.pop_back();
                            break;
                        default:
                            break;
                        }
                    }
                    node = node->nextSiblingHandle();
                }
            }
        
        }
        
        popTransform();     // cleanup default top-level transform
        
        stages >>= 1;
        currentStage <<= 1;
    }
    
    CK_ASSERT(_transformStack.empty());
    CK_ASSERT(_nodeStack.empty());
    CK_ASSERT(_armatureStack.empty());
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
    const ProgramMap& programs,
    const UniformMap& uniforms,
    const Matrix4& localTransform,
    const MeshElement& element
)
{
    //  determine program
    NodeProgramSlot programSlot = kNodeProgramNone;
    const Mesh* mesh = element.mesh.resource();
    const bgfx::VertexDecl& meshVertexDecl = VertexTypes::declaration(mesh->format());
    
    if (!_armatureStack.empty()) {
        if (mesh->format() == VertexTypes::kVNormal_Tex0_Weights) {
            programSlot = kNodeProgramBoneMeshUV;
        }
        else if (mesh->format() == VertexTypes::kVNormal_Weights) {
            programSlot = kNodeProgramBoneMeshColor;
        }
    }
    else {
        if (mesh->format() == VertexTypes::kVPosition) {
            programSlot = kNodeProgramFlat;
        }
        else if (mesh->format() == VertexTypes::kVPositionNormal) {
            programSlot = kNodeProgramMeshColor;
        }
        else if (mesh->format() == VertexTypes::kVNormal_Tex0) {
            programSlot = kNodeProgramMeshUV;
        }
    }
    
    CK_ASSERT_RETURN(programSlot != kNodeProgramNone);
    
    //  setup rendering state
    bgfx::setUniform(uniforms[kNodeUniformColor], element.material->diffuseColor, 1);
    
    //  diffuse texture selection
    if (element.material->diffuseTex) {
        bgfx::TextureHandle texDiffuse = element.material->diffuseTex->bgfxHandle();
        bgfx::setTexture(0, uniforms[kNodeUniformTexDiffuse], texDiffuse,
            BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_ANISOTROPIC);
    }
    else {
        //  if our mesh has uvs but no material texture?  use a placeholder
        //  texture
        if (meshVertexDecl.has(bgfx::Attrib::TexCoord0)) {
            bgfx::setTexture(0, uniforms[kNodeUniformTexDiffuse],
                _placeholderDiffuseTex->bgfxHandle(),
                BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_ANISOTROPIC);
        }
    }
    //  TODO - include specular color?
    Vector4 specular;
    specular.x = element.material->specularIntensity;
    specular.y = element.material->specularPower;
    specular.z = 0;
    specular.w = 0;
    bgfx::setUniform(uniforms[kNodeUniformMatSpecular], specular);
    
    Matrix4 worldTransform;
    bx::mtxMul(worldTransform, localTransform, _transformStack.back());
    
    
    
    //  setup lighting
    setupLightUniforms(uniforms, worldTransform);
    
    //  setup mesh rendering
    bgfx::setVertexBuffer(mesh->vertexBuffer());
    bgfx::setIndexBuffer(mesh->indexBuffer());
    
    if (!_armatureStack.empty()) {
        const ArmatureState& armatureState = _armatureStack.back();

        Matrix4 worldViewProjMtx;
        bx::mtxMul(worldViewProjMtx, armatureState.armatureToWorldMtx,
                                     _viewProjMtx);
        
        bgfx::setUniform(uniforms[kNodeUniformWorldMtx],
                         armatureState.armatureToWorldMtx.comp, 1);
        bgfx::setUniform(uniforms[kNodeUniformWorldViewProjMtx],
                         worldViewProjMtx.comp, 1);
   
        bgfx::Transform boneTransforms;
   
        int transformCacheIndex =
                        bgfx::allocTransform(&boneTransforms, BGFX_CONFIG_MAX_BONES);
        
        //printf("Kf=%d\n", (int)(armatureState.armature->animController->animationTime() * 24));
   
        Matrix4 rootBoneTransform;
        bx::mtxIdentity(rootBoneTransform);
   
        buildBoneTransforms(armatureState, 0, worldTransform, rootBoneTransform, boneTransforms.data);
        bgfx::setTransform(transformCacheIndex,
                           armatureState.armature->animSet->boneCount());
    }
    else
    {
        bgfx::setTransform(worldTransform);
    }

    auto state = BGFX_STATE_RGB_WRITE
        | BGFX_STATE_ALPHA_WRITE
        | BGFX_STATE_DEPTH_WRITE
        | BGFX_STATE_DEPTH_TEST_LESS
        | BGFX_STATE_MSAA;
    
    
    if (mesh->primitiveType() == PrimitiveType::kTriangles) {
        state |= BGFX_STATE_CULL_CW;
    }
    else if (mesh->primitiveType() == PrimitiveType::kLines) {
        state |= BGFX_STATE_PT_LINES;
    }
    else {
        CK_ASSERT(false);
    }
    
    bgfx::setState(state);

    bgfx::submit(_camera->viewIndex, programs[programSlot]);
}

void NodeRenderer::setupLightUniforms
(
    const UniformMap& uniforms,
    const Matrix4& objWorldMtx
)
{
    //  reset uniforms generated during the stack traversal
    _lightColors.clear();
    _lightDirs.clear();
    _lightParams.clear();
    _lightCoeffs.clear();
    _lightOrigins.clear();
    
    for (auto& light : _globalLights) {
        const Light* l = light.light.resource();
        
        _lightCoeffs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
        _lightOrigins.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
        
        Vector4 color;
        _lightColors.emplace_back(color.fromABGR(l->color));
        _lightParams.emplace_back(l->ambientComp, l->diffuseComp, 0.0f, 0.0f);
    
        if (l->type == LightType::kDirectional) {
            Vector4 dir;
            bx::vec4MulMtx(dir, Vector4::kUnitZ, light.worldMtx);
            //bx::vec3Norm(dir, dir);
            bx::vec3Neg(dir, dir);
            _lightDirs.emplace_back(dir);
        }
        else {
            _lightDirs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
        }
    }
    
    for (auto& light : _directionalLights) {
        const Light* l = light.light.resource();
        float dist = 0.0f;
        float span = 0.0f;
        if (l->type == LightType::kPoint || l->type == LightType::kSpot) {
            dist = l->distance;
            
            _lightOrigins.emplace_back(light.worldMtx[12],
                light.worldMtx[13],
                light.worldMtx[14],
                0.0f);
            
            if (l->type == LightType::kSpot) {
                span = l->cutoff;
            }
            
            _lightCoeffs.emplace_back(l->coeff.x, l->coeff.y, l->coeff.z, 0.0f);
        }
        
        Vector4 color;
        _lightColors.emplace_back(color.fromABGR(l->color));
        _lightParams.emplace_back(l->ambientComp, l->diffuseComp, dist, span);
    
        if (l->type == LightType::kSpot) {
            Vector4 dir;
            bx::vec4MulMtx(dir, Vector4::kUnitZ, light.worldMtx);
            bx::vec3Neg(dir, dir);
            _lightDirs.emplace_back(dir);
        }
        else {
            _lightDirs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
        }
    }
    
    if (!_lightColors.empty()) {
        bgfx::setUniform(uniforms[kNodeUniformLightColor], _lightColors.data(), (uint16_t)_lightColors.size());
    }
    if (!_lightParams.empty()) {
        bgfx::setUniform(uniforms[kNodeUniformLightParam], _lightParams.data(), (uint16_t)_lightParams.size());
    }
    if (!_lightDirs.empty()) {
        bgfx::setUniform(uniforms[kNodeUniformLightDir], _lightDirs.data(), (uint16_t)_lightDirs.size());
    }
    
    if (!_lightOrigins.empty()) {
        bgfx::setUniform(uniforms[kNodeUniformLightOrigin], _lightOrigins.data(), (uint16_t)_lightOrigins.size());
    }
    if (!_lightCoeffs.empty()) {
        bgfx::setUniform(uniforms[kNodeUniformLightCoeffs], _lightCoeffs.data(), (uint16_t)_lightCoeffs.size());
    }
}

void NodeRenderer::buildBoneTransforms
(
    const ArmatureState& armatureState,
    int boneIndex,
    const Matrix4& worldTransform,
    const Matrix4& parentBoneTransform,
    float* outTransforms
)
{
    const AnimationSet* animSet = armatureState.armature->animSet.resource();
    auto bone = animSet->boneFromIndex(boneIndex);

    const AnimationController* animController = armatureState.armature->animController.resource();
    const Animation* animation = animController ?
        armatureState.armature->animController->animation() :
        nullptr;

    
    //  generate our "local (most likely a mesh)" space to bone transform
    //  local -> world -> armature -> bone_local -> armature = final bone transform
    Matrix4 boneTransform;
    //  animations exist on this bone
    if (animation && animation->channels[boneIndex].animatedSeqMask) {        
        Matrix4 interMtx;
        Matrix4 meshToBoneMtx;
        bx::mtxMul(interMtx, worldTransform, armatureState.worldToArmatureMtx);
        bx::mtxMul(meshToBoneMtx, interMtx, bone->invMtx);
        
        const SequenceChannel& seqForBone = animation->channels[boneIndex];
    
        Vector3 scale;
        scale.x = 1.0f;
        scale.y = 1.0f;
        scale.z = 1.0f;
        interpScaleFromSequenceChannel(scale, seqForBone, animController->animationTime());
        
        Matrix4 multMtx;
        bx::mtxScale(multMtx, scale.x, scale.y, scale.z);
        
        Vector4 boneRotQuat;
        bx::quatIdentity(boneRotQuat);
        interpRotationFromSequenceChannel(boneRotQuat, seqForBone, animController->animationTime());
       
        Matrix4 rotMtx;
        bx::mtxQuat(rotMtx, boneRotQuat);
        //mtxQuatRCS(rotMtx, boneRotQuat);
        
        Vector3 translate;
        translate.x = 0;
        translate.y = 0;
        translate.z = 0;
        interpTranslateFromSequenceChannel(translate, seqForBone, animController->animationTime());

        // Mint = Mrot * Mscale
        // Mint = Mint + translate
        bx::mtxMul(interMtx, multMtx, rotMtx);
        
        interMtx[12] = translate.x;
        interMtx[13] = translate.y;
        interMtx[14] = translate.z;
        
        bx::mtxMul(boneTransform, interMtx, parentBoneTransform);
    
        // Transform our bone based on animation
        bx::mtxMul(multMtx, meshToBoneMtx, boneTransform);
        // Transform back to armature space
        bx::mtxMul(outTransforms + boneIndex*16, multMtx, bone->mtx);
    }
    else {
        //  no animation - just use to mesh to armature matrix as our bone
        //  transform
        bx::mtxMul(outTransforms + boneIndex*16,
                   worldTransform, armatureState.worldToArmatureMtx);
        boneTransform = parentBoneTransform;
    }
    
    for (int childBoneIndex = bone->firstChild;
         childBoneIndex >= 0;
         childBoneIndex = animSet->boneFromIndex(childBoneIndex)->nextSibling) {
        
        buildBoneTransforms(armatureState, childBoneIndex, worldTransform, boneTransform, outTransforms);
    }
}


    }   // namespace gfx
}   // namespace cinek

