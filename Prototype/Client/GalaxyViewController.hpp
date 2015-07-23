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
#include "UI/UITypes.hpp"
#include "AppInterface.hpp"
#include "Engine/ViewController.hpp"

#include <cinek/vector.hpp>

namespace cinek { namespace ovproto {

class GalaxyViewController : public overview::ViewController, public UISubscriber
{
public:
    GalaxyViewController(AppInterface api);
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
    
    struct Starmap;
    unique_ptr<Starmap> _starmap;
    
    Entity _camera;
    
    //  UI defines
    enum
    {
        kID_VIEW,
        kID_BUTTON
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
