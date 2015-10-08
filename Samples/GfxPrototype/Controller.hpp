//
//  Controller.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/7/15.
//
//

#ifndef Controller_hpp
#define Controller_hpp

#include "Common.hpp"

namespace cinek {

class AppController
{
public:
    AppController();
    
    void handleCameraInput(gfx::Camera& camera, const PollStateSDL& state,
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

}

#endif /* Controller_hpp */
