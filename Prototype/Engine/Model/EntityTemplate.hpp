//
//  EntityTemplate.hpp
//  Overview
//
//  Created by Samir Sinha on 10/6/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_EntityTemplate_hpp
#define Overview_EntityTemplate_hpp

#include "Shared/GameTypes.hpp"

#include "cinek/string.hpp"

namespace cinek {
    namespace overview {

    class EntityTemplate
    {
        CK_CLASS_NON_COPYABLE(EntityTemplate);

    public:
        EntityTemplate() = default;
        EntityTemplate(const std::string& name, const std::string& spriteName,
                       const std::string& controllerName);
        EntityTemplate(EntityTemplate&& other);
        EntityTemplate& operator=(EntityTemplate&& other);
        
        operator bool() const { return !_name.empty(); }

        const std::string& name() const { return _name; }
        const std::string& spriteName() const { return _spriteName; }
        const std::string& controllerName() const { return _controllerName; }

    private:
        std::string _name;
        std::string _spriteName;
        std::string _controllerName;
    };

    }   // ovengine
}   // cinekine


#endif
