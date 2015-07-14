//
//  RenderDefines.hpp
//  Overview
//
//  Created by Samir Sinha on 7/9/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_RenderDefines_hpp
#define Overview_App_RenderDefines_hpp

#include "Engine/Render/RenderTypes.hpp"

namespace cinek { namespace ovproto {

enum
{
    kShaderIndex_Default    = 0,
    
    //  Variants
    //  kVertexType_Vec3_UV
    kShaderIndex_Star       = 1
};

enum
{
    kShaderUniform_ColorTexture0 = 0,
    kShaderUniform_ColorTexture1 = 1
};

enum
{
    kRenderPipeline_Default,
    kRenderPipeline_Starmap,
    kRenderPipeline_Count
};

} /* namespace ovproto */ } /* namespace cinek */

#endif
