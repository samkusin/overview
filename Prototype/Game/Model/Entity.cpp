//
//  Entity.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Game/Model/Entity.hpp"

namespace cinekine {
    namespace prototype {

Entity::Entity(EntityId id, const glm::vec3& pos) :
    _id(id),
    _position(pos)
{

}

    } /* namespace prototype */
} /* namespace cinekine */
