//
//  PathEntityRequest.hpp
//  Overview
//
//  Created by Samir Sinha on 02/21/14.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Game_PathEntityRequest_hpp
#define Overview_Game_PathEntityRequest_hpp

#include "Shared/GameTypes.hpp"
#include "Core/SDO/SDO.hpp"

namespace cinek { namespace overview {

class PathEntityRequest : public SDO
{
public:
    CINEK_SDO_CLASS(PathEntityRequest)
    
public:
    PathEntityRequest() = default;
    
    void setEntityId(EntityId entityId) { _entityId = entityId; }
    EntityId entityId() const { return _entityId; }
    
    const Point& destination() const { return _destPos; }
    void setDestination(const Point& pos) { _destPos = pos; }
    
private:
    EntityId _entityId;
    Point _destPos;
};


} /* namespace overview */ } /* namespace cinek */

#endif
