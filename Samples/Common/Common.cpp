//
//  Common.cpp
//  SampleCommon
//
//  Created by Samir Sinha on 11/8/15.
//  Copyright © 2015 Cinekine. All rights reserved.
//

#include "Common.hpp"

#include "UICore/UIEngine.hpp"

#include <cinek/file.hpp>

#include <SDL2/SDL_syswm.h>

#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>



////////////////////////////////////////////////////////////////////////////////

int OverviewMain(SDL_Window* window, int argc, char* argv[])
{
    cinek::file::setOpsStdio();
    
    int viewWidth;
    int viewHeight;
    
    SDL_GetWindowSize(window, &viewWidth, &viewHeight);

    //  renderer initialization
    //
    bgfx::sdlSetWindow(window);
    bgfx::init();

    bgfx::reset(viewWidth, viewHeight, BGFX_RESET_VSYNC);
    bgfx::setDebug(BGFX_DEBUG_TEXT /*|BGFX_DEBUG_STATS*/ );
    
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x001122ff,
        1.0f,
        0);
    
    imGuiInit(window, 1);

    int result = runSample(viewWidth, viewHeight, 2);
    
    imGuiShutdown();
    
    //  subsystem termination
    //
    bgfx::shutdown();
    
    return result;
}