//
//  StellarSystemUtility.cpp
//  Overview
//
//  Created by Samir Sinha on 6/29/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "StellarSystemUtility.hpp"
#include "Engine/Entity/Comp/Transform.hpp"
//#include "Engine/Entity/TransformEntity.hpp"

#include <cinek/entity/entitystore.hpp>
#include <cinek/entity/entitydatatable.hpp>

namespace cinek { namespace ovproto {

StellarSystemUtility::StellarSystemUtility(EntityStore& store) :
    _entityStore(&store),
    _transformTable(_entityStore->table<overview::TransformComponent>()),
    _stellarSystemTable(_entityStore->table<StellarSystemComponent>())
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
    auto data = _stellarSystemTable.dataForEntity(objIdx);
    if (!data)
        return;
    data->_indexToTreeNode = (int)objData;
}

intptr_t StellarSystemUtility::getObjectData(Entity objIdx) const
{
    auto data = _stellarSystemTable.dataForEntity(objIdx);
    return data != nullptr ? data->_indexToTreeNode : -1;
}

ckm::scalar StellarSystemUtility::objectRadius(Entity objIdx) const
{
    auto data = _stellarSystemTable.dataForEntity(objIdx);
    return data != nullptr ? data->_radius : 0;
}

ckm::vec3 StellarSystemUtility::position(Entity objIdx) const
{
    auto transform = _transformTable.dataForEntity(objIdx);
    if (!transform)
        return ckm::vec3(0);

    //  systems don't have parents - so the local should work
    return transform->localPosition();
}


} /* namespace ovproto */ } /* namespace cinek */