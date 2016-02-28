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

#include "NodeRendererTypes.hpp"

#include <ckm/geometry.hpp>
#include <vector>



namespace cinek {
    namespace gfx {

class NodeRenderer
{
    CK_CLASS_NON_COPYABLE(NodeRenderer);
    
public:
    using ProgramMap = RenderProgramMap;
    using UniformMap = RenderUniformMap;
    
    NodeRenderer();
    
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
    
    void setPlaceholderDiffuseTexture(TextureHandle diffuseTexHandle);
    
    void operator()(const ProgramMap& programs, const UniformMap& uniforms,
                    const Camera& camera,
                    NodeHandle root, uint32_t stages=kStageAll);
    
    void operator()(const ProgramMap& programs, const UniformMap& uniforms,
                    const RenderTarget& renderTarget,
                    const Camera& camera,
                    NodeHandle root, uint32_t stages=kStageAll);
    
private:
    struct ArmatureState;
    
    void pushTransform(const Matrix4& mtx);
    void popTransform();
    

    void renderMeshElement
    (
        const ProgramMap& programs,
        const UniformMap& uniforms,
        const Matrix4& localTransform,
        const MeshElement& element
    );
    
    void buildBoneTransforms(const ArmatureState& armatureState,
                             int boneIndex,
                             const Matrix4& parentBoneTransform,
                             float* outTransforms);
    
    void setupLightUniforms(const UniformMap& uniforms, const Matrix4& objWorldMtx);
    
private:
    struct ArmatureState
    {
        const ArmatureElement* armature;
        Matrix4 armatureToWorldMtx;
    };
    struct LightState
    {
        Matrix4 worldMtx;
        LightHandle light;
    };
    
    //  Local State
    const Camera* _camera;
    Matrix4 _viewMtx;
    Matrix4 _projMtx;
    Matrix4 _viewProjMtx;
    
    TextureHandle _placeholderDiffuseTex;
        
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
