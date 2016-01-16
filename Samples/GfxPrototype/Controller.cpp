//
//  Controller.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/7/15.
//
//

#include "Controller.hpp"
#include "CKGfx/Camera.hpp"
#include "UICore/Input.hpp"

#include <bx/fpumath.h>

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>


namespace cinek {

AppController::AppController() :
    _moveFlags(0),
    _moveYaw(0),
    _movePitch(0)
{
    _cameraTransform.pos = { 0,1.0, -8.0f };
    bx::mtxIdentity(_cameraTransform.rotMtx);
}

void AppController::handleCameraInput
(
    gfx::Camera &camera,
    const uicore::InputState &state,
    float frameDelta
)
{
    const float kSpeed = 4.0f;
    const float kRadiansDelta = M_PI * 0.25f;
    
    if (state.keystate[SDL_SCANCODE_W]) {
        if (state.keystate[SDL_SCANCODE_LSHIFT]) {
            _moveFlags |= kInputUp;
            _moveFlags &= ~kInputFwd;
        }
        else {
            _moveFlags |= kInputFwd;
            _moveFlags &= ~kInputUp;
        }
    }
    else {
        _moveFlags &= ~kInputUp;
        _moveFlags &= ~kInputFwd;
    }
    if (state.keystate[SDL_SCANCODE_S]) {
        if (state.keystate[SDL_SCANCODE_LSHIFT]) {
            _moveFlags |= kInputDown;
            _moveFlags &= ~kInputBack;
        }
        else {
            _moveFlags |= kInputBack;
            _moveFlags &= ~kInputDown;
        }
    }
    else {
        _moveFlags &= ~kInputDown;
        _moveFlags &= ~kInputBack;
    }
    if (state.keystate[SDL_SCANCODE_A]) {
        _moveFlags |= kInputLeft;
    }
    else {
        _moveFlags &= ~kInputLeft;
    }    
    if (state.keystate[SDL_SCANCODE_D]) {
        _moveFlags |= kInputRight;
    }
    else {
        _moveFlags &= ~kInputRight;
    }
    
    if (state.keystate[SDL_SCANCODE_UP]) {
        _movePitch = -frameDelta;
    }
    if (state.keystate[SDL_SCANCODE_DOWN]) {
        _movePitch = frameDelta;
    }
    if (!state.keystate[SDL_SCANCODE_UP] && !state.keystate[SDL_SCANCODE_DOWN]) {
        _movePitch = 0;
    }
    
    if (state.keystate[SDL_SCANCODE_LEFT]) {
        _moveYaw = frameDelta;
    }
    if (state.keystate[SDL_SCANCODE_RIGHT]) {
        _moveYaw = -frameDelta;
    }
    if (!state.keystate[SDL_SCANCODE_LEFT] && !state.keystate[SDL_SCANCODE_RIGHT]) {
        _moveYaw = 0;
    }
    
    gfx::Vector3 viewTranslate = { 0,0,0 };

    if ((_moveFlags & kInputFwd) != 0) {
        viewTranslate.z += frameDelta * kSpeed;
    }
    if ((_moveFlags & kInputBack) != 0) {
        viewTranslate.z -= frameDelta * kSpeed;
    }
    if ((_moveFlags & kInputLeft) != 0) {
        viewTranslate.x -= frameDelta * kSpeed;
    }
    if ((_moveFlags & kInputRight) != 0) {
        viewTranslate.x += frameDelta * kSpeed;
    }
    if ((_moveFlags & kInputUp) != 0) {
        viewTranslate.y += frameDelta * kSpeed;
    }
    if ((_moveFlags & kInputDown) != 0) {
        viewTranslate.y -= frameDelta * kSpeed;
    }
    
    gfx::Matrix4 viewRotate;
    bx::mtxRotateXY(viewRotate, _movePitch * kRadiansDelta, _moveYaw * kRadiansDelta);
    
    gfx::Matrix4 worldRotate;
    bx::mtxMul(worldRotate, viewRotate, _cameraTransform.rotMtx);
    _cameraTransform.rotMtx = worldRotate;
    
    gfx::Vector3 worldTranslate;
    bx::vec3MulMtx(worldTranslate, viewTranslate, _cameraTransform.rotMtx);
    _cameraTransform.pos += worldTranslate;
    
    gfx::Matrix4 translate;
    bx::mtxTranslate(translate, _cameraTransform.pos.x,
        _cameraTransform.pos.y,
        _cameraTransform.pos.z);
    
    bx::mtxMul(camera.worldMtx, _cameraTransform.rotMtx, translate);
}

}
