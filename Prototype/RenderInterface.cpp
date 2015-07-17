//
//  RenderInterface.cpp
//  Overview
//
//  Created by Samir Sinha on 7/7/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "RenderInterface.hpp"
#include "AppInterface.hpp"

#include "Engine/Render/Renderer.hpp"

#include <cinek/debug.hpp>

namespace cinek {
    namespace ovproto {

RenderInterface::RenderInterface(AppInterface& api) :
    _context(api._context)
{
}

void RenderInterface::registerObjectListHandler
(
    uint32_t viewIndex,
    const overview::BuildRenderObjectListCb& cb
)
{
    _context->renderer->onBuildObjectList(viewIndex, cb);
}

void RenderInterface::unregisterObjectListHandler(uint32_t viewIndex)
{
    _context->renderer->delBuildObjectListHandler(viewIndex);
}

        
    } /* namespace ovproto */
} /* namespace cinek */