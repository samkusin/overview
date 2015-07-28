//
//  RenderInterface.cpp
//  Overview
//
//  Created by Samir Sinha on 7/7/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "RenderService.hpp"

#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/TextureAtlas.hpp"

#include <cinek/debug.hpp>

namespace cinek {
    namespace ovproto {

RenderService::RenderService(AppContext& context) :
    _context(context._o)
{
}

overview::RenderResources& RenderService::renderResources()
{
    return *_context->renderResources;
}

gfx::TextureHandle RenderService::loadTexture(const char* pathname)
{
    return _context->renderResources->textureAtlas->loadTexture(pathname);
}

void RenderService::unloadTexture(gfx::TextureHandle handle)
{
    _context->renderResources->textureAtlas->unloadTexture(handle);
}


bgfx::TextureHandle RenderService::bgfxTextureFromHandle(gfx::TextureHandle handle) const
{
    return _context->renderResources->textureAtlas->texture(handle);
}

gfx::Rect RenderService::viewRect() const
{
    return _context->viewRect;
}


NVGcontext* RenderService::obtain2DAPI()
{
    return _context->nvg;
}
        
    } /* namespace ovproto */
} /* namespace cinek */