//
//  OVMain.cpp
//  Overview
//
//  Created by Samir Sinha on 4/3/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "AppContext.hpp"

#include "CKGfx/MeshLibrary.hpp"
#include "CKGfx/TextureAtlas.hpp"
#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/VertexTypes.hpp"

#include "Engine/Entity/EntityStore.hpp"
#include "Engine/MessageDispatcher.hpp"
#include "Engine/MessagePublisher.hpp"
#include "Engine/MessageStream.hpp"

#include "Engine/Entity/Comp/Camera.hpp"
#include "Engine/Entity/Comp/Light.hpp"
#include "Engine/Entity/Comp/Transform.hpp"
#include "Engine/Entity/Comp/Renderable.hpp"
#include "Engine/Entity/Comp/MeshRenderable.hpp"

#include "Engine/ViewStack.hpp"

#include <cinek/file.hpp>
#include <cinek/json/json.hpp>

#include <SDL2/SDL.h>                   // must include prior to bgfx includes
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>

#include "Custom/Comp/StellarSystem.hpp"
#include "Custom/Comp/StarBody.hpp"

#include "Render/RenderShaders.hpp"

#include "Sim/SpectralClassUtility.hpp"

#include "UI/UISystem.hpp"

#include "GalaxyViewController.hpp"


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
    
    registerShaders(shaderLibrary);
    
    overview::EntityStore entityStore(kMaxEntities, {
        { overview::component::Transform::kComponentType, kMaxTransforms },
        { overview::component::Renderable::kComponentType, kMaxRenderables },
        { overview::component::MeshRenderable::kComponentType, 16*1024 },
        { component::StellarSystem::kComponentType, kMaxStarSystems },
        { component::StarBody::kComponentType, kMaxStars },
        { overview::component::Camera::kComponentType, 4 },
        { overview::component::Light::kComponentType, 8 }
    });
    
    //  UI
    //overview::RenderView uiView(0,0,viewWidth,viewHeight);
    //uiView.sequentialSubmit = true;
    //renderer.initView(16, uiView);
    
    //UISystem uiSystem(16, renderer);
    
    //  messaging - use two streams, one that is active, and one for the next
    //  frame
    overview::MessageStream messages(256, allocator);
    overview::MessagePublisher messagePublisher(std::move(messages), 64, 32, 32, allocator);
    messages = std::move(overview::MessageStream(256, allocator));
    
    //  setup application master template
    AppDocumentMap appDocumentMap(allocator);
    
    //  schedule an application task
    overview::ViewStack viewStack(allocator);
    AppContext context;
    
    context.entityStore = &entityStore;
    context.viewStack = &viewStack;
    context.messagePublisher = &messagePublisher;
    context.renderResources = &renderResources;
    context.viewRect = { 0, 0, viewWidth, viewHeight };
    context.documentMap = &appDocumentMap;
    context.allocator = &allocator;
    context.createComponentCb =
        [&entityStore](overview::Entity entity,
                    const cinek::JsonValue& definitions,
                    const cinek::JsonValue& data)
        {
        };
    
    //  should be moved into a task for "initialization"
    if (!SpectralUtility::loadTables())
        return;
    
    auto starMesh = gfx::createIcoSphere(1.0f, 4, gfx::VertexTypes::kVec3_Normal_Tex0);
    auto starMeshHandle = meshLibrary.create("star");
    meshLibrary.setMeshForLOD(starMeshHandle, std::move(starMesh), gfx::LODIndex::kHigh);
    
    viewStack.setFactory(
        [&context](int viewId) -> unique_ptr<overview::ViewController>
        {
            unique_ptr<overview::ViewController> vc;
            Allocator allocator;
            switch (viewId)
            {
            case kViewControllerId_Galaxy:
                vc = allocate_unique<GalaxyViewController, overview::ViewController>(
                    allocator,
                    AppInterface(context)
                );
                break;
            }
            
            return vc;
        });

    viewStack.present(kViewControllerId_Galaxy);
    
    ////////////////////////////////////////////////////////////////////////////
    //  the main loop
    
    uint32_t lastSystemTime = SDL_GetTicks();
    bool running = true;
    
    while (running)
    {
        uint32_t thisSystemTime = SDL_GetTicks();
        uint32_t frameTimeMs = thisSystemTime - lastSystemTime;

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
        
        viewStack.process();

        viewStack.render();
        
        bgfx::frame();

        entityStore.gc();

        lastSystemTime = thisSystemTime;
    }
    
    SpectralUtility::unloadTables();
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
