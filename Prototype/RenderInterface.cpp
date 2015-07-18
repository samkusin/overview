//
//  RenderInterface.cpp
//  Overview
//
//  Created by Samir Sinha on 7/7/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "RenderInterface.hpp"
#include "AppInterface.hpp"

#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/TextureAtlas.hpp"

#include <cinek/debug.hpp>

namespace cinek {
    namespace ovproto {

RenderInterface::RenderInterface(AppInterface& api) :
    _context(api._context)
{
}

overview::RenderResources& RenderInterface::renderResources()
{
    return *_context->renderResources;
}

gfx::Rect RenderInterface::viewRect() const
{
    return _context->viewRect;
}
        
    } /* namespace ovproto */
} /* namespace cinek */