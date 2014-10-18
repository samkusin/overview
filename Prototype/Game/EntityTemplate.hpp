//
//  EntityTemplate.hpp
//  Overview
//
//  Created by Samir Sinha on 10/6/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_EntityTemplate_hpp
#define Overview_EntityTemplate_hpp

#include "Game/EntityTypes.hpp"

#include <string>

namespace cinekine {
    namespace ovengine {
        
    class EntityTemplate
    {
    public:
        EntityTemplate() = default;
        
        EntityTemplate(const std::string& name, const std::string& spriteName);
        
        const std::string& name() const { return _name; }
        
    private:
        const std::string _name;
        const std::string _spriteName;
    };
    
    }   // ovengine
}   // cinekine


#endif
