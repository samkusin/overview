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

#include "AppInterface.hpp"
#include "Services/RenderService.hpp"
#include "Services/EntityService.hpp"

#include <cinek/vector.hpp>
#include <array>

namespace cinek { namespace ovproto {

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
    virtual void updateView() override;
    virtual void renderView() override;
    virtual int viewId() const override { return kViewControllerId_Galaxy; }
    
    virtual void onUIEvent(int evtId, int evtType) override;
    
private:
    Allocator _allocator;
    
    AppInterface _API;
    RenderService _Render;
    EntityService _Entity;
    
    struct Starmap;
    unique_ptr<Starmap> _starmap;
    
    Entity _camera;
    Entity _ship;
    int _frameCnt;
    
    //  UI defines
    enum
    {
        kID_VIEW,
        kID_BUTTON,
        kID_BUTTON2,
        kID_BUTTON3
    };
    
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
    
    using RenderObjectList = vector<overview::RenderObject>;
    RenderObjectList _renderables;
};

} /* namespace ovproto */ } /* namespace cinek */


#endif /* defined(Overview_App_GalaxyViewController_hpp) */
