//
//  EntityTemplate.cpp
//  Overview
//
//  Created by Samir Sinha on 10/6/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/EntityTemplate.hpp"

namespace cinekine {
    namespace ovengine {
        
    EntityTemplate::EntityTemplate(const std::string& name, const std::string& spriteName) :
        _name(name),
        _spriteName(spriteName)
    {
    }
        
    }   // ovengine
}   // cinekine
