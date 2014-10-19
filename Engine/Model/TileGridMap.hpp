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

#include "Engine/Model/TileGrid.hpp"

namespace cinek { namespace overview {

/// @class  TileGridMap
/// @ingroup TileModel
/// @brief  A container of TileGrid objects defining a single map
///
class TileGridMap
{
    CK_CLASS_NON_COPYABLE(TileGridMap);

public:
    /// Constructor
    /// Creates a TileGridMap with the specified grid dimensions.
    /// The TileGridMap consists of layers.   The overlay layer's grid size is
    /// often not the same as the floor grid.  Put another way, a floor tile may
    /// be 1x1, 2x2, 4x4, etc overlay tiles.
    ///
    /// @param xDimension       The x coordinate dimension in floor tile
    ///                         coordinates
    /// @param yDimension       The y coordinate dimension in floor tile
    ///                         coordinates
    /// @param overlayToFloorRatio The number of overlay tiles per floor tile
    /// @param allocator        The memory allocator
    ///
    TileGridMap(uint32_t xDimension, uint32_t yDimension,
                uint32_t overlayToFloorRatio,
                const Allocator& allocator=Allocator());

    /// Default constructor
    ///
    TileGridMap();

    TileGridMap(TileGridMap&& other);
    TileGridMap& operator=(TileGridMap&& other);

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

    glm::ivec2 floorDimensions() const {
        return glm::ivec2(_floorGrid.rowCount(), _floorGrid.columnCount());
    }
    glm::ivec2 overlayDimensions() const {
        return glm::ivec2(_overlayGrid.rowCount(), _overlayGrid.columnCount());
    }

private:
    TileGrid _floorGrid;
    TileGrid _overlayGrid;
    uint32_t _overlayToFloorRatio;
};

} /* namespace overview */ } /* namespace cinek */

#endif
