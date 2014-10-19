//
//  Entity.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Game/Entity.hpp"

namespace cinek {
    namespace overview {

Entity::Entity(EntityId id, const EntityTemplate& entityTempl) :
    _template(entityTempl),
    _id(id)
{

}
    } /* namespace overview */
} /* namespace cinek */
