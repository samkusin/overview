//
//  StageApplication.cpp
//  Overview
//
//  Created by Samir Sinha on 5/22/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "GameTypes.hpp"
#include "StageApplication.hpp"
#include "GameStage/GameStage.hpp"

#include "Engine/Render/RenderResources.hpp"
#include "Engine/Render/RenderScene.hpp"

#include "CKGfx/Mesh.hpp"

#include "entityevents.capnp.h"

namespace cinek { namespace ovproto {

StageApplication::StageApplication
(
    overview::EntityDatabase& entityDb,
    overview::RenderResources& renderResources,
    const Allocator& allocator
) :
    EngineContext(256, entityDb, renderResources, allocator),
    _allocator(allocator)
{
    //  upload predefined meshes
    auto mesh = gfx::createIcoSphere(1.0f, 3, gfx::VertexTypes::kVec3_Normal);
    
    EngineContext::renderResources().meshes.acquire(std::move(mesh), "star");
    
    //  generate initial stage
    _stage = allocate_unique<ovproto::GameStage>(_allocator);
    
    _scene = allocate_unique<overview::RenderScene>(
        _allocator,
        256,
        entityDb,
        renderResources,
        _allocator
    );
    
    //  event handlers
    EngineContext::onEvent(overview::msg::kCreateEntityEvent,
        [this](const overview::MessageHeader::ClassId&,
               capnp::MessageReader& reader,
               overview::MessageHeader::SequenceId)
        {
            auto createEvent = reader.getRoot<overview::msg::CreateEntityEvent>();
            EntityId eid = createEvent.getEntityId();
            _scene->addEntity(eid, overview::RenderScene::EntityType::kStatic);
        }
    );
}
    
void StageApplication::update(uint32_t deltaMs)
{
    _stage->update(*this);
    
    dispatchMessages();
}

void StageApplication::render()
{
   _scene->render();
}

} /* namespace ovproto */ } /* namespace cinek */
