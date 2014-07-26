///
/// @file
/// Contains the class definition for the TileGridMap
///
/// @author     Samir Sinha
/// @date       07/20/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Model_TileGridMap_hpp
#define Overview_Model_TileGridMap_hpp

#include "Engine/Model/StageTypes.hpp"

namespace cinekine { namespace ovengine {

/// @class  TileGridMap
/// @ingroup TileModel
/// @brief  A container of TileGrid objects defining a single map
///
class TileGridMap
{
public:
    /// Constructor
    /// Creates a TileGridMap with the specified grid dimensions.
    /// The TileGridMap consists of layers.   The overlay layer's grid size is
    /// often not the same as the floor grid.  Put another way, a floor tile may
    /// be 1x1, 2x2, 4x4, etc overlay tiles.
    ///
    /// @param xDimension       The x coordinate dimension in overlay tile
    ///                         coordinates
    /// @param yDimension       The y coordinate dimension in overlay tile
    ///                         coordinates
    /// @param overlayToFloorRatio The number of overlay tiles per floor tile
    /// @param allocator        The memory allocator
    ///
    TileGridMap(uint32_t xDimension, uint32_t yDimension,
                uint32_t overlayToFloorRatio,
                const Allocator& allocator=Allocator());

    /// @return The floor's TileGrid
    ///
    TileGrid& floor() { return _floorGrid; }
    /// @return The floor's constant TileGrid
    ///
    const TileGrid& floor() const { return _floorGrid; }
    /// @return The overlay's TileGrid
    ///
    TileGrid& overlay() { return _overlayGrid; }
    /// @return The overlay's constant TileGrid
    ///
    const TileGrid& overlay() const { return _overlayGrid; }

    uint32_t overlayToFloorRatio() const { return _overlayToFloorRatio; }

private:
    TileGrid _floorGrid;
    TileGrid _overlayGrid;
    uint32_t _overlayToFloorRatio;
};

} /* namespace ovengine */ } /* namespace cinekine */

#endif
