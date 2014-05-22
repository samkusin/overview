//
//  Overview.h
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Overview_hpp
#define Overview_Overview_hpp

#include "SceneController.hpp"
#include "Engine/Director.hpp"

namespace cinekine {
    namespace glx {
        class RendererCLI;
    }
    namespace ovengine {
        class WindowComponentCLI;
    }
}

namespace cinekine {
    namespace prototype {

    class Overview: public ovengine::Director
    {
    public:
        Overview(ovengine::WindowComponentCLI& uiCLI,
                 glx::RendererCLI& rendererCLI);
        virtual ~Overview();

        virtual void update();

    private:
        Allocator _allocator;
        SceneController _sceneController;
    };

    }
}

#endif
