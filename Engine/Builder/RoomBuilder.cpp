//
//  RoomBuilder.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Builder/RoomBuilder.hpp"
#include "Engine/Builder/GridBuilder.hpp"

namespace cinekine {
    namespace ovengine {
        namespace room_builder {

void paint(Room room, const PaintStyle& style, GridBuilder& grid)
{
    // paint edges based on portal points
    auto roomAABB = room.AABB();

}
        } /* namespace room_builder */
    } /* namespace overview */
} /* namespace cinekine */