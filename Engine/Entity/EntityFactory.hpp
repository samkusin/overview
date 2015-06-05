//
//  EntityFactory.hpp
//  Overview
//
//  Created by Samir Sinha on 5/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Factory_hpp
#define Overview_Entity_Factory_hpp

#include "Engine/Render/RenderTypes.hpp"
#include "Engine/Entity/EntityObject.hpp"
#include "Engine/MessageTypes.hpp"

#include <cinek/allocator.hpp>

#include <functional>

namespace cinek { namespace overview {

EntityObject* createEntity(
    EntityDatabase& db,
    RenderResources& renderResources,
    const cinek::JsonValue& definitions,
    const char* name,
    MessagePublisher* eventPublisher = nullptr,
    const CustomComponentCreateFn& customCompFn = CustomComponentCreateFn()
);
    
} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Entity_Factory_hpp) */
