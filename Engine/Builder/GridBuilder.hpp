///
/// @file
/// The GridBuilder utility class
///
/// @author     Samir Sinha
/// @date       07/16/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Builder_GridBuilder_Hpp
#define Overview_Builder_GridBuilder_Hpp

#include "Engine/Builder/BuilderTypes.hpp"
#include "Engine/Builder/BlockLibrary.hpp"

namespace cinekine {
    namespace ovengine {
        class TileLibrary;
        class BlockCollection;
    }
}

namespace cinekine { namespace ovengine {

    /// @defgroup Builder The Map Grid Builder
    /// The Builder is a toolbox used by map generators to procedurally create
    /// tile grids onto Stages.
    ///

    /// @class GridBuilder
    /// @ingroup Builder
    /// @brief Utilities for building grid-based maps
    ///
    /// Applications use GridBuilder to procedurally generate maps using the
    /// supplied Tile and Block libraries.
    ///
    class GridBuilder
    {
    public:
        GridBuilder(TileGrid& gridMap, int32_t tileCoordRatio);

        glm::ivec2 dimensions() const;

        void clear();

        void fillBox(const Block& block,
                     TileSlot tileCollectionSlot,
                     const glm::ivec2& mapPoint, const glm::ivec2& mapDims);

        enum BlockSideType
        {
            kBlockSide_Left,
            kBlockSide_Right,
            kBlockSide_Top = kBlockSide_Left,
            kBlockSide_Bottom = kBlockSide_Right
        };

        enum DrawDirection
        {
            kDrawDirection_Horizontal,
            kDrawDirection_Vertical
        };

        /// Draws a line segment given a position, fixed direction and length
        /// using a source Block grid, and which side of that grid to use.
        ///
        /// Given the nature of mapping Block grids to the target Map grid, the
        /// method limits drawing tile 'lines' horizontally or vertically.  The
        /// Block 'side' determines what area of the source Block grid is
        /// plotted to the builder's target grid.
        ///
        /// @param  block           The source Block used for drawing
        /// @param  blockSide       The Block area used as the tile source
        /// @param  tileSlot        The Tile Collection source for the Block's
        ///                         tiles
        /// @param  drawDirection   The line's direction, using mapPoint as the
        ///                         origin
        /// @param  mapPoint        The line's origin
        /// @param  mapLineUnits    Also the line's length, allowing for
        ///                         negative values.  For example, a horizontal
        ///                         line with a (0,0) origin, and offset value
        ///                         of -5, this method plots a line from (-5,0)
        ///                         to (0,0).  A map unit is the block's
        ///                         granularity value
        ///
        void drawLine(const Block& block, BlockSideType blockSide,
                      TileSlot tileSlot,
                      DrawDirection drawDirection,
                      const glm::ivec2& mapPoint, int mapLineOffset);


    private:
        TileGrid* _grid;
        int _tileCoordRatio;
    };

} /* namespace overview */ } /* namespace cinekine */

#endif
