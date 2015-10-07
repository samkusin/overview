//
//  RenderNodeGraph.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/5/15.
//
//

#ifndef CK_Graphics_RenderNodeGraph_hpp
#define CK_Graphics_RenderNodeGraph_hpp

#include "GfxTypes.hpp"
#include "NodeGraph.hpp"

#include <ckm/geometry.hpp>
#include <vector>
#include <array>

#include <bgfx/bgfx.h>

namespace cinek {
    namespace gfx {

struct Camera
{
    //  world transform matrix (with 'world' as defined for the rendered graph.)
    Matrix4 worldMtx;
    //  view frustrum relative to the current view matrix
    Frustrum viewFrustrum;
};


enum NodeProgramSlot
{
    kNodeProgramMesh,
    kNodeProgramLimit       = 16,
    
    kNodeProgramNone        = -1
};

enum NodeUniformSlot
{
    kNodeUniformTexDiffuse,
    kNodeUniformLimit       = 16,
    
    kNodeUniformNone        = -1
};

class NodeRenderer
{
    CK_CLASS_NON_COPYABLE(NodeRenderer);
    
public:
    using ProgramMap = std::array<bgfx::ProgramHandle, kNodeProgramLimit>;
    using UniformMap = std::array<bgfx::UniformHandle, kNodeUniformLimit>;
    
    NodeRenderer();
    NodeRenderer(const ProgramMap& programs,
                 const UniformMap& uniforms);

    void setCamera(const Camera& camera);
    
    void operator()(NodeHandle root);
    
private:
    void pushTransform(const Matrix4& mtx);
    void popTransform();
    
    void renderMeshElement(const MeshElement& element);
    
private:
    //  Global State
    ProgramMap _programs;
    UniformMap _uniforms;
    
    //  Local State
    Camera _camera;
    Matrix4 _viewMtx;
    Matrix4 _projMtx;
    
    NodeProgramSlot _currentProgram;
    
    //  various stacks used to store current rendering state during execution
    std::vector<NodeHandle> _nodeStack;
    std::vector<Matrix4> _transformStack;
};



    }   // namespace gfx
}   // namespace cinek

#endif /* RenderNodeGraph_hpp */
