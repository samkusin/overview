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
#include "Engine/Render/RenderTypes.hpp"
#include "Engine/MessageTypes.hpp"

#include <cinek/allocator.hpp>

#include <functional>


namespace cinek { namespace overview {

Entity createEntity(
    Entity::context_type context,
    EntityStore& store,
    RenderResources& renderResources,
    const cinek::JsonValue& definitions,
    const char* name,
    const CustomComponentCreateFn& customCompFn = CustomComponentCreateFn()
);

void destroyEntityComponent(
    Entity entity,
    component::EntityDataTable& dataTable,
    EntityStore& store,
    RenderResources& resources,
    const CustomComponentDestroyFn& customCompFn = CustomComponentDestroyFn()
);

    
} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Entity_Factory_hpp) */
