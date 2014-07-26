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
    enum Class
    {
        kClass_1x1,         ///< Has a tile width/height equal to granularity
        kClass_2x2,         ///< Has a tile dimension equal to 2 * granularity
        kClass_3x3,         ///< Has a tile dimension equal to 3 * granularity
        kClass_4x4,         ///< Has a tile dimension equal to 4 * granularity
        kClass_Count        ///< Total number of available classes
    };

    enum Layer
    {
        kLayer_Overlay,
        kLayer_Floor
    };

    typedef Grid<TileId> Grid;

    /// Default Constructor
    ///
    Block();
    /// @return Whether the Block is valid (granularity > 0)
    ///
    operator bool() const;
    /// Constructor
    /// @param name        The name of the block
    /// @param granularity The tile dimension (tiles width and height)
    /// @param layer       The Grid layer target for painting this Block
    /// @param allocator   The memory Allocator
    ///
    Block(const char* name, int granularity,
          Layer layer,
          BuilderPaintStyle paintStyle,
          const Allocator& allocator=Allocator());
    /// Enables a Grid Class.  If the Grid has not already been enabled, this
    /// method allocates a new Grid mapped to the specified Class.
    /// @param type The Grid Class
    ///
    void enableGrid(Class type);
    /// Checks whether this Block has enabled the specified Grid Class.
    /// @param  classId The Grid Class
    /// @return True if the class was enabled
    ///
    bool hasGrid(Class classId) const;
    /// Returns a TileGrid for the given class ID
    /// @param  classId The TileGrid class
    /// @return A pointer to the TileGrid or nullptr if not found
    ///
    Grid& grid(Class classId);
    /// Returns a const TileGrid for the given class ID
    /// @param  classId The TileGrid class
    /// @return A pointer to the TileGrid or nullptr if not found
    ///
    const Grid& grid(Class classId) const;
    /// @return The number of tiles per grid unit
    ///
    int granularity() const;
    /// @return The Block's name
    ///
    const string& name() const { return _name; }
    /// @return The paint style used by the Builder toolbox
    ///
    BuilderPaintStyle paintStyle() const { return _paintStyle; }
    /// @return The layer type used for painting to the Stage
    ///
    Layer layerType() const { return _type; }

private:
    Grid createGrid(int dimension, const Allocator& allocator);

private:
    Allocator _allocator;
    Layer _type;
    BuilderPaintStyle _paintStyle;
    string _name;
    std::array<Grid, kClass_Count> _grids;
    int _granularity;
};

////////////////////////////////////////////////////////////////////////////////

inline Block::operator bool() const
{
    return _granularity > 0;
}

inline auto Block::grid(Class classId) -> Grid&
{
    return _grids[classId % _grids.size()];
}

inline auto Block::grid(Class classId) const -> const Grid&
{
    return _grids[classId % _grids.size()];
}

inline int Block::granularity() const
{
    return _granularity;
}

inline bool Block::hasGrid(Class classId) const
{
    return _grids[classId];
}

} /* namespace ovengine */ } /* namespace cinekine */

#endif
