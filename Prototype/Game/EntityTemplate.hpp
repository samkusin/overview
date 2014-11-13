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

#include "cinek/string.hpp"

namespace cinek {
    namespace overview {

    class EntityTemplate
    {
        CK_CLASS_NON_COPYABLE(EntityTemplate);

    public:
        EntityTemplate() = default;
        EntityTemplate(const std::string& name, const std::string& spriteName);
        EntityTemplate(EntityTemplate&& other);
        EntityTemplate& operator=(EntityTemplate&& other);
        
        operator bool() const { return !_name.empty(); }

        const std::string& name() const { return _name; }
        const std::string& spriteName() const { return _spriteName; }

    private:
        std::string _name;
        std::string _spriteName;
    };

    }   // ovengine
}   // cinekine


#endif
