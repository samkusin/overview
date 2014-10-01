//
//  Entity.hpp
//  Overview
//
//  Created by Samir Sinha on 9/23/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_ModelEntity_hpp
#define Overview_Game_ModelEntity_hpp

#include "Game/Model/EntityTypes.hpp"

namespace cinekine {
    namespace prototype {

    class Entity
    {
    public:
        /// Constructs an Entity with an initial position
        ///
        Entity(EntityId id, const glm::vec3& pos);

        /// @return The Entity's identifier
        EntityId id() const;

        /// @return The current position of the Entity
        const glm::vec3& position() const;

    private:
        const EntityId _id;
        glm::vec3 _position;
        
        
    };

    }   /* namespace prototype */
}   /* namespace cinekine */

#include "Game/Model/Entity.inl"

#endif