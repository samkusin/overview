//
//  NodeRenderer.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/5/15.
//
//

#ifndef CK_Graphics_RenderNodeGraph_hpp
#define CK_Graphics_RenderNodeGraph_hpp

#include "GfxTypes.hpp"
#include "Camera.hpp"
#include "NodeGraph.hpp"

#include <ckm/geometry.hpp>
#include <vector>
#include <array>

#include <bgfx/bgfx.h>

namespace cinek {
    namespace gfx {

enum NodeProgramSlot
{
    kNodeProgramMesh,
    kNodeProgramBoneMesh,
    kNodeProgramLimit       = 16,
    
    kNodeProgramNone        = -1
};

enum NodeUniformSlot
{
    //  Matrices
    kNodeUniformWorldMtx,
    kNodeUniformWorldViewProjMtx,
    
    //  Color Texture 
    kNodeUniformTexDiffuse,

    //      x = intensity, y = power
    //
    kNodeUniformMatSpecular,

    //  For the basic lighting model, light param is an amalgam of:
    //      x = ambient, y = diffuse, z = distance, w = cutoff
    //      if z == 0, then directional
    //      else
    //          if w == 0, then point (omnidirectional)
    //          else angle between light and extent of light on surface (spot)
    //
    kNodeUniformLightParam,
    //  For the basic lighting model, light coefficients for spot and point
    //
    kNodeUniformLightCoeffs,
    //  For the basic lighting model, light color is the base lamp color
    //
    kNodeUniformLightColor,
    //  For the basic lighting model, the light direction
    //
    kNodeUniformLightDir,
    //  For the basic lighting model, the origin used in spot or point lights
    //
    kNodeUniformLightOrigin,
    
    kNodeUniformLimit,
    
    kNodeUniformNone = -1
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
    
    
    enum
    {
        /// Enumerate Lighting Elements for Rendering
        kStageFlagLightEnum             = 1,
        /// Process Rendering Elements
        kStageFlagRender                = 2,
        /// All stages
        kStageAll                       = kStageFlagLightEnum
                                        | kStageFlagRender
    };
    
    void operator()(NodeHandle root, uint32_t stages=kStageAll);
    
private:
    struct ArmatureState;
    
    void pushTransform(const Matrix4& mtx);
    void popTransform();
    

    void renderMeshElement(const Matrix4& localTransform,
            const MeshElement& element);
    
    void buildBoneTransforms(const ArmatureState& armatureState,
                             int boneIndex,
                             const Matrix4& worldTransform,
                             const Matrix4& parentBoneTransform,
                             float* outTransforms);
    
    void setupLightUniforms(const Matrix4& objWorldMtx);
    
private:
    struct ArmatureState
    {
        const ArmatureElement* armature;
        Matrix4 armatureToWorldMtx;
        Matrix4 worldToArmatureMtx;
    };
    struct LightState
    {
        Matrix4 worldMtx;
        LightHandle light;
    };
    
    //  Global State
    ProgramMap _programs;
    UniformMap _uniforms;
    
    //  Local State
    Camera _camera;
    Matrix4 _viewMtx;
    Matrix4 _projMtx;
    Matrix4 _viewProjMtx;
        
    //  Calculated State during Lighting Object Pass
    using Lights = std::vector<LightState, std_allocator<LightState>>;
    Lights _globalLights;
    Lights _directionalLights;
    
    //  various stacks used to store current rendering state during execution
    std::vector<NodeHandle, std_allocator<NodeHandle>> _nodeStack;
    std::vector<Matrix4, std_allocator<Matrix4>> _transformStack;
    std::vector<ArmatureState, std_allocator<ArmatureState>> _armatureStack;
    
    std::vector<Vector4, std_allocator<Vector4>> _lightColors;
    std::vector<Vector4, std_allocator<Vector4>> _lightParams;
    std::vector<Vector4, std_allocator<Vector4>> _lightDirs;
    std::vector<Vector4, std_allocator<Vector4>> _lightOrigins;
    std::vector<Vector4, std_allocator<Vector4>> _lightCoeffs;
};



    }   // namespace gfx
}   // namespace cinek

#endif /* RenderNodeGraph_hpp */
