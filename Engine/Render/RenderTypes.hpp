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

struct RenderContext
{
    overview::EntityStore* entityStore;
    overview::RenderResources* resources;
    
    int32_t viewWidth;
    int32_t viewHeight;
};

struct RenderObject
{
    int32_t renderableIdx;
};

class Renderer;
class RenderObjectListWriter;
class RenderObjectListReader;

using BuildRenderObjectListCb = std::function<void(const ckm::Frustrum&,
    RenderObjectListWriter&)>;
using RenderPipelineCb = std::function<void(RenderContext,
    RenderObjectListReader&)>;
    
} /* namespace overview */ } /* namespace cinek */


#endif
