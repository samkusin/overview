//
//  Entity.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Game/Entity.hpp"
#include "Game/EntityTemplate.hpp"

namespace cinek {
    namespace overview {

Entity::Entity(EntityId id, const EntityTemplate& entityTempl) :
    _template(entityTempl),
    _id(id),
    _updateFlags(0),
    _body(nullptr)
{

}
        
        
void Entity::attachBody(WorldObject* body)
{
    _body = body;
}

WorldObject* Entity::detachBody()
{
    WorldObject* obj = _body;
    _body = nullptr;
    return obj;
}
        
    } /* namespace overview */
} /* namespace cinek */
