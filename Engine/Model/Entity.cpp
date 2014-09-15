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

Entity::Entity() :
    _viewInstance()
{

}

Entity::Entity(const ViewModelInstance& modelInst) :
    _viewInstance(modelInst)
{

}

void Entity::setViewModelInstance(const ViewModelInstance& modelInst)
{
    _viewInstance = modelInst;
}

    } /* namespace ovengine */
} /* namespace cinekine */
