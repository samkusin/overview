//
//  Main.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/23/15.
//
//

#include "CKGfx/VertexTypes.hpp"
#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/Context.hpp"

#include <cinek/file.hpp>
#include <cinek/allocator.hpp>
#include <cinek/objectpool.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_mouse.h>
#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>

#include "UICore/UITypes.hpp"
#include "UICore/UIEngine.hpp"
#include "UICore/UIRenderer.hpp"

#include "UICore/oui.h"

////////////////////////////////////////////////////////////////////////////////

enum
{
    kPollSDLEvent_Quit = 0x0001
};

struct PollStateSDL
{
    int mx, my;
    int mxWheel, myWheel;
    uint32_t mbtn;
};

static uint32_t pollSDLEvents(PollStateSDL& state)
{
    int mx, my;
    
    //  handle Mouse UI, which is polled per frame instead of set per event.
    uint32_t mbtn = SDL_GetMouseState(&mx, &my);
    
    
    uiSetCursor(mx, my);
    
    uiSetButton(0, 0, (mbtn & SDL_BUTTON_LMASK) != 0);
    uiSetButton(2, 0, (mbtn & SDL_BUTTON_RMASK) != 0);
    
    state.mx = mx;
    state.my = my;
    state.mbtn = mbtn;
    state.mxWheel = 0;
    state.myWheel = 0;
    
    //  poll system and key events
    uint32_t flags = 0;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            flags |= kPollSDLEvent_Quit;
            break;
        }
        else if (event.type == SDL_MOUSEWHEEL)
        {
            if (event.wheel.which != SDL_TOUCH_MOUSEID)
            {
                state.mxWheel += event.wheel.x;
                state.myWheel += event.wheel.y;
                uiSetScroll(state.mxWheel, state.myWheel);
            }
        }
        else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            SDL_KeyboardEvent& kevt = event.key;
            if (!kevt.repeat)
            {
                uiSetKey(kevt.keysym.scancode, kevt.keysym.mod, kevt.state);
            }
        }
        else if (event.type == SDL_TEXTINPUT)
        {
            uiSetChar(event.text.text[0]);
        }
    }
    
    return flags;
}

static int run(SDL_Window* window)
{
    int viewWidth, viewHeight;
    
    SDL_GetWindowSize(window, &viewWidth, &viewHeight);

    bgfx::reset(viewWidth, viewHeight, BGFX_RESET_VSYNC);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x002244ff,
        1.0f,
        0);
    
    //  UI
    UIcontext* uiContext = uiCreateContext(4096, 1<<20);
    uiMakeCurrent(uiContext);
    uiSetHandler(cinek::uicore::OUIHandler);
    
    NVGcontext* nvg = cinek::uicore::createRenderingContext(1);
    if (!nvg)
        return 1;
    {
        //  GFX
        cinek::Allocator allocator;
        cinek::gfx::Rect viewRect = { 0, 0, viewWidth, viewHeight };
        cinek::gfx::VertexTypes::initialize();
        cinek::gfx::ShaderLibrary shaderLibrary;
        
        cinek::gfx::Context::ResourceInitParams gfxInitParams;
        gfxInitParams.numMeshes = 1024;
        gfxInitParams.numMaterials = 1024;
        gfxInitParams.numTextures = 256;
        
        cinek::gfx::Context gfxContext(gfxInitParams);
        
        uint32_t systemTimeMs = SDL_GetTicks();
        bool running = true;
        
        while (running) {
            uint32_t nextSystemTimeMs = SDL_GetTicks();
            int32_t frameTimeMs = nextSystemTimeMs - systemTimeMs;
            systemTimeMs = nextSystemTimeMs;

            PollStateSDL polledInputState;
            
            if (pollSDLEvents(polledInputState) & kPollSDLEvent_Quit)
                running = false;
            
            
            gfxContext.update();
    
            bgfx::setViewRect(0, viewRect.x, viewRect.y, viewRect.w, viewRect.h);
            bgfx::submit(0);
            
            cinek::uicore::render(nvg, viewRect);
            
            uiProcess(systemTimeMs);
            bgfx::frame();
        }
    }
    
    cinek::uicore::destroyRenderingContext(nvg);
    
    return 0;
}


int OverviewMain(SDL_Window* window, int argc, char* argv[])
{
    cinek::file::setOpsStdio();
    
    //  renderer initialization
    //
    bgfx::sdlSetWindow(window);
    bgfx::init();
    
    int result = run(window);
    
    //  subsystem termination
    //
    bgfx::shutdown();
    
    return result;
}
