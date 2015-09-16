//
//  ComponentCreationCallback.hpp
//  Overview
//
//  Created by Samir Sinha on 7/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Comp_Custom_ComponentCreator_hpp
#define Overview_App_Comp_Custom_ComponentCreator_hpp

#include "GameTypes.hpp"
#include "Client/RenderContext.hpp"
#include "Engine/Entity/EntityTypes.hpp"

#include <cinek/json/jsontypes.hpp>

namespace cinek {

    namespace ovproto {
    
        void customComponentCreateCb
        (
            AppContext context,
            RenderContext render,
            Entity entity,
            const cinek::JsonValue& definitions,
            const char* componentName,
            const cinek::JsonValue& data
        );
        
        void customComponentDestroyCb
        (
            AppContext context,
            RenderContext render,
            Entity entity,
            ComponentId componentId
        );
    
    } /* namespace ovproto */
    
} /* namespace cinek */

#endif /* defined(__Overview__ComponentCreationCallback__) */
