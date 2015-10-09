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

#include "Shaders/ckgfx.sh"

#include <ckm/math.hpp>
#include <ckm/geometry.hpp>

#include <bgfx/bgfx.h>
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

void NodeRenderer::operator()(NodeHandle root)
{
    CK_ASSERT(_nodeStack.empty());
    
    NodeHandle node = root;
    
    bgfx::setViewTransform(0, _viewMtx.comp, _projMtx.comp);
    
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
    Vector3 light0Dir = { -0.75f, -0.75f, -0.25f };
    bx::vec3Norm(light0Dir, light0Dir);
    
    
    //  directional
    _lightParams.emplace_back(0.10f, 0.90f, 0.0f, 0.0f);
    _lightDirs.emplace_back(light0Dir.x, light0Dir.y, light0Dir.z, 1.0f);
    _lightColors.emplace_back(0.25f, 0.25f, 0.25f, 1.0f);
    _lightOrigins.emplace_back();
    _lightCoeffs.emplace_back();
    
    //  point
    _lightParams.emplace_back(0.0f, 1.0f, 100.0f, 0.0f);
    _lightDirs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
    _lightColors.emplace_back(1.0f, 0.0f, 1.0f, 1.0f);
    _lightOrigins.emplace_back(20.0f, 20.0f, -5.0f, 0.0f);
    _lightCoeffs.emplace_back(1.0f, 0.05, 0.001, 0.0f);
    
    //  point
    _lightParams.emplace_back(0.10f, 1.0f, 100.0f, 0.0f);
    _lightDirs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
    _lightColors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
    _lightOrigins.emplace_back(-15.0f, 20.0f, 10.0f, 0.0f);
    _lightCoeffs.emplace_back(1.0f, 0.001, 0.001, 0.0f);
    
    while (!_nodeStack.empty() || node) {
        if (node) {
            //  parse current node
            switch (node->elementType()) {
            case Node::kElementTypeTransform:
                pushTransform(node->transform()->mtx);
                break;
                
            case Node::kElementTypeMesh: {
                    const MeshElement* mesh = node->mesh();
                    while (mesh) {
                        renderMeshElement(*mesh);
                        mesh = mesh->next;
                    }
                }
                break;
            
            case Node::kElementTypeCustom:
                break;
                
            default:
                break;
            }
            
            _nodeStack.emplace_back(node);
            
            node = node->firstChildHandle();
        }
        else {
            node = _nodeStack.back();
            _nodeStack.pop_back();
            
            //  execute cleanup of the parent node
            switch (node->elementType()) {
                case Node::kElementTypeTransform:
                popTransform();
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

void NodeRenderer::renderMeshElement(const MeshElement& element)
{
    //  setup rendering state
    if (element.material->diffuseTex) {
        bgfx::TextureHandle texDiffuse = element.material->diffuseTex->bgfxHandle();
        bgfx::setTexture(0, _uniforms[kNodeUniformTexDiffuse], texDiffuse,
            BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_ANISOTROPIC);
    }
    Vector4 specular;
    specular.x = element.material->specularIntensity;
    specular.y = element.material->specularPower;
    specular.z = 0;
    specular.w = 0;
    bgfx::setUniform(_uniforms[kNodeUniformMatSpecular], specular);
    bgfx::setUniform(_uniforms[kNodeUniformMatSpecularColor], element.material->specularColor);
    
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
    bgfx::setTransform(_transformStack.back().comp);
    
    const Mesh* mesh = element.mesh.resource();
    
    bgfx::setVertexBuffer(mesh->vertexBuffer());
    bgfx::setIndexBuffer(mesh->indexBuffer());

//    bgfx::setState(BGFX_STATE_DEFAULT);
    
    bgfx::setState(0
						| BGFX_STATE_RGB_WRITE
						| BGFX_STATE_ALPHA_WRITE
						| BGFX_STATE_DEPTH_WRITE
						| BGFX_STATE_DEPTH_TEST_LESS
						| BGFX_STATE_MSAA
                        | BGFX_STATE_CULL_CCW
						);
    bgfx::submit(0, _programs[kNodeProgramMesh]);
}


    }   // namespace gfx
}   // namespace cinek

