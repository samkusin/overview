//
//  Simulation.hpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Simulation_hpp
#define Overview_Game_Simulation_hpp

#include "Game/SimulationTypes.hpp"
#include "Game/Entity.hpp"
#include "Game/SimulationContext.hpp"
#include "Core/MessageDispatcher.hpp"

#include "cinek/objectpool.hpp"
#include "cinek/memorystack.hpp"
#include "cinek/taskscheduler.hpp"
#include "cinek/allocator.hpp"
#include "cinek/map.hpp"
#include "cinek/vector.hpp"

#include <functional>
#include <array>


namespace cinek {
    
    namespace overview {
        class GameTemplates;
        class World;
        
        class CreateEntityRequest;
        class CreateEntityResponse;
        class MoveEntityRequest;
        
    } /* namespace overview */

} /* namespace cinek */

namespace cinek { namespace overview {
    
    struct SimulationParams
    {
        uint32_t entityLimit;
        uint32_t debugMsgLimit;
        Allocator allocator;
    };
    
    class Simulation
    {
    public:
        Simulation(const overview::GameTemplates& gameTemplates,
                   const SimulationParams& params);
        ~Simulation();

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
        /// @param inMsgQueue  Input message queue
        /// @param outMsgQueue Output message queue
        /// @param timeMs      The current simulation time.
        ///
        void update(MessageQueue& inMsgQueue,
                    MessageQueue& outMsgQueue,
                    uint32_t timeMs);
        
        /// Obtains debug messages from the Simulation for display
        ///
        /// @param messages A vector that receives SimDebugMessage objects
        ///                 from the Simulation.
        /// @param timeMs   The current simulation time.
        ///
        void syncDebugMessages(vector<SimDebugMessage>& messages);

    private:
        Allocator _allocator;
        Allocator _entityAllocator;
        const overview::GameTemplates& _gameTemplates;
        
        uint32_t _systemTimeMs;
        
        vector<SimDebugMessage> _debugMessages;

        std::array<MessageQueue, 2> _eventQueues;
        uint32_t _activeEventQueue;
        MessageDispatcher _commandDispatcher;
        MessageDispatcher _eventDispatcher;
        TaskScheduler _scheduler;
        
        SimObjectId _nextObjectId;
        unordered_map<EntityId, overview::Entity> _entityMap;
        unique_ptr<overview::World> _world;
        
    private:
        MessageQueue& activeEventQueue() { return _eventQueues[_activeEventQueue]; }
        
        template<typename T> using ResponseCallback = std::function<void(T&)>;

        void createEntityCommand(
            const CreateEntityRequest& req,
            const ResponseCallback<CreateEntityResponse>& respCb);
        void moveEntityCommand(
            const MoveEntityRequest& req,
            const ResponseCallback<CommandResponse>& respCb);
    };

} /* namespace overview */ } /* namespace cinek */

#endif /* defined(__Overview__Simulation__) */
