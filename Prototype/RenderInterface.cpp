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

Handle RenderInterface::registerObjectListHandler
(
    const overview::BuildRenderObjectListCb& cb
)
{
    return _context->renderer->onBuildObjectList(cb);
}

void RenderInterface::unregisterObjectListHandler(Handle handle)
{
    _context->renderer->delBuildObjectListHandler(handle);
}

        
    } /* namespace ovproto */
} /* namespace cinek */