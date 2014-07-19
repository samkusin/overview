///
/// @file
/// Contains the definition for the tile-based Block class
///
/// @author     Samir Sinha
/// @date       07/15/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Model_Block_hpp
#define Overview_Model_Block_hpp

#include "Engine/Model/ModelTypes.hpp"
#include "Engine/Model/Tile.hpp"
#include "Engine/Model/StageTypes.hpp"

#include "cinek/allocator.hpp"
#include "cinek/string.hpp"

#include <array>

namespace cinekine { namespace ovengine {

/// @class  Block
/// @ingroup TileModel
/// @brief  A Block is a collection of "mini" tilemaps referenced by class, and
///         typically used by Builder based applications
///
class Block
{
public:
    enum Class
    {
        kClass_1x1,         ///< Has a tile width/height equal to granularity
        kClass_3x3,         ///< Has a tile dimension equal to 3 * granularity
        kClass_Count        ///< Total number of available classes
    };

    /// Default Constructor
    ///
    Block();
    /// @return Whether the Block is valid (granularity > 0)
    ///
    operator bool() const;
    /// Constructor
    /// @param granularity The tile dimension (tiles width and height)
    ///
    Block(const char* name, int granularity, const Allocator& allocator=Allocator());
    /// Returns a TileGrid for the given class ID
    /// @param  classId The TileGrid class
    /// @return A pointer to the TileGrid or nullptr if not found
    ///
    TileGrid& grid(Class classId);
    /// Returns a const TileGrid for the given class ID
    /// @param  classId The TileGrid class
    /// @return A pointer to the TileGrid or nullptr if not found
    ///
    const TileGrid& grid(Class classId) const;
    /// @return The number of tiles per grid unit
    ///
    int granularity() const;
    /// @return The Block's name
    ///
    const string& name() const { return _name; }

private:
    TileGrid createGrid(int dimension, const Allocator& allocator);

private:
    string _name;
    std::array<TileGrid, kClass_Count> _grids;
    int _granularity;
};

////////////////////////////////////////////////////////////////////////////////

inline Block::operator bool() const
{
    return _granularity > 0;
}

inline TileGrid& Block::grid(Class classId)
{
    return _grids[classId % _grids.size()];
}

inline const TileGrid& Block::grid(Class classId) const
{
    return _grids[classId % _grids.size()];
}

inline int Block::granularity() const
{
    return _granularity;
}

} /* namespace ovengine */ } /* namespace cinekine */

#endif
