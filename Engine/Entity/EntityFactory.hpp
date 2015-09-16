//
//  EntityFactory.hpp
//  Overview
//
//  Created by Samir Sinha on 5/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Factory_hpp
#define Overview_Entity_Factory_hpp

#include "Engine/Entity/EntityTypes.hpp"
#include "Engine/MessageTypes.hpp"

#include <cinek/allocator.hpp>
#include <cinek/json/jsontypes.hpp>

namespace cinek { namespace overview {


using CustomComponentCreateFn =
    std::function<void(Entity entity,
                       const cinek::JsonValue& definitions,
                       const char* componentName,
                       const cinek::JsonValue& data)>;
    
using CustomComponentDestroyFn =
    std::function<void(Entity entity, ComponentId componentId)>;


Entity createEntity(
    EntityContextType context,
    EntityStore& store,
    const cinek::JsonValue& definitions,
    const char* name,
    const CustomComponentCreateFn& customCompFn = CustomComponentCreateFn()
);

void destroyEntityComponent(
    Entity entity,
    EntityDataTable& dataTable,
    EntityStore& store,
    const CustomComponentDestroyFn& customCompFn = CustomComponentDestroyFn()
);
    
} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Entity_Factory_hpp) */
