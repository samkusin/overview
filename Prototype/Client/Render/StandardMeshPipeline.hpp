//
//  StandardMeshPipeline.hpp
//  Overview
//
//  Created by Samir Sinha on 7/13/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Render_StandardMesh_hpp
#define Overview_App_Render_StandardMesh_hpp

#include "RenderDefines.hpp"

#include <array>

namespace cinek { namespace ovproto {

class StandardMeshPipeline
{

public:
    StandardMeshPipeline();
    
    void loadResources(overview::RenderResources& resources);
    void unloadResources(overview::RenderResources& resources);
    
    void operator()(overview::RenderContext context,
                    const overview::RenderCameraContext& camera,
                    overview::RenderObjectListReader& objects);

private:
    enum
    {
        kUniform_Texture0,
        kUniform_Texture1,
        kUniform_HandleCount
    };
    std::array<bgfx::UniformHandle, kUniform_HandleCount> _uniforms;
};


} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_App_Render_StarmapPipeline_hpp) */