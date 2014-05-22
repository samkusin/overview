//
//  Stage.hpp
//  Overview
//
//  Created by Samir Sinha on 5/20/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Model_Stage_hpp
#define Overview_Model_Stage_hpp

#include "Map.hpp"

#include "cinek/framework/allocator.hpp"
#include "cinek/rendermodel/tiledatabase.hpp"
#include "cinek/rendermodel/spritedatabase.hpp"

namespace cinekine {
    namespace glx {
        class BitmapLibrary;
    }
}

namespace cinekine { namespace ovengine {

    class Stage
    {
    public:
        struct ResourceCounts
        {
            uint16_t tileLimit = 16;
            uint16_t spriteLimit = 16;
        };

        Stage(glx::BitmapLibrary& bitmapLibrary,
              const ResourceCounts& counts,
              const cinek_ov_map_bounds& bounds,
              const Allocator& allocator);

        const rendermodel::TileDatabase& tileDatabase() const {
            return _tileDb;
        }
        void loadTileDatabase(const char* filename);

        const rendermodel::SpriteDatabase& spriteDatabase() const {
            return _spriteDb;
        }
        void loadSpriteDatabase(const char* filename);

        const Map& map() const {
            return _map;
        }
        Map& map() {
            return _map;
        }

    private:
        Allocator _allocator;
        glx::BitmapLibrary& _bitmapLibrary;
        rendermodel::TileDatabase _tileDb;
        rendermodel::SpriteDatabase _spriteDb;
        Map _map;
    };


} /* namespace ovengine */ } /* namespace cinekine */

#endif

