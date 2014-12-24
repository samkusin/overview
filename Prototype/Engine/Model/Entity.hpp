//
//  Entity.hpp
//  Overview
//
//  Created by Samir Sinha on 9/23/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_ModelEntity_hpp
#define Overview_Game_ModelEntity_hpp

#include "Game/SimulationTypes.hpp"
#include "Game/WorldObject.hpp"

//  Engine forward declarations
namespace cinek {
    namespace overview {
        class EntityTemplate;
        class WorldObject;
    }
}

namespace cinek {
    namespace overview {

    class Entity
    {
        CK_CLASS_NON_COPYABLE(Entity);
        
    public:
        /// Constructs an Entity with its components
        ///
        Entity(EntityId id, const EntityTemplate& entityTempl);

        /// @return The Entity's identifier
        EntityId id() const;

        /// @ return The Entity's template object
        const EntityTemplate& sourceTemplate() const;

        /// @return The accumulation of changes/results to the Entity this
        ///         frame.  These flags are defined in EntityTypes.hpp
        ///
        uint32_t resultFlags() const;
        
        /// Resets result flags (typically on simulation update start.)
        ///
        void resetResultFlags();
        
        /// Sets the change/result state for the Entity
        ///
        /// @param  resultFlag  One or more flags to set (or clear)
        /// @param  state       (Optional) set or clear, defaulting to set.
        void setResult(uint32_t resultFlag, bool state=true);
        
        /// @return Returns the body (physical) object
        ///
        const WorldObject* body() const { return _body; }
        
        /// @return Returns the body (physical) object
        ///
        WorldObject* body() { return _body; }

    public:
        //////////////////////////////////////////////////////////////////
        
        /// Associates a collision body with the Entity.  The Entity
        /// does not own the body, and the caller should invoke detachBody
        /// prior to destroying the associated body object (or otherwise
        /// destroy the Entity prior to destroying the body object.)
        ///
        /// @param body The collision engine-specific body
        ///
        void attachBody(WorldObject* body);

        /// Detaches a collision body from the entity
        ///
        /// @return The collision engine-specific body
        ///
        WorldObject* detachBody();
        
        bool operator<(const Entity& rhs) const;

    private:
        const EntityTemplate& _template;
        EntityId _id;
        
        uint32_t _updateFlags;
        
        WorldObject* _body;
    };

    }   /* namespace overview */
}   /* namespace cinek */

#include "Engine/Model/Entity.inl"

#endif