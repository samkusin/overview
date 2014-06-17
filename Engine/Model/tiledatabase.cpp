/**
 * @file    Model/TileDatabase.cpp
 *
 * Container for Tiles
 *
 * @note    Created by Samir Sinha on 7/4/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "TileDatabase.hpp"

namespace cinekine {
    namespace ovengine {

    TileDatabase::TileDatabase(uint16_t tileLimit, const Allocator& allocator) :
        _tiles(tileLimit, Tile(),
               std_allocator<Tile>(allocator)),
        _tilesByDescriptor(std_allocator<std::pair<uint32_t, TileId>>(allocator))
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

    void TileDatabase::mapTileToInfo(TileId tile, const Tile& info)
    {
        if (tile >= _tiles.size())
            return;
        _tiles[tile] = info;

        uint32_t descriptor = makeDescriptor(info);
        _tilesByDescriptor[descriptor] = tile;
    }

    TileId TileDatabase::tileHandleFromDescriptor(
                                    uint8_t categoryId,
                                    uint8_t classId,
                                    uint16_t roleFlags) const
    {
        Tile tile;
        tile.categoryIndex = categoryId;
        tile.classIndex = classId;
        tile.flags = roleFlags;
        uint32_t descriptor = makeDescriptor(tile);
        auto it = _tilesByDescriptor.find(descriptor);
        if (it == _tilesByDescriptor.end())
            return 0;

        return it->second;
    }

    uint32_t TileDatabase::makeDescriptor(const Tile& tile) const
    {
        uint32_t descriptor = tile.flags;
        descriptor <<= 8;
        descriptor += tile.categoryIndex;
        descriptor <<= 8;
        descriptor += tile.classIndex;
        return descriptor;
    }

    }   // namespace ovengine
}   //  namespace cinekine
