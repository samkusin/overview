//
//  tiledatabase.cpp
//  ovpackages
//
//  Created by Samir Sinha on 8/24/13.
//
//

#include "tiledatabase.hpp"

namespace cinekine {
    namespace rendermodel {

    TileDatabase::TileDatabase(uint16_t tileLimit, const Allocator& allocator) :
        _tiles(tileLimit, TileInfo(kCinekBitmapAtlas_Invalid, kCinekBitmapIndex_Invalid),
               std_allocator<TileInfo>(allocator))
    {
    }
    TileDatabase::TileDatabase(TileDatabase&& other) :
        _tiles(std::move(other._tiles))
    {
    }
   
    void TileDatabase::clear()
    {
        _tiles.clear();
    }
   
    void TileDatabase::mapTileToBitmap(cinek_tile tile,
                                       cinek_bitmap_atlas atlas,
                                       cinek_bitmap_index index)
    {
        if (tile >= _tiles.size())
            return;
        _tiles[tile] = TileInfo(atlas, index);
    }
        
    }   // namespace rendermodel
}   //  namespace cinekine
