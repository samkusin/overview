//
//  CreateEntityRequest.hpp
//  Overview
//
//  Created by Samir Sinha on 11/23/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_CreateEntityRequest_hpp
#define Overview_Game_CreateEntityRequest_hpp

#include "Game/SimulationTypes.hpp"
#include "Core/SDO/SDO.hpp"

#include <string>

namespace cinek { namespace overview {

class CreateEntityRequest : public SDO
{
public:
    CINEK_SDO_CLASS(CreateEntityRequest)
    
public:
    CreateEntityRequest() = default;
    CreateEntityRequest(const std::string& templateId,
                        const Point& position,
                        const Point& direction);

    const std::string& templateId() const { return _templateId; }
    const Point& position() const { return _position; }
    const Point& direction() const { return _direction; }
    
    
private:
    std::string _templateId;
    Point _position;
    Point _direction;
};

inline CreateEntityRequest::CreateEntityRequest(
    const std::string& templateId,
    const Point& position,
    const Point& direction
) :
    _templateId(templateId),
    _position(position),
    _direction(direction)
{
}

} /* namespace overview */ } /* namespace cinek */

#endif
