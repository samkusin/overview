//
//  GalaxyViewController.h
//  Overview
//
//  Created by Samir Sinha on 7/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_GalaxyViewController_hpp
#define Overview_App_GalaxyViewController_hpp

#include "GameTypes.hpp"
#include "AppContext.hpp"
#include "RenderContext.hpp"

#include "UI/UITypes.hpp"
#include "Engine/ViewController.hpp"
#include "Engine/EngineMath.hpp"

#include "AppInterface.hpp"
#include "Services/RenderService.hpp"
#include "Services/EntityService.hpp"

#include <cinek/vector.hpp>
#include <array>

namespace cinek { namespace ovproto {

//  Responsible for generating torque on a single axis
//  Callers should assign a PIDTorqueController per each axis of control.
//  For example, a plane in 3D space would need two such controllers : pitch and
//  yaw.  The implementation itself doesn't reference any specific axis.
//
//  References:
//      PID Control of Physics Bodies
//      by James Wucher
//      from  http://www.gamedev.net/
//
//  Using the Proportional-Integral-Derivative technique detailed by the
//  referenced article.  The controller tracks a short term history for purposes
//  of integrating feedback
//
class PIDController
{
public:
    PIDController();
    PIDController(ckm::scalar proportionalK,
                  ckm::scalar integralK,
                  ckm::scalar derivativeK,
                  ckm::scalar dt);
    
    //  issue when starting a new turn action
    void resetSamples();
    
    //  TODO: likely the below methods can be consolidated into a single
    //  (calcAngularAcceleration) method
    //
    //  call once per steering cycle while turning to a target
    void addInput(ckm::scalar input);
    //  returns the desired angular acceleration for applying torque
    ckm::scalar output() const { return _output; }
    
private:
    static constexpr int32_t kReqInputs = 3;
    std::array<ckm::scalar, 8> _inputs;
    int32_t _inputTail;
    ckm::scalar _output;
    ckm::scalar _proportionalK;
    ckm::scalar _integralK;
    ckm::scalar _derivativeK;
    ckm::scalar _dt;
};


struct GalaxyStarmapRenderTree;


class GalaxyViewController : public overview::ViewController, public UISubscriber
{
public:
    GalaxyViewController(AppContext context, RenderContext renderContext);
    virtual ~GalaxyViewController();
    
    virtual void onViewLoad() override;
    virtual void onViewUnload() override;
    virtual void onViewAdded() override;
    virtual void onViewRemoved() override;
    virtual void onViewForeground() override;
    virtual void onViewBackground() override;
    virtual void layoutView() override;
    virtual void simulateView(double time, double dt) override;
    virtual void frameUpdateView(double dt) override;
    virtual int viewId() const override { return kViewControllerId_Galaxy; }
    
    virtual void onUIEvent(int evtId, UIevent evtType, const UIeventdata& data) override;
    
private:
    void starmapLayout();
    
    static void viewUIRenderHook(void* context, NVGcontext* nvg);
    
    void viewUIRender(NVGcontext* nvg);
    
    void setCurrentSystem(Entity system);
    
    void rotateSystemCamera(ckm::vec3 axis, ckm::scalar radians);
    void zoomSystemCamera(ckm::scalar offset);
    void buildCameraTransform(overview::TransformComponent& t);
    
private:
    Allocator _allocator;
    
    AppInterface _API;
    RenderService _Render;
    EntityService _Entity;
    
    unique_ptr<GalaxyStarmapRenderTree> _starmap;
    
    vector<Entity> _starmapLocalEntities;
    
    Entity _camera;
    Entity _ship;
    Entity _currentSystem;
    
    double _driveTimer;
    
    //  UI defines
    enum
    {
        kID_VIEW,
        kID_BUTTON,
        kID_BUTTON2,
        kID_BUTTON3
    };
    int _uiStarmapItem;
    
    //  used for selecting a star on the screen
    struct StarSystemUIBox
    {
        Entity entity;
        int32_t x,y;
        float z;
        int32_t radius2;
        int32_t radius;
    };
    
    vector<StarSystemUIBox> _starSystemBoxes;
    
    //  mouse rotation of starmap
    int _lastTrackMouseX, _lastTrackMouseY;
    int _trackMouseX, _trackMouseY;
    
    void resetMouseTrack();
    void updateMouseTrack(int x, int y);
    ckm::ivec2 mouseTrackDelta(int x, int y) const;
    
    ckm::vec3 _systemCameraCenter;
    ckm::quat _systemCameraRot;
    ckm::scalar _systemCameraDist;
    
    //  starmap rendering
    gfx::ShaderProgramId _shaderProgram;
    gfx::TextureHandle _starCorona;
    
    gfx::Matrix4 _mapTransform;

    enum
    {
        kUniform_Texture0,
        kUniform_Texture1,
        kUniform_Scale,
        kUniform_HandleCount
    };
    std::array<bgfx::UniformHandle, kUniform_HandleCount> _uniforms;
    


    //  Camera Driver
    Entity _driveTarget;
    PIDController _driveYaw;
    PIDController _drivePitch;
};

} /* namespace ovproto */ } /* namespace cinek */


#endif /* defined(Overview_App_GalaxyViewController_hpp) */
