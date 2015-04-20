//
//  ClientController.cpp
//  Overview
//
//  Created by Samir Sinha on 4/15/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "ClientController.hpp"
#include <bgfx/bgfx.h>

namespace cinek { namespace ovproto {

overview::MessageBuffer run(overview::MessageBuffer msgBuffer)
{
    overview::MessageStream msgStream(msgBuffer);
    
    //  ui subsystem initialization
    //
    
    //  StageController initialization
    
    
    //  Main Loop
    /*
    bool active = true;
    while (active)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                active = false;
            }
        }
     
        // Set view 0 default viewport.
		bgfx::setViewRect(0, 0, 0, viewWidth, viewHeight);
        bgfx::submit(0);
        
        bgfx::frame();
    }
    */
    
    return msgBuffer;
}
    
    }
}

