//
//  RenderScene.hpp
//  Overview
//
//  Created by Samir Sinha on 5/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Render_Scene_hpp
#define Overview_Render_Scene_hpp

#include "Engine/Render/RenderTypes.hpp"
#include "Engine/Entity/EntityTypes.hpp"
#include "Engine/BVH/BVHStaticGraph.hpp"

#include <cinek/objectpool.hpp>

namespace cinek { namespace overview {

struct RenderResources
{
    cinek::gfx::TextureAtlas* textureAtlas;
    cinek::gfx::MeshLibrary* meshLibrary;
    cinek::gfx::ShaderLibrary* shaderLibrary;
};

struct RenderContext
{
    EntityDatabase* entityDb;
    RenderResources* resources;
    
    int32_t viewWidth;
    int32_t viewHeight;
};

void renderScene(RenderContext& context);

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Render_Scene_hpp) */
