//
//  Entity.hpp
//  Overview
//
//  Created by Samir Sinha on 9/23/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_ModelEntity_hpp
#define Overview_Game_ModelEntity_hpp

#include "Game/EntityTypes.hpp"

//  Bullet forward decls
class btCollisionObject;

//  Engine forward declarations
namespace cinek {
    namespace overview {
        class EntityTemplate;
    }
}

namespace cinek {
    namespace overview {

    class Entity
    {
    public:
        /// Constructs an Entity with its components
        ///
        Entity(EntityId id, const EntityTemplate& entityTempl);

        /// @return The Entity's identifier
        EntityId id() const;

        /// @ return The Entity's template object
        const EntityTemplate& sourceTemplate() const;

        /// @return The current position of the Entity
        glm::vec3 position() const;

        /// @return The Front direction vector
        glm::vec3 frontDirection() const;

        /// Associates a collision body with the Entity.  The Entity
        /// does not own the body, and the caller should invoke detachBody
        /// prior to destroying the associated body object (or otherwise
        /// destroy the Entity prior to destroying the body object.)
        ///
        /// @param body The collision engine-specific body
        ///
        void attachBody(btCollisionObject* body);

        /// Detaches a collision body from the entity
        ///
        /// @return The collision engine-specific body
        ///
        btCollisionObject* detachBody();

    private:
        const EntityTemplate& _template;
        const EntityId _id;
        btCollisionObject* _bodyObject;
    };

    }   /* namespace overview */
}   /* namespace cinek */

#include "Game/Entity.inl"

#endif