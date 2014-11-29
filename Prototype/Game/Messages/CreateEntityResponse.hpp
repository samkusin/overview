//
//  CreateEntityResponse.hpp
//  Overview
//
//  Created by Samir Sinha on 11/23/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_CreateEntityResponse_hpp
#define Overview_Game_CreateEntityResponse_hpp

#include "Game/Messages/CommandResponse.hpp"
#include "Game/SimulationTypes.hpp"

namespace cinek { namespace overview {

class CreateEntityResponse : public CommandResponse
{
public:
    CINEK_SDO_SUB_CLASS(CreateEntityResponse, CommandResponse)

    CreateEntityResponse() = default;

    void setEntityId(EntityId id) { _entityId = id; }
    EntityId entityId() const { return _entityId; }
    
private:
    EntityId _entityId;
};


} /* namespace overview */ } /* namespace cinek */

#endif
