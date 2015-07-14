//
//  StarmapRenderPipeline.hpp
//  Overview
//
//  Created by Samir Sinha on 7/9/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Render_StarmapPipeline_hpp
#define Overview_App_Render_StarmapPipeline_hpp

#include "RenderDefines.hpp"

#include <cinek/vector.hpp>
#include <array>

namespace cinek { namespace ovproto {


class StarmapRenderPipeline
{

public:
    StarmapRenderPipeline();
    
    void loadResources(overview::RenderResources& resources);
    void unloadResources(overview::RenderResources& resources);
    
    void operator()(overview::RenderContext context,
                    const overview::RenderCameraContext& camera,
                    overview::RenderObjectListReader& objects);

private:
    gfx::ShaderProgramId _shaderProgram;
    gfx::TextureHandle _starCorona;
    
    gfx::Matrix4 _mapTransform;
    
    int _starLimit;                     // number of stars per render call
    
    enum
    {
        kUniform_Texture0,
        kUniform_Texture1,
        kUniform_Scale,
        kUniform_HandleCount
    };
    std::array<bgfx::UniformHandle, kUniform_HandleCount> _uniforms;
};


} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_App_Render_StarmapPipeline_hpp) */
