//
//  Overview.h
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Overview_hpp
#define Overview_Overview_hpp

#include "ApplicationController.hpp"

#include "Engine/Director.hpp"

namespace cinek {
    namespace glx {
        class RendererCLI;
    }
    namespace overview {
        class WindowComponentCLI;
    }
}

namespace cinek {
    namespace overview {

    class Overview: public overview::Director
    {
    public:
        Overview(overview::WindowComponentCLI& uiCLI,
                 glx::RendererCLI& rendererCLI);
        virtual ~Overview();

        virtual void update(uint32_t ticks);

    private:
        Allocator _allocator;
        ApplicationController _controller;
    };

    }
}

#endif
