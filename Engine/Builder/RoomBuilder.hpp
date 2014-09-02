///
/// @file
/// The RoomBuilder utility
///
/// @author     Samir Sinha
/// @date       08/17/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Builder_RoomBuilder_Hpp
#define Overview_Builder_RoomBuilder_Hpp

#include "Engine/Builder/BuilderTypes.hpp"
#include "Engine/Model/Room.hpp"

#include "cinek/string.hpp"

namespace cinekine {
    namespace ovengine {
        class GridBuilder;
    }
}

namespace cinekine {
    namespace ovengine {
        namespace room_builder {

struct PaintStyle
{
    string floorBlockName;
    string wallBlockName;
    TileSlot tileCollectionSlot;
};

void paint(Room room, const PaintStyle& style, GridBuilder& grid);

        }
    } /* namespace ovengine */
} /* namespace cinekine */

#endif
