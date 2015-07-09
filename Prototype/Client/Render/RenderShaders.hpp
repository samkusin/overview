//
//  RenderShaders.hpp
//  Overview
//
//  Created by Samir Sinha on 7/7/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Render_Shaders_hpp
#define Overview_App_Render_Shaders_hpp

#include "Engine/Render/RenderTypes.hpp"

namespace cinek { namespace ovproto {

namespace render {

    enum
    {
        kShaderUniform_ColorTexture = 0
    };

    void registerShaders(gfx::ShaderLibrary& shaderLibrary);
}

} /* namespace ovproto */ } /* namespace cinek */

#endif
