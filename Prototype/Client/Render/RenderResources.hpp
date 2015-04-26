//
//  RenderResources.hpp
//  Overview
//
//  Created by Samir Sinha on 4/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Render_Resources_hpp
#define Overview_Render_Resources_hpp

#include "CKGfx/GfxTypes.hpp"
#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/VertexTypes.hpp"
#include "CKGfx/TextureAtlas.hpp"

namespace cinek { namespace overview {

struct RenderResources
{
    cinek::gfx::VertexDefintions vertexDecls;
    cinek::gfx::ShaderLibrary shaders;
    cinek::gfx::TextureAtlas textures;
    
    struct InitParams
    {
        gfx::VertexDefintions vertexDefs;
        uint32_t textureCount;
    };
    
    RenderResources(const InitParams& params,
                    const Allocator& allocator=Allocator());
};

inline RenderResources::RenderResources
(
    const InitParams& params,
    const Allocator& allocator
) :
    vertexDecls(std::move(params.vertexDefs)),
    shaders(allocator),
    textures(params.textureCount, allocator)
{
}

} /* namespace overview */ } /* namespace cinek */

#endif /* defined(Overview_Render_Resources_hpp) */
