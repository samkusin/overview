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


namespace cinek { namespace overview {


Entity createEntity(
    Entity::context_type context,
    EntityStore& store,
    const cinek::JsonValue& definitions,
    const char* name,
    const CustomComponentCreateFn& customCompFn = CustomComponentCreateFn()
);

void destroyEntityComponent(
    Entity entity,
    component::EntityDataTable& dataTable,
    EntityStore& store,
    const CustomComponentDestroyFn& customCompFn = CustomComponentDestroyFn()
);
    
} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Entity_Factory_hpp) */
