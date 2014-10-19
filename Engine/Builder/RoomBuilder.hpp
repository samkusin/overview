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

namespace cinek {
    namespace overview {
        class TileGridMap;
        class BlockCollection;
    }
}

namespace cinek {
    namespace overview {
        namespace room_builder {

struct PaintStyle
{
    std::string floorBlockName;
    std::string wallBlockName;
};

void paint(Room room, TileGridMap& grid,
           const PaintStyle& style,
           const BlockCollection& blockCollection,
           TileSlot tileCollectionSlot);

        } /* namespace room_builder */
    } /* namespace overview */
} /* namespace cinek */

#endif
