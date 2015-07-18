//
//  RenderTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 7/5/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_RenderTypes_hpp
#define Overview_RenderTypes_hpp

#include "Engine/Entity/EntityTypes.hpp"
#include "CKGfx/GfxTypes.hpp"
#include "CKGfx/VertexTypes.hpp"

namespace cinek { namespace overview {

struct RenderResources
{
    cinek::gfx::TextureAtlas* textureAtlas;
    cinek::gfx::MeshLibrary* meshLibrary;
    cinek::gfx::ShaderLibrary* shaderLibrary;
};

struct RenderObject
{
    int32_t renderableIdx;
};

    
} /* namespace overview */ } /* namespace cinek */


#endif
