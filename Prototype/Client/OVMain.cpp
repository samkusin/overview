//
//  OVMain.cpp
//  Overview
//
//  Created by Samir Sinha on 4/3/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "Tasks/GameTask.hpp"

#include "CKGfx/MeshLibrary.hpp"
#include "CKGfx/TextureAtlas.hpp"
#include "CKGfx/ShaderLibrary.hpp"

#include "Engine/Render/RenderScene.hpp"
#include "Engine/Entity/EntityDatabase.hpp"
#include "Engine/MessageDispatcher.hpp"
#include "Engine/MessagePublisher.hpp"
#include "Engine/MessageStream.hpp"

#include "Engine/Entity/Comp/Camera.hpp"
#include "Engine/Entity/Comp/Light.hpp"

#include "CKGfx/VertexTypes.hpp"

#include <cinek/file.hpp>
#include <cinek/taskscheduler.hpp>
#include <cinek/json/json.hpp>

#include <SDL2/SDL.h>                   // must include prior to bgfx includes
#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>


namespace cinek { namespace ovproto {

enum
{
    kPollSDLEvent_Quit = 0x0001
};


static uint32_t PollSDLEvents()
{
    uint32_t flags = 0;
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            flags |= kPollSDLEvent_Quit;
            break;
        }
    }
    
    return flags;
}

void run(SDL_Window* window)
{
    Allocator allocator;
    
    //  initialize rendering
    int viewWidth = 0;
    int viewHeight = 0;
    
    SDL_GetWindowSize(window, &viewWidth, &viewHeight);

    bgfx::reset(viewWidth, viewHeight, BGFX_RESET_VSYNC);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x111111ff,
        1.0f,
        0);
    
    gfx::VertexTypes::initialize();
    
    gfx::TextureAtlas textureAtlas(256, allocator);
    gfx::MeshLibrary meshLibrary(256, allocator);
    gfx::ShaderLibrary shaderLibrary(allocator);
    
    overview::RenderResources renderResources = {
        &textureAtlas,
        &meshLibrary,
        &shaderLibrary
    };
    
    shaderLibrary.loadProgram(0x00000001,
        "Shaders/vs_cubes.bin",
        "Shaders/fs_cubes.bin");
    
    overview::EntityDatabase entityDb(2048, {
        { overview::component::Camera::kComponentType, 4 },
        { overview::component::Light::kComponentType, 8 }
    });
    
    overview::RenderContext renderContext;
    renderContext.entityDb = &entityDb;
    renderContext.resources = &renderResources;
   
    TaskScheduler taskScheduler(64, allocator);
    
    //  messaging - use two streams, one that is active, and one for the next
    //  frame
    overview::MessageStream messages(256, allocator);
    overview::MessagePublisher messagePublisher(std::move(messages), 64, 32, 32, allocator);
    messages = std::move(overview::MessageStream(256, allocator));
    
    //  setup application master template
    AppDocumentMap appDocumentMap(allocator);
    
    //  schedule an application task
    AppContext context;
    
    context.entityDb = &entityDb;
    context.messagePublisher = &messagePublisher;
    context.renderResources = &renderResources;
    context.documentMap = &appDocumentMap;
    context.allocator = &allocator;
    context.createComponentCb =
        [&entityDb](overview::EntityObject& obj,
                    const cinek::JsonValue& definitions,
                    const cinek::JsonValue& data)
        {
        };
    
    //  should be moved into a task for "initialization"
    auto starMesh = gfx::createIcoSphere(1.0f, 3, gfx::VertexTypes::kVec3_Normal);
    meshLibrary.acquire(std::move(starMesh), "star");
    
    //  start app
    
    auto initialTask = allocate_unique<GameTask>(allocator, context);
    
    taskScheduler.schedule(std::move(initialTask));
    
    ////////////////////////////////////////////////////////////////////////////
    //  the main loop
    
    uint32_t lastSystemTime = SDL_GetTicks();
    bool running = true;
    
    while (running)
    {
        uint32_t thisSystemTime = SDL_GetTicks();
        uint32_t frameTimeMs = thisSystemTime - lastSystemTime;
        
        //  run tasks (including the application)
        taskScheduler.update(frameTimeMs);

        //  queue inputs
        uint32_t sdlEvents = PollSDLEvents();
        if (sdlEvents & kPollSDLEvent_Quit)
            running = false;
        
        //  dispatch messages, and set our new message target for the next
        //  frame.  setting a new stream allows message handlers to publish
        //  messages to our only message publisher.
        auto messagesForDispatch = overview::setMessageStreamForPublisher(std::move(messages),
            messagePublisher);

        overview::dispatchMessageStreamToPublisher(messagesForDispatch, messagePublisher);

        messages = std::move(messagesForDispatch);

        //  render scene
        bgfx::setViewRect(0, 0, 0, viewWidth, viewHeight);
        
        renderContext.viewWidth = viewWidth;
        renderContext.viewHeight = viewHeight;
        
        overview::renderScene(renderContext);

        bgfx::submit(0);
        bgfx::frame();
    
        lastSystemTime = thisSystemTime;
    }
}

} /* namespace ovproto */ } /* namespace cinek */


int OverviewMain(SDL_Window* window, int argc, char* argv[])
{
    cinek::file::setOpsStdio();
    
    //  renderer initialization
    //
    bgfx::sdlSetWindow(window);
    bgfx::init();
    
    cinek::ovproto::run(window);
    
    //  subsystem termination
    //
    bgfx::shutdown();
    
    return 0;
}
