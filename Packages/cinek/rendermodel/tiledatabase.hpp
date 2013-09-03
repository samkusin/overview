/**
 * @file    rendermodel/tiledatabase.hpp
 *
 * Container for Tiles
 *
 * @note    Created by Samir Sinha on 7/4/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_RENDER_TILEDATABASE_HPP
#define CINEK_RENDER_TILEDATABASE_HPP

#include "cinek/core/ckdebug.h"
#include "cinek/cpp/ckstring.hpp"
#include "cinek/cpp/ckmemorypool.hpp"
#include "cinek/rendermodel/types.h"

#include <vector>

namespace cinekine {
    namespace rendermodel {

/** Defines a tile based on bitmap atlas and index. */
typedef std::pair<cinek_bitmap_atlas, cinek_bitmap_index> TileInfo;

/**
 * @class TileDatabase
 * @brief A store for Tile records.
 *
 * Tiles are compressed bitmap atlas/index pairs to minimize the memory usage of
 * tilemap grids.
 */
class TileDatabase
{
    CK_CLASS_NON_COPYABLE(TileDatabase);
    
public:
    /**
     * @param tileLimit The maximum number of tiles to reserve for the
     *                  database.
     * @param allocator An optional allocator.
     */
    TileDatabase(uint16_t tileLimit, const Allocator& allocator=Allocator());
    /**
     * @param other The source database.
     */
    TileDatabase(TileDatabase&& other);
    /**
     * Clears the database.
     */
    void clear();
    /**
     * @return The maximum number of tiles allowed in the database.
     */
    /**
     * Maps a tile atlas to a bitmap atlas.
     * 
     * @param tile  The tile index to map bitmap info to.
     * @param atlas The bitmap atlas used for the specified tile.
     * @param index The bitmap index into the atlas used for the specified tile.
     */    
    void mapTileToBitmap(cinek_tile tile, cinek_bitmap_atlas atlas,
                         cinek_bitmap_index index);

    /**
     * @param  tile The tile info to retrieve.
     * @return A TileInfo pair object containing bitmap atlas and index.
     */
    const TileInfo& getTileInfo(cinek_tile tile) const {
        CK_ASSERT(tile < _tiles.size());
        return _tiles.at(tile);
    }

private:
    std::vector< TileInfo, std_allocator<TileInfo> > _tiles;
};

    }   // namespace rendermodel
}   //  namespace cinekine

#endif
