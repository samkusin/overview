//
//  StellarSystemUtility.cpp
//  Overview
//
//  Created by Samir Sinha on 6/29/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "StellarSystemUtility.hpp"
#include "Engine/Entity/EntityStore.hpp"
#include "Engine/Entity/EntityDataTable.hpp"
#include "Engine/Entity/Comp/Transform.hpp"

namespace cinek { namespace ovproto {

StellarSystemUtility::StellarSystemUtility(overview::EntityStore& store) :
    _entityStore(&store),
    _transformTable(_entityStore->table<overview::component::Transform>()),
    _stellarSystemTable(_entityStore->table<component::StellarSystem>())
{
}

StellarSystemUtility::StellarSystemUtility(const StellarSystemUtility& other) :
    _entityStore(other._entityStore),
    _transformTable(other._transformTable),
    _stellarSystemTable(other._stellarSystemTable)
{
}


void StellarSystemUtility::setObjectData
(
    Entity objIdx,
    intptr_t objData
)
{
    auto data = _stellarSystemTable.rowForEntity(objIdx);
    if (!data)
        return;
    data->indexToTreeNode = objData;
}

intptr_t StellarSystemUtility::getObjectData(Entity objIdx) const
{
    auto data = _stellarSystemTable.rowForEntity(objIdx);
    return data != nullptr ? data->indexToTreeNode : -1;
}

ckm::scalar StellarSystemUtility::objectRadius(Entity objIdx) const
{
    auto data = _stellarSystemTable.rowForEntity(objIdx);
    return data != nullptr ? data->radius : 0;
}

ckm::vec3 StellarSystemUtility::position(Entity objIdx) const
{
    ckm::vec3 pos;
    
    auto transform = _transformTable.rowForEntity(objIdx);
    if (!transform)
        return ckm::vec3 { 0.0 };
    
    auto& translate = transform->worldSRT()[3];
    pos.x = translate.x;
    pos.y = translate.y;
    pos.z = translate.z;
    return pos;
}


} /* namespace ovproto */ } /* namespace cinek */