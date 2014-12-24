//
//  EntityTemplate.cpp
//  Overview
//
//  Created by Samir Sinha on 10/6/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Engine/Model/EntityTemplate.hpp"

namespace cinek { namespace overview {

    EntityTemplate::EntityTemplate(const std::string& name, const std::string& spriteName) :
        _name(name),
        _spriteName(spriteName)
    {
    }

    EntityTemplate::EntityTemplate(EntityTemplate&& other) :
        _name(std::move(other._name)),
        _spriteName(std::move(other._spriteName))
    {
    }

    EntityTemplate& EntityTemplate::operator=(EntityTemplate&& other)
    {
        _name = std::move(other._name);
        _spriteName = std::move(other._spriteName);
        return *this;
    }


}   /* ovengine */ }   /* cinekine */
