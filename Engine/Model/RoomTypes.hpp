///
/// @file
/// Data nodes within a Room graph
///
/// @author     Samir Sinha
/// @date       08/17/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_RoomTypes_Hpp
#define Overview_Model_RoomTypes_Hpp

#include "Engine/Model/ModelTypes.hpp"
#include "Engine/Model/AABB.hpp"

namespace cinekine {
    namespace ovengine {

class RoomGraph;

typedef glm::ivec2 RoomVertex;
typedef AABB<RoomVertex> RoomAABB;

enum RoomSide
{
    kRoomSide_North,
    kRoomSide_East,
    kRoomSide_South,
    kRoomSide_West,
    kRoomSide_Count
};

    } /* namespace overview */
} /* namespace cinekine */

#endif
