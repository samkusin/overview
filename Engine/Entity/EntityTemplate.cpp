//
//  EntityTemplate.cpp
//  Overview
//
//  Created by Samir Sinha on 10/6/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Engine/Entity/EntityTemplate.hpp"

namespace cinek { namespace overview {

    EntityTemplate::EntityTemplate
    (
        const std::string& name,
        const std::string& spriteName,
        const std::string& controllerName
    ) :
        _name(name),
        _spriteName(spriteName),
        _controllerName(controllerName)
    {
    }

    EntityTemplate::EntityTemplate(EntityTemplate&& other) :
        _name(std::move(other._name)),
        _spriteName(std::move(other._spriteName)),
        _controllerName(std::move(other._controllerName))
    {
    }

    EntityTemplate& EntityTemplate::operator=(EntityTemplate&& other)
    {
        _name = std::move(other._name);
        _spriteName = std::move(other._spriteName);
        _controllerName = std::move(other._controllerName);
        return *this;
    }


}   /* ovengine */ }   /* cinekine */
