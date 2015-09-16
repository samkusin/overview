//
//  StellarSystemTree.hpp
//  Overview
//
//  Created by Samir Sinha on 6/29/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Sim_StellarSystemTree_hpp
#define Overview_Sim_StellarSystemTree_hpp

#include "GameTypes.hpp"
#include "Custom/Comp/StellarSystem.hpp"
#include "Engine/Entity/Comp/Transform.hpp"
#include "Engine/BVH/AABBTree.hpp"

#include <cinek/entity/entitydatatable.hpp>

namespace cinek { namespace ovproto {

class StellarSystemUtility
{
public:
    StellarSystemUtility() = default;
    StellarSystemUtility(EntityStore& store);
    StellarSystemUtility(const StellarSystemUtility& other);
    
    void setObjectData(Entity objIdx, intptr_t data);
    intptr_t getObjectData(Entity objIdx) const;
    ckm::scalar objectRadius(Entity objIdx) const;
    ckm::vec3 position(Entity objIdx) const;

private:
    EntityStore* _entityStore = nullptr;
    component::Table<overview::TransformComponent> _transformTable;
    component::Table<StellarSystemComponent> _stellarSystemTable;
};

using StellarSystemTree = overview::AABBTree<Entity, StellarSystemUtility>;


} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Sim_StellarSystemTree_hpp) */
