//
//  EntityMap.hpp
//  Overview
//
//  Created by Samir Sinha on 2/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Game_EntityMap_hpp
#define Overview_Game_EntityMap_hpp

#include "Engine/Model/Entity.hpp"
#include "cinek/map.hpp"

namespace cinek { namespace overview {


//  The EntityMap contains all Entities and offers the Simulation a store
//  for Entity references.

class EntityMap
{
    CK_CLASS_NON_COPYABLE(EntityMap);
    
public:
    EntityMap(uint32_t entityLimit, const Allocator& allocator);
    ~EntityMap();
    
    
    
private:
    Allocator _allocator;
    unordered_map<EntityId, overview::Entity> _entityMap;
};

} /* namespace overview */ } /* namespace cinek */

#endif
