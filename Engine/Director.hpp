//
//  Director.hpp
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Director_hpp
#define Overview_Director_hpp

#include "ViewCreate.hpp"

namespace cinekine {
    namespace glx {
        class RendererCLI;
    }
}

namespace cinekine {
    namespace ovengine {

    class WindowComponentCLI;

    //  The Director drives execution of the simulation based on an external application controller
    //  Currently the Engine only initializes and destroys the Director object
    class Director
    {
    public:
        virtual ~Director() {}

        /**
         * Updates the director - called once per frame.
         */
        virtual void update() = 0;
    };

    //  must be defined by the implementing application.
    Director* CreateDirector(WindowComponentCLI& winCLI,
                             glx::RendererCLI& rendererCLI);

    //  must be defined by the implementing application - destroys the Director created by
    //  CreateDirector
    void DestroyDirector(Director* director);

    }   // namespace ovengine
}   // namespace cinekine

#endif
