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
#include "Game/EntitySimResult.hpp"


namespace cinek {
    using namespace overview;
}

namespace cinek { namespace overview {

Simulation::Simulation(
    const overview::GameTemplates& gameTemplates,
    const SimulationParams& params
) :
    _allocator(params.allocator),
    _entityAllocator(_allocator),
    _gameTemplates(gameTemplates),
    _nextObjectId(0),
    _entityMap(_entityAllocator),
    _resultMemory(64*1024, _allocator),
    _results(_allocator)
{
    _entityMap.reserve(params.entityLimit);
    _results.reserve(1024);
    
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
}

Simulation::~Simulation()
{
    for (auto& entity : _entityMap)
    {
        auto body = entity.second.detachBody();
        _world->destroyObject(body);
    }
}

EntityId Simulation::createEntity(const std::string& templateId)
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
    entity.setResult(kEntityResultFlag_Activated);
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
    entity.setResult(kEntityResultFlag_Destroyed);
}
    
void Simulation::deactivateEntity(Entity& entity)
{
    auto body = entity.detachBody();
    if (!body)
        return;
    _world->destroyObject(body);
    entity.setResult(kEntityResultFlag_Deactivated);
}

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

void Simulation::update(uint32_t timeMs)
{
    //  update simulation subsystems
    //
    _world->update(timeMs);
    
    //  report entity results
    //
    for (auto entityIt = _entityMap.begin(); entityIt != _entityMap.end(); )
    {
        auto& entity = entityIt->second;
        auto flags = entity.resultFlags();
        
        if (flags)
        {
            EntitySimResult* result =
                _resultMemory.newItem<EntitySimResult>(entityIt->first, flags, timeMs);
            _results.push_back(result);
        }
        
        if (flags & kEntityResultFlag_Destroyed)
        {
            entityIt = _entityMap.erase(entityIt);
        }
        else
        {
            ++entityIt;
        }
    }
}

void Simulation::syncResults(const std::function<void(const EventBase*)>& fn)
{
    for (auto& result : _results)
    {
        fn(result);
    }
    
    //  clear results
    //
    for (auto& result : _results)
    {
        result->~EventBase();
    }
    _results.clear();
    _resultMemory.reset();
    
    for (auto entityIt = _entityMap.begin(); entityIt != _entityMap.end(); ++entityIt)
    {
        auto& entity = entityIt->second;
        entity.resetResultFlags();
    }
}

} /* namespace overview */ } /* namespace cinek */
