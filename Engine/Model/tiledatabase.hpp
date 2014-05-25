/**
 * @file    Model/TileDatabase.hpp
 *
 * Container for Tiles
 *
 * @note    Created by Samir Sinha on 7/4/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Overview_Model_TileDatabase_hpp
#define Overview_Model_TileDatabase_hpp

#include "Engine/Model/ModelTypes.hpp"

#include "cinek/debug.hpp"
#include "cinek/string.hpp"
#include "cinek/vector.hpp"
#include "cinek/map.hpp"
#include "cinek/rendertypes.h"

#include <vector>

namespace cinekine {
    namespace ovengine {

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
     * @param tile   The tile index to map bitmap info to.
     * @param bitmap The bitmap atlas and index used for the specified tile.
     */
    void mapTileToInfo(TileId tile, const TileInfo& info);
    /**
     * @param  tile The tile info to retrieve.
     * @return A TileInfo pair object containing bitmap atlas and index.
     */
    const TileInfo& tileInfo(TileId tile) const {
        CK_ASSERT(tile < _tiles.size());
        return _tiles.at(tile);
    }

    /**
     * Retrieves a TileTemplate from the given description/selection
     * parameters
     * @param  categoryId Tile category
     * @param  classId    Tile class
     * @param  roleFlags  Tile role
     * @return            The matching TileTemplate.  If there is no tile
     *                    found, the returned template has an handle of 0
     *                    which usually indicates an empty tile
     */
    TileId tileHandleFromDescriptor(uint8_t categoryId,
                                    uint8_t classId,
                                    uint16_t roleFlags) const;

private:
    vector< TileInfo > _tiles;
    unordered_map<uint32_t, TileId> _tilesByDescriptor;

    uint32_t makeDescriptor(const TileInfo& tile) const;
};

    }   // namespace ovengine
}   //  namespace cinekine

#endif
