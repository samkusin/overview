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
#include "Engine/Entity/EntityDataTable.hpp"
#include "Engine/BVH/AABBTree.hpp"

namespace cinek { namespace ovproto {

class StellarSystemUtility
{
public:
    StellarSystemUtility() = default;
    StellarSystemUtility(overview::EntityStore& store);
    StellarSystemUtility(const StellarSystemUtility& other);
    
    void setObjectData(Entity objIdx, intptr_t data);
    intptr_t getObjectData(Entity objIdx) const;
    ckm::scalar objectRadius(Entity objIdx) const;
    ckm::vec3 position(Entity objIdx) const;

private:
    overview::EntityStore* _entityStore = nullptr;
    overview::component::Table<overview::component::Transform> _transformTable;
    overview::component::Table<component::StellarSystem> _stellarSystemTable;
};

using StellarSystemTree = overview::AABBTree<Entity, StellarSystemUtility>;


} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Sim_StellarSystemTree_hpp) */
