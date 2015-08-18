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

#include "Engine/ViewStack.hpp"

#include <cinek/file.hpp>
#include <cinek/json/json.hpp>
#include <cinek/taskscheduler.hpp>

#include <SDL2/SDL.h>                   // must include prior to bgfx includes
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>

#include "Engine/Entity/Transform.hpp"
#include "Engine/Render/Camera.hpp"
#include "Engine/Render/Renderable.hpp"
#include "Engine/Render/MeshRenderable.hpp"
#include "Engine/Render/RenderComponentFactory.hpp"

#include "Engine/Entity/TransformEntity.hpp"

#include "Custom/Comp/StellarSystem.hpp"
#include "Custom/Comp/StarBody.hpp"
#include "Custom/Comp/ComponentCreationCallback.hpp"
#include "Custom/Comp/Loadout.hpp"
#include "Custom/Comp/Party.hpp"
#include "Custom/Comp/Character.hpp"
#include "Custom/Comp/RigidBody.hpp"

#include "Physics/RigidBodySystem.hpp"

#include "Render/RenderShaders.hpp"

#include "Sim/SpectralClassUtility.hpp"
#include "Sim/EntityRoles.hpp"
#include "Sim/AIActionClient.hpp"
#include "Sim/Simulation.hpp"

#include "GalaxyViewController.hpp"

#include "UI/UIRenderer.hpp"
#include "UI/UIEngine.hpp"
#include "UI/oui.h"

#include "Diagnostics.hpp"


namespace cinek { namespace ovproto {

static int SDLCALL CustomSDLEventFilter(void *userdata, SDL_Event * event)
{
    //  these events do not need to be queued - the event values are obtained
    //  via polling as we use an IMGUI architecture relying on fast framerates
    switch (event->type)
    {
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
        return 0;
    }
    
    return 1;
}

enum
{
    kPollSDLEvent_Quit = 0x0001
};


static uint32_t PollSDLEvents()
{
    int mx, my;
    
    //  handle Mouse UI, which is polled per frame instead of set per event.
    uint32_t mbtn = SDL_GetMouseState(&mx, &my);
    
    uiSetCursor(mx, my);
    
    uiSetButton(0, 0, (mbtn & SDL_BUTTON_LMASK) != 0);
    uiSetButton(2, 0, (mbtn & SDL_BUTTON_RMASK) != 0);
    
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
                uiSetScroll(event.wheel.x, event.wheel.y);
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

void run(SDL_Window* window)
{
    Allocator allocator;
    
    //  initialize rendering
    int viewWidth = 0;
    int viewHeight = 0;
    
    SDL_GetWindowSize(window, &viewWidth, &viewHeight);
    
    SDL_SetEventFilter(CustomSDLEventFilter, NULL);
    
    bgfx::reset(viewWidth, viewHeight, BGFX_RESET_VSYNC);
    bgfx::setDebug(0
                    | BGFX_DEBUG_TEXT
                  //  | BGFX_DEBUG_STATS
                  );
    
    //const bgfx::Caps& bgfxCaps = *bgfx::getCaps();

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x000000ff,
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
    
    //  UI
    UIcontext* uiContext = uiCreateContext(4096, 1<<20);
    uiMakeCurrent(uiContext);
    uiSetHandler(OUIHandler);
    
    auto nvgContext = createUIRenderingContext(1);
    if (!nvgContext)
    {
        CK_LOG_ERROR("OVMain", "Failed to initialize UI Context");
        return;
    }
    
    //  Simulation Parameters
    //
    
    overview::EntityGroup::RoleLimits playerPartyRoles;
    playerPartyRoles[kPartyRole_Players] = 4;
    playerPartyRoles.fill(0);
    
    overview::EntityGroup::RoleLimits smallShipPartyRoles;
    smallShipPartyRoles[kPartyRole_Ship_Captain] = 1;
    smallShipPartyRoles[kPartyRole_Ship_Pilot] = 1;
    smallShipPartyRoles[kPartyRole_Ship_Navigation] = 1;
    smallShipPartyRoles[kPartyRole_Ship_Engineer] = 1;
    smallShipPartyRoles[kPartyRole_Ship_Science] = 1;
    smallShipPartyRoles[kPartyRole_Ship_Medicine] = 1;
    smallShipPartyRoles.fill(0);
    
    overview::EntityGroup::RoleLimits playerLoadoutCategories;
    playerLoadoutCategories[kLoadoutRole_Players_Hands] = kLoadout_Players_Slot_Hand_Count;
    playerLoadoutCategories[kLoadoutRole_Players_Body] = kLoadout_Players_Slot_Body_Count;
    playerLoadoutCategories.fill(0);
    
    overview::EntityGroup::RoleLimits smallShipLoadoutCategories;
    smallShipLoadoutCategories[kLoadoutRole_Ships_Weapons] = 1;
    smallShipLoadoutCategories[kLoadoutRole_Ships_Missiles] = 1;
    smallShipLoadoutCategories.fill(0);
    
    overview::EntityStore entityStore(kMaxEntities, {
        overview::component::Transform::makeDescriptor(kMaxTransforms),
        overview::component::Renderable::makeDescriptor(kMaxRenderables),
        overview::component::MeshRenderable::makeDescriptor(16*1024),
        overview::component::Camera::makeDescriptor(4),
        component::RigidBody::makeDescriptor(kMaxRigidBodies),
        component::StellarSystem::makeDescriptor(kMaxStarSystems),
        component::StarBody::makeDescriptor(kMaxStars),
        component::Character::makeDescriptor(16),
        component::Loadout::makeDescriptor(4),
        component::Party::makeDescriptor(4)
    },
    {
        { component::Loadout::kGroupId_Player, playerLoadoutCategories, 4 },
        { component::Loadout::kGroupId_SmallShip, smallShipLoadoutCategories, 4 },
        { component::Party::kGroupId_Player, playerPartyRoles, 4 },
        { component::Party::kGroupId_SmallShip, smallShipPartyRoles, 4 }
    });

    //  should be moved into a task for "initialization"
    if (!SpectralUtility::loadTables())
        return;
    
    //  AI System
    Simulation<AIAction> aiActionSim(64, allocator);
    AIActionClient aiActionClient;
    
    //  setup application master template
    AppDocumentMap appDocumentMap(allocator);
    
    overview::ViewStack viewStack(allocator);
    overview::ViewStack overlayStack(allocator);
    
    RenderObjects renderObjects;
    renderObjects.renderResources = &renderResources;
    renderObjects.viewRect = { 0, 0, viewWidth, viewHeight };
    renderObjects.nvg = nvgContext;
    
    AppObjects appObjects;
    appObjects.entityStore = &entityStore;
    appObjects.documentMap = &appDocumentMap;
    appObjects.allocator = &allocator;
    
    appObjects.createComponentCb = [&appObjects, &renderObjects]
        (
            Entity entity,
            const JsonValue& definitions,
            const char* componentName,
            const JsonValue& data
        )
        {
            overview::component::createRenderable(entity, definitions, componentName, data,
                *appObjects.entityStore, *renderObjects.renderResources);
            overview::component::createCamera(entity, definitions, componentName, data,
                *appObjects.entityStore);
            
            component::customComponentCreateCb(AppContext(appObjects),
                RenderContext(renderObjects),
                entity,
                definitions,
                componentName,
                data
            );
        };
    appObjects.destroyComponentCb = [&appObjects, &renderObjects]
        (
            Entity entity,
            overview::ComponentId componentId
        )
        {
            overview::component::destroyRenderable(entity, componentId,
                *appObjects.entityStore, *renderObjects.renderResources);
            overview::component::destroyCamera(entity, componentId,
                *appObjects.entityStore);
                
            component::customComponentDestroyCb(AppContext(appObjects),
                RenderContext(renderObjects),
                entity,
                componentId);
        };
    

    AppContext appContext(appObjects);
    RenderContext renderContext(renderObjects);
    
    /*
    auto starMesh = gfx::createIcoSphere(1.0f, 4, gfx::VertexTypes::kVec3_Normal_Tex0);
    auto starMeshHandle = meshLibrary.create("star");
    meshLibrary.setMeshForLOD(starMeshHandle, std::move(starMesh), gfx::LODIndex::kHigh);
    */
    
    //  schedule an application task
    //
    viewStack.setFactory(
        [&appContext, &renderContext](int viewId) -> unique_ptr<overview::ViewController>
        {
            unique_ptr<overview::ViewController> vc;
            Allocator allocator;
            switch (viewId)
            {
            case kViewControllerId_Galaxy:
                vc = allocate_unique<GalaxyViewController, overview::ViewController>(
                    allocator,
                    appContext,
                    renderContext
                );
                break;
            }
            
            return vc;
        });

    viewStack.present(kViewControllerId_Galaxy);
    
    ////////////////////////////////////////////////////////////////////////////
    //  the main loop
    //
    Diagnostics diagnostics;
    overview::EntityDiagnostics entityDiagnostics;
    
    const double kSimFPS = 60.0;
    const double kSecsPerSimFrame = 1/kSimFPS;
    const double kAIFPS = 2.0;
    const double kSecsPerAIFrame = 1/kAIFPS;
    const double kActionFPS = 30.0;
    const double kSecsPerActionFrame = 1/kActionFPS;

    uint32_t systemTimeMs = SDL_GetTicks();
    double simTime = 0.0;
    double lagSecsSim = 0.0;
    double timeUntilAIFrame = 0.0;
    double timeUntilActionFrame = 0.0;
    
    bool running = true;
    
    auto transforms = entityStore.table<overview::component::Transform>();
    overview::component::UpdateTransform updateTransform(transforms);
    
    auto rigidBodies = entityStore.table<component::RigidBody>();
    
    while (running)
    {
        uint32_t nextSystemTimeMs = SDL_GetTicks();
        int32_t frameTimeMs = nextSystemTimeMs - systemTimeMs;
        systemTimeMs = nextSystemTimeMs;
        
        //  TODO: Lag should not be incremented while Sim is Paused
        lagSecsSim += frameTimeMs/1000.0;
    
        entityStore.diagnostics(entityDiagnostics);
    
        ////////////////////////////////////////////////////////////////////////
        //  SYSTEM POLL AND INPUT
        //
        {
            uint32_t sdlEvents = PollSDLEvents();
            if (sdlEvents & kPollSDLEvent_Quit)
                running = false;
        }
        
        ////////////////////////////////////////////////////////////////////////
        //  SIMULATION START (using a fixed timestep)
        //  All subsystems driven by the physics framerate.
        //
        while (lagSecsSim >= kSecsPerSimFrame)
        {
            ////////////////////////////////////////////////////////////////////
            // AI Behavior Simulation
            //
            timeUntilAIFrame -= kSecsPerSimFrame;
            while (timeUntilAIFrame < 0)
            {
                //  dispatch messages, and set our new message target for the
                //  next frame.  setting a new stream allows message handlers to
                //  publish messages to our only message publisher.
                //aiControl.simulate(aiGameClient, simTime);
                
                timeUntilAIFrame += kSecsPerAIFrame;
            }
            
            ////////////////////////////////////////////////////////////////////
            // Action (Game State) Simulation
            //
            timeUntilActionFrame -= kSecsPerActionFrame;
            while (timeUntilActionFrame < 0)
            {
                //  AI action tasks
                aiActionSim.simulate(aiActionClient, simTime, kSecsPerActionFrame);
                
                //  simulation of current view
                viewStack.process();
                overlayStack.process();
                
                timeUntilActionFrame += kSecsPerActionFrame;
            }
            
            ////////////////////////////////////////////////////////////////////
            // Physics and Collision Simulations
            //
            simulateRigidBodies(rigidBodies, transforms, kSecsPerSimFrame);
            
            //  TODO : We should only update entities that have changed
            //  transforms.  While checking for the 'dirty' flag per transform
            //  saves some time, we should exclude static entities and such
            //  from this sweep.  Perhaps we need a separate list of 'changed'
            //  entities compiled from the simulateRigidBodies sweep.
            updateTransform.all();

            ////////////////////////////////////////////////////////////////////
            //  Update our simulation times
            //
            lagSecsSim -= kSecsPerSimFrame;
            simTime += kSecsPerSimFrame;
            
            diagnostics.incrementRateGauge(Diagnostics::kFrameRate_Update);
        }
        //  SIMULATION END
        ////////////////////////////////////////////////////////////////////////
        
        diagnostics.updateTime(systemTimeMs, simTime*1000);
        
        ////////////////////////////////////////////////////////////////////////
        //  RENDER START (TODO: Take Lag into Account for Interp)
        {
            uiBeginLayout();
            viewStack.layout();
            overlayStack.layout();
            uiEndLayout();
            
            viewStack.render();
            overlayStack.render();
            
            renderUI(renderObjects.nvg, renderObjects.viewRect);
            renderDiagnostics(diagnostics, entityDiagnostics,
                renderObjects.nvg, renderObjects.viewRect);
        }
        //  RENDER END
        ////////////////////////////////////////////////////////////////////////
        
        uiProcess(systemTimeMs);
        
        bgfx::frame();

        entityStore.gc();
    
        diagnostics.incrementRateGauge(Diagnostics::kFrameRate_Main);
    }
    
    viewStack.pop();
    overlayStack.pop();
    
    SpectralUtility::unloadTables();
    
    destroyUIRenderingContext(nvgContext);
    nvgContext = nullptr;
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
