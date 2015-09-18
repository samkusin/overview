//
//  RenderComponentFactory.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Render_CreateComponent_hpp
#define Overview_Render_CreateComponent_hpp

#include "Engine/Render/RenderTypes.hpp"
#include "Engine/Entity/EntityTypes.hpp"

namespace cinek {
    namespace overview {
    
ComponentFactoryResult createRenderableComponent
(
    Entity entity,
    const cinek::JsonValue& definitions,
    const char* componentName,
    const cinek::JsonValue& data,
    EntityStore& store,
    RenderResources& resources
);

ComponentFactoryResult destroyRenderableComponent
(
    EntityDataTable& entityTable,
    ComponentRowIndex compRowIndex,
    RenderResources& renderResources
);

ComponentFactoryResult createCameraComponent
(
    Entity entity,
    const cinek::JsonValue& definitions,
    const char* componentName,
    const cinek::JsonValue& data,
    EntityStore& store
);

ComponentFactoryResult destroyCameraComponent
(
    EntityDataTable& entityTable,
    ComponentRowIndex compRowIndex
);
        
    } /* namespace overview */
} /* namespace cinek */

#endif /* defined(Overview_Render_CreateComponent_hpp) */
