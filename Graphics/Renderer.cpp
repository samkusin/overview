//
//  Renderer.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Renderer.hpp"
#include "RenderDefines.hpp"

namespace cinekine {
    namespace glx {

    Renderer::Renderer(const RendererInitParameters& initParams,
                       const Allocator& allocator) :
        _allocator(allocator),
        _bitmapLibrary(*this),
        _fontLibrary(*this, 10 /* TODO make this font limit configurable. */)
    {    
    }
    
    Renderer::~Renderer()
    {
    }
        
    }   // namespace glx
}   // namespace cinekine
