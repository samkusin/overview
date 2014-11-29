//
//  Simulation.cpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/Simulation.hpp"
#include "Game/World.hpp"
#include "Shared/GameTemplates.hpp"
#include "Engine/Model/TileGridMap.hpp"
#include "Engine/Model/SpriteLibrary.hpp"
#include "Core/MessageQueue.hpp"

#include "Game/Messages/SimMessageClassIds.hpp"
#include "Game/Messages/CreateEntityRequest.hpp"
#include "Game/Messages/CreateEntityResponse.hpp"

#include "Game/Events/SimEventClassIds.hpp"
#include "Game/Events/CreateEntityEvent.hpp"

namespace cinek {
    using namespace overview;
}

namespace cinek { namespace overview {

Simulation::Simulation(
    const GameTemplates& gameTemplates,
    const SimulationParams& params
) :
    _allocator(params.allocator),
    _entityAllocator(_allocator),
    _gameTemplates(gameTemplates),
    _systemTimeMs(0),
    _commandDispatcher(128, 32, 0, _allocator),
    _eventDispatcher(128, 16, 128, _allocator),
    _activeEventQueue(0),
    _scheduler(16, _allocator),
    _nextObjectId(0),
    _entityMap(_entityAllocator)
{
    _entityMap.reserve(params.entityLimit);
    
    for (auto& eventQueue : _eventQueues)
    {
        MessageQueue q(1024, _allocator);
        eventQueue = std::move(q);
    }
    
    auto tileDims = gameTemplates.tileGridMap()->overlayDimensions();
    auto tileHeight = gameTemplates.tileGridMap()->overlayToFloorRatio();

    //  Note, the 'World' is a virtual 3D world (XZ plane with an up +Y)
    World::CreateParams worldParams;
    worldParams.bounds.min = Point(0,0,0);
    worldParams.bounds.max = Point(tileDims.x, tileHeight, tileDims.y);
    worldParams.objectLimit = 1024;

    _world = allocate_unique<World>(_allocator,
                                    worldParams,
                                    _allocator);
    
    _commandDispatcher.on(SimCommand::kCreateEntity,
        [this](const SDO* data, Message::SequenceId seqId, void* context)
        {
            SimulationContext& simContext = *reinterpret_cast<SimulationContext*>(context);
            auto req = sdo_cast<const CreateEntityRequest*>(data);
            if (req)
            {
                simContext.sequenceId = seqId;
                createEntityCommand(*req,
                    [&simContext](const CreateEntityResponse& resp) {
                        simContext.resultMsgQueue->push(SimCommand::kCreateEntity,
                            resp,
                            simContext.sequenceId);
                    });
            }
        });
    
    _eventDispatcher.on(SimEvent::kCreateEntity,
        [this](const SDO* data, Message::SequenceId, void* context)
        {
            SimulationContext& simContext = *reinterpret_cast<SimulationContext*>(context);
            auto evt = sdo_cast<const CreateEntityEvent*>(data);
            if (evt)
            {
                simContext.resultMsgQueue->push(SimEvent::kCreateEntity, *evt);
            }
        });
}

Simulation::~Simulation()
{
    for (auto& entity : _entityMap)
    {
        auto body = entity.second.detachBody();
        _world->destroyObject(body);
    }
}

void Simulation::update(MessageQueue& inQueue, MessageQueue& outQueue,
        uint32_t timeMs)
{
    uint32_t deltaTimeMs = timeMs - _systemTimeMs;
    _systemTimeMs = timeMs;
    
    SimulationContext simContext;
    simContext.eventQueue = &_eventQueues[_activeEventQueue];
    simContext.resultMsgQueue = &outQueue;
    simContext.scheduler = &_scheduler;
    simContext.allocator = _allocator;
    //  process commands
    //  every command has a "command" key. dispatch the handler matching
    //  the command's key.
    _commandDispatcher.dispatch(inQueue, timeMs, &simContext);
    
    //  update simulation subsystems
    //
    _world->update(deltaTimeMs);
    _scheduler.process(deltaTimeMs);
    
    //  process events created during world/task execution
    //  the process will also fill our result queue
    MessageQueue& events = activeEventQueue();
    _activeEventQueue = (_activeEventQueue+1) % _eventQueues.size();
    _eventDispatcher.dispatch(events, timeMs, &simContext);
}


/*
void Simulation::createEntity(const std::string& templateId,
                              const SimulationCallback<CreateEntityResponse>& cb)
{
    auto& tmpl = _gameTemplates.entityTemplateCollection()[templateId];
    if (!tmpl)
    {
        OVENGINE_LOG_WARN("Simulation.createEntity - invalid template %s specified",
                          templateId.c_str());
        return 0;
    }
    EntityId newEntityId = (_nextObjectId+1);
    if (!newEntityId)
        newEntityId = 1;
    
    auto entityIt = _entityMap.emplace(std::piecewise_construct,
                        std::forward_as_tuple(newEntityId),
                        std::forward_as_tuple(newEntityId, tmpl)).first;
    
    if (entityIt != _entityMap.end())
    {
        _nextObjectId = newEntityId;
        return _nextObjectId;
    }
    
    OVENGINE_LOG_WARN("Simulation.createEntity - failed to create entity");
    return 0;
}
    
void Simulation::activateEntity(EntityId id, const Point& pos,
                                const Point& front)
{
    // add to or validate that the entity is in the World
    // attach entity to the world
    
    // TODO: create the WorldBody here instead of createEntity
    auto entityIt = _entityMap.find(id);
    if (entityIt == _entityMap.end())
    {
        OVENGINE_LOG_WARN("Simulation.activateEntity - no entity found (%u)", id);
        return;
    }
    auto& entity = entityIt->second;
    auto& tmpl = entity.sourceTemplate();
    auto& spriteTmpl = _gameTemplates.spriteLibrary().spriteByName(tmpl.spriteName());
    WorldObject* body = _world->createObject(pos, front, spriteTmpl.aabb());
    if (!body)
    {
        OVENGINE_LOG_WARN("Simulation.createEntity - failed to create body");
        return;
    }
    
    entity.attachBody(body);
    
    auto event = allocate_unique<EntityStateEvent>(
                                _allocator,
                                entity.id(),
                                EntityStateEvent::kActivated
                            );
    _eventQueue.queue(std::move(event));
}
    
void Simulation::deactivateEntity(EntityId id)
{
    auto entityIt = _entityMap.find(id);
    if (entityIt == _entityMap.end())
    {
        OVENGINE_LOG_WARN("deactivateEntity - no entity found (%u)", id);
        return;
    }
    auto& entity = entityIt->second;
    deactivateEntity(entity);
}
    
void Simulation::destroyEntity(EntityId id)
{
    auto entityIt = _entityMap.find(id);
    if (entityIt == _entityMap.end())
    {
        OVENGINE_LOG_WARN("destroyEntity - no entity found (%u)", id);
        return;
    }
    auto& entity = entityIt->second;
    deactivateEntity(entity);
    auto event = allocate_unique<EntityStateEvent>(
                                _allocator,
                                entity.id(),
                                EntityStateEvent::kDestroyed
                            );
    _eventQueue.queue(std::move(event));
}
    
void Simulation::deactivateEntity(Entity& entity)
{
    auto body = entity.detachBody();
    if (!body)
        return;
    _world->destroyObject(body);

    auto event = allocate_unique<EntityStateEvent>(
                                _allocator,
                                entity.id(),
                                EntityStateEvent::kDeactivated
                            );
    _eventQueue.queue(std::move(event));
}
*/

const Entity* Simulation::entity(EntityId entityId) const
{
    auto it = _entityMap.find(entityId);
    if (it == _entityMap.end())
        return nullptr;
    return &it->second;
}

Entity* Simulation::entity(EntityId entityId)
{
    return const_cast<Entity*>(static_cast<const Simulation*>(this)->entity(entityId));
}

void Simulation::createEntityCommand(
    const CreateEntityRequest& req,
    ResponseCallback<CreateEntityResponse> respCb
)
{
    CreateEntityResponse resp;
    auto& tmpl = _gameTemplates.entityTemplateCollection()[req.templateId()];
    if (tmpl)
    {
        auto& spriteTmpl = _gameTemplates.spriteLibrary().spriteByName(tmpl.spriteName());
        WorldObject* body = _world->createObject(req.position(), req.direction(), spriteTmpl.aabb());
        if (!body)
        {
            OVENGINE_LOG_WARN("Simulation.createEntity - failed to create body");
            resp.setResponseCode(CommandResponse::kFailure);
        }
        else
        {
            EntityId newEntityId = (_nextObjectId+1);
            if (!newEntityId)
                newEntityId = 1;
        
            auto entityIt = _entityMap.emplace(std::piecewise_construct,
                                std::forward_as_tuple(newEntityId),
                                std::forward_as_tuple(newEntityId, tmpl)).first;
        
            if (entityIt != _entityMap.end())
            {
                _nextObjectId = newEntityId;

                entityIt->second.attachBody(body);

                resp.setEntityId(newEntityId);
                
                CreateEntityEvent evt;
                evt.setEntityId(newEntityId);
                activeEventQueue().push(SimEvent::kCreateEntity, evt);
            }
            else
            {
                OVENGINE_LOG_WARN("Simulation.createEntity - failed to create body");
                _world->destroyObject(body);
                resp.setResponseCode(CommandResponse::kFailure);
            }
        }
    }
    else
    {
        OVENGINE_LOG_WARN("Simulation.createEntity - invalid template %s specified",
                          req.templateId().c_str());
        resp.setResponseCode(CommandResponse::kInvalidParameter);
    }
    respCb(resp);
}


} /* namespace overview */ } /* namespace cinek */
