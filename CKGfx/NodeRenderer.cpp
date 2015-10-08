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
 
NodeRenderer::NodeRenderer() :
    _currentProgram(kNodeProgramNone)
{
}

NodeRenderer::NodeRenderer
(
    const ProgramMap& programs,
    const UniformMap& uniforms
) :
    _currentProgram(kNodeProgramNone)
{
    std::copy(programs.begin(), programs.end(), _programs.begin());
    std::copy(uniforms.begin(), uniforms.end(), _uniforms.begin());

    _nodeStack.reserve(32);
    _transformStack.reserve(32);
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
    
    _currentProgram = kNodeProgramNone;
    
    NodeHandle node = root;
    
    bgfx::setViewTransform(0, _viewMtx.comp, _projMtx.comp);
    
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
        bgfx::setTexture(0, _uniforms[kNodeUniformTexDiffuse], texDiffuse);
    }

    if (_currentProgram != kNodeProgramMesh) {
        _currentProgram = kNodeProgramMesh;
        bgfx::setProgram(_programs[_currentProgram]);
    }

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
    bgfx::submit(0);
}


    }   // namespace gfx
}   // namespace cinek

