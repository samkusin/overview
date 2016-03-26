//
//  Controller.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/7/15.
//
//

#ifndef Controller_hpp
#define Controller_hpp

#include "Input.hpp"

#include "Engine/EngineTypes.hpp"
#include "CKGfx/GfxTypes.hpp"


namespace cinek {
    namespace ove {

class FreeCameraController
{
public:
    FreeCameraController();
    
    void setTransform(const gfx::Vector3& pos, const gfx::Matrix4& rotMtx);
    
    void handleCameraInput(gfx::Camera& camera, const cinek::input::InputState& state,
                           float frameDelta);

private:
    enum
    {
        kInputFwd       = 0x00000001,
        kInputBack      = 0x00000002,
        kInputLeft      = 0x00000004,
        kInputRight     = 0x00000008,
        kInputUp        = 0x00000010,
        kInputDown      = 0x00000020
    };
    
    uint32_t _moveFlags;
    float _moveYaw;
    float _movePitch;
    
    struct LocalTransform
    {
        gfx::Vector3 pos;
        gfx::Matrix4 rotMtx;
    };
    
    LocalTransform _cameraTransform;
};
    } /* namespace ove */
} /* namespace cinek */

#endif /* Controller_hpp */
