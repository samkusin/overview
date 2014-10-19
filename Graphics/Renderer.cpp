//
//  Renderer.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Renderer.hpp"
#include "RenderDefines.hpp"

namespace cinek {
    namespace glx {

    Renderer::Renderer(const RendererInitParameters& initParams,
                       const Allocator& allocator) :
        _allocator(allocator)
    {
    }

    }   // namespace glx
}   // namespace cinek
