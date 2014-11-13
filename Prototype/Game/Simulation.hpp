//
//  Simulation.hpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Simulation_hpp
#define Overview_Game_Simulation_hpp

#include "Game/WorldTypes.hpp"
#include "Game/Entity.hpp"

#include "cinek/objectpool.hpp"
#include "cinek/memorystack.hpp"
#include "cinek/allocator.hpp"
#include "cinek/map.hpp"
#include "cinek/vector.hpp"

#include <functional>


namespace cinek {

    class EventBase;
    
    namespace overview {
        class GameTemplates;
        class World;
    } /* namespace overview */
    
} /* namespace cinek */

namespace cinek { namespace overview {

    struct SimulationParams
    {
        uint32_t entityLimit;
        Allocator allocator;
    };
    
    class Simulation
    {
    public:
        Simulation(const overview::GameTemplates& gameTemplates,
                   const SimulationParams& params);
        ~Simulation();

        /// Creates an Entity from a named template.  The response contains
        /// the created entity by template and its unique (per simulation) ID
        /// value.
        ///
        /// @param  templateId  The entity definition used to create the
        ///                     instance.
        /// @return The ID of the created entity.
        ///
        EntityId createEntity(const std::string& templateId);
        
        /// Positions an entity onto the simulation world.  This is the most
        /// direct way to place an entity into the world, bypassing spawn
        /// points and other indirect methods.  Upon activation, all active
        /// jobs/directives on the entity are cleared - the Entity's state
        /// is reset.
        ///
        /// @param  id     The entity ID
        /// @param  pos    The world position
        /// @param  front  The front direction (facing)
        ///
        void activateEntity(EntityId id, const Point& pos,
                            const Point& front);
        
        /// Removes the Entity from the World.  The Entity is not destroyed.
        /// Callers can use this method to maintain an Entity's local state
        /// without destroying it completely.
        ///
        /// @param  id      The Entity ID
        ///
        void deactivateEntity(EntityId id);
        
        /// Destroys the Entity, removing it from the world if it's active.
        ///
        /// @param  id      The Entity ID
        ///
        void destroyEntity(EntityId id);
        
        /// Obtain a const pointer to the entity.
        ///
        /// @param  entityId The Entity ID
        /// @return A valid read-only interface to the entity, or nullptr if it
        ///         does not exist
        ///
        const Entity* entity(EntityId entityId) const;
        
        /// Obtain a pointer to the entity.
        ///
        /// @param  entityId The Entity ID
        /// @return A valid pointer to the entity, or nullptr if it does not
        ///         exist
        ///
        Entity* entity(EntityId entityId);
        
        /// Steps the simulation.
        ///
        /// @param timeMs The current simulation time.
        ///
        void update(uint32_t timeMs);
        
        /// Handle simulation results
        ///
        /// @param  fn  Callback issued per result
        ///
        void syncResults(const std::function<void(const EventBase*)>& fn);
        
    private:
        void deactivateEntity(Entity& entity);
        
    private:
        Allocator _allocator;
        Allocator _entityAllocator;
        const overview::GameTemplates& _gameTemplates;
        
        WorldObjectId _nextObjectId;
        unordered_map<EntityId, overview::Entity> _entityMap;
        unique_ptr<overview::World> _world;
        
        //  results
        MemoryStack _resultMemory;
        vector<EventBase*> _results;
    };

} /* namespace overview */ } /* namespace cinek */

#endif /* defined(__Overview__Simulation__) */
