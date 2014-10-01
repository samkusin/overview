//
//  Entity.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/Entity.hpp"

namespace cinekine {
    namespace ovengine {

Entity::Entity(EntityId id, const Point& pos) :
    _id(id),
    _position(pos)
{

}


    } /* namespace ovengine */
} /* namespace cinekine */
