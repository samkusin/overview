///
/// @file
/// Common types for all Model objects
///
/// @author     Samir Sinha
/// @date       07/16/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Builder_Hpp
#define Overview_Builder_Hpp

#include "Engine/Builder/BuilderTypes.hpp"
#include "Engine/Model/Stage.hpp"

#include "cinek/vector.hpp"

namespace cinekine {
    namespace ovengine {
        class TileLibrary;
        class BlockLibrary;
    }
}

namespace cinekine { namespace ovengine {

    /// @defgroup Builder The Map Grid Builder
    /// The Builder is a toolbox used by map generators to procedurally create
    /// tile grids onto Stages.
    ///

    /// @class Builder
    /// @brief The front-end interface for the Builder toolbox.
    ///
    /// Applications create a Builder to procedurally generate maps using the
    /// supplied Tile and Block libraries.   Tile maps/grids are managed by
    /// a Stage object also supplied by the application.
    ///
    class Builder
    {
    public:
        Builder(Stage& stage,
                const TileLibrary& tileLibrary,
                const BlockLibrary& blockLibrary,
                const Allocator& allocator);

    private:
        Allocator _allocator;
        Stage& _map;
        const TileLibrary& _tileTemplates;
        const BlockLibrary& _blockTemplates;
    };
} /* namespace overview */ } /* namespace cinekine */

#endif
