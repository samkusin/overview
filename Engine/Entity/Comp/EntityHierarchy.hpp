//
//  Comp/EntityHierarchy.hpp
//  Overview
//
//  Created by Samir Sinha on 6/22/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Component_EntityHierarchy_hpp
#define Overview_Entity_Component_EntityHierarchy_hpp

#include "Engine/Entity/EntityTypes.hpp"

namespace cinek { namespace overview {

namespace component
{
    struct EntityHierarchy
    {
        COMPONENT_DEFINITION(EntityHierarchy);
        
        Entity parent;
        Entity firstChild;
        Entity nextSibling;
    };
}

} /* namespace overview */ } /* namespace cinek */


#endif
