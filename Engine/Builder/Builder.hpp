///
/// @file
/// Common types for all Model objects
///
/// @author     Samir Sinha
/// @date       07/16/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_GridBuilder_Hpp
#define Overview_GridBuilder_Hpp

#include "Engine/Builder/BuilderTypes.hpp"
#include "Engine/Builder/BlockLibrary.hpp"

#include "cinek/vector.hpp"
#include "cinek/string.hpp"


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
    /// @brief Utilities for building grid-based maps
    ///
    /// Applications use GridBuilder to procedurally generate maps using the
    /// supplied Tile and Block libraries.
    ///
    class GridBuilder
    {
    public:
        GridBuilder(TileGrid& gridMap,
                    const TileLibrary& tileLibrary,
                    const BlockLibrary& blockLibrary,
                    const Allocator& allocator);
        
        void clear();

        glm::ivec2 dimensions() const;
        
        void setBlockCollection(BlockCollectionId id);
        void paintRect(const string& blockName,
                       int x0, int y0, int x1, int y1);

    private:
        typedef void (*PlotFn)(TileGrid&, const TileGrid&, TileSlot, int, int, int, int);

        PlotFn plotFunctionFromStyle(BuilderPaintStyle style) const;

    private:
        Allocator _allocator;
        TileGrid& _grid;
        const TileLibrary& _tileTemplates;
        const BlockLibrary& _blockTemplates;

        //  context
        const BlockCollection* _blockCollection;
        TileSlot _tileCollectionSlot;
    };
    
    
} /* namespace overview */ } /* namespace cinekine */

#endif
