///
/// @file
/// Contains the definition for the tile-based Block class
///
/// @author     Samir Sinha
/// @date       07/15/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Builder_Block_hpp
#define Overview_Builder_Block_hpp

#include "Engine/Builder/BuilderTypes.hpp"
#include "Engine/Model/Tile.hpp"
#include "Engine/Grid.hpp"

#include "cinek/allocator.hpp"
#include "cinek/string.hpp"

#include <array>

namespace cinekine { namespace ovengine {

/// @class  Block
/// @ingroup Builder
/// @brief  A Block is a collection of "mini" tilemaps referenced by class, and
///         typically used by Builder based applications
///
class Block
{
public:
    enum
    {
        kGrid_MaxSize = 4               ///< Maximum grid dimension
    };

    enum Layer
    {
        kLayer_Overlay,
        kLayer_Floor
    };

    typedef Grid<TileId> Grid;
    typedef int32_t      GridDimension;

    /// Default Constructor
    ///
    Block();
    /// @return Whether the Block is valid (granularity > 0)
    ///
    operator bool() const;
    /// Constructor
    /// @param name        The name of the block
    /// @param granularity The tile dimension (tiles width and height)
    /// @param border      Grid border width used for drawing edges
    /// @param layer       The Grid layer target for painting this Block
    /// @param allocator   The memory Allocator
    ///
    Block(const char* name, int granularity, int border,
          Layer layer,
          BuilderPaintStyle paintStyle,
          const Allocator& allocator=Allocator());
    /// Enables a Grid Class.  If the Grid has not already been enabled, this
    /// method allocates a new Grid mapped to the specified Class.
    /// @param type The Grid Class
    ///
    void enableGrid(GridDimension xGrid, GridDimension yGrid);
    /// Checks whether this Block has enabled the specified Grid Class.
    /// @param  classId The Grid Class
    /// @return True if the class was enabled
    ///
    bool hasGrid(GridDimension xGrid, GridDimension yGrid) const;
    /// Returns a TileGrid for the given class ID
    /// @param  classId The TileGrid class
    /// @return A pointer to the TileGrid or nullptr if not found
    ///
    Grid& grid(GridDimension xGrid, GridDimension yGrid);
    /// Returns a const TileGrid for the given class ID
    /// @param  classId The TileGrid class
    /// @return A pointer to the TileGrid or nullptr if not found
    ///
    const Grid& grid(GridDimension xGrid, GridDimension yGrid) const;
    /// @return The number of tiles per grid unit
    ///
    int granularity() const;
    /// @return The border width used for drawing tiles along the edge of a grid
    ///
    int border() const;
    /// @return The Block's name
    ///
    const string& name() const { return _name; }
    /// @return The paint style used by the Builder toolbox
    ///
    BuilderPaintStyle paintStyle() const { return _paintStyle; }
    /// @return The layer type used for painting to the Stage
    ///
    Layer layerType() const { return _type; }
    /// Returns the largest Grid available given dimensions
    /// @param  xGrid The X dimension in block units
    /// @param  yGrid the Y dimension in block units
    /// @return The largest grid available.  If no grid matches, returns a zero
    ///         sized grid
    ///
    const Grid& findLargestGridByDimensions(GridDimension xGrid,
                                            GridDimension yGrid) const;

private:
    Grid createGrid(int xDim, int yDim);

private:
    Allocator _allocator;
    Layer _type;
    BuilderPaintStyle _paintStyle;
    string _name;
    Grid _nullGrid;
    Grid _grids[kGrid_MaxSize][kGrid_MaxSize];
    int _granularity;
    int _border;
};

////////////////////////////////////////////////////////////////////////////////

inline Block::operator bool() const
{
    return _granularity > 0;
}

inline auto Block::grid(GridDimension xGrid, GridDimension yGrid) -> Grid&
{
    if (yGrid <= 0 || yGrid > kGrid_MaxSize || xGrid <=0 || xGrid > kGrid_MaxSize)
        return _nullGrid;
    return _grids[yGrid-1][xGrid-1];
}

inline auto Block::grid(GridDimension xGrid, GridDimension yGrid) const -> const Grid&
{
    if (yGrid <= 0 || yGrid > kGrid_MaxSize || xGrid <=0 || xGrid > kGrid_MaxSize)
        return _nullGrid;
    return _grids[yGrid-1][xGrid-1];
}

inline int Block::granularity() const
{
    return _granularity;
}
    
inline int Block::border() const
{
    return _border;
}

inline bool Block::hasGrid(GridDimension xGrid, GridDimension yGrid) const
{
    if (yGrid <= 0 || yGrid > kGrid_MaxSize || xGrid <=0 || xGrid > kGrid_MaxSize)
        return _nullGrid;
    return _grids[yGrid-1][xGrid-1];
}

} /* namespace ovengine */ } /* namespace cinekine */

#endif
