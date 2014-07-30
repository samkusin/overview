//
//  TileCollectionLoader.cpp
//  Implementation
//
//  Copyright 2013 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/TileCollectionLoader.hpp"
#include "Engine/Model/TileCollection.hpp"
#include "Engine/Debug.hpp"

#include "Core/StreamBufRapidJson.hpp"
#include "Core/JsonUtilities.hpp"

#include <cstring>
#include <cstdlib>

namespace cinekine {
    namespace ovengine {

TileCollectionLoader::TileCollectionLoader(
        const JsonValue& tileFlagConsts,
        std::function<cinek_bitmap_atlas(const char*)> atlasReqCb,
        std::function<cinek_bitmap_index(cinek_bitmap_atlas, const char*)> bitmapReqCb,
        std::function<void(TileCollection&&)> collectionCb,
        const Allocator& allocator) :
    _tileFlagConsts(tileFlagConsts),
    _atlasReqCb(atlasReqCb),
    _bitmapReqCb(bitmapReqCb),
    _collectionCb(collectionCb),
    _atlasId(kCinekBitmapAtlas_Invalid),
    _tiles(allocator)
{
}

bool TileCollectionLoader::startCollection(const char* name, uint32_t modelCount)
{
    //  start with a fresh model container
    _tiles.clear();
    _tiles.reserve(modelCount);
    _name = name;
    _atlasId = _atlasReqCb(name);
    return true;
}

bool TileCollectionLoader::parseAttribute(const char* key, const JsonValue& value)
{
    //  none
    return true;
}

bool TileCollectionLoader::parseModel(const char* key, JsonValue& object)
{
    Tile tile;

    for (auto attrIt = object.MemberBegin(), attrItEnd = object.MemberEnd();
         attrIt != attrItEnd;
         ++attrIt)
    {
        auto& attr = *attrIt;
        const char* attrName = attr.name.GetString();
        if (!strcmp(attrName, "name"))
        {
            tile.bitmap.bmpAtlas = _atlasId;
            tile.bitmap.bmpIndex = _bitmapReqCb(_atlasId, attr.value.GetString());
        }
        else if (!strcmp(attrName, "flags"))
        {
            const char* flagsStr = attr.value.GetString();
            tile.flags = parseFlagsToUint(_tileFlagConsts, flagsStr);
        }
        else if (!strcmp(attrName, "aabb"))
        {
            tile.aabb.min = parseVec3(attr.value["min"]);
            tile.aabb.max = parseVec3(attr.value["max"]);
        }
        else if (!strcmp(attrName, "anchor"))
        {
            tile.anchor.x = parseInt(attr.value["x"]);
            tile.anchor.y = parseInt(attr.value["y"]);
        }
    }
    uint16_t tileIndex = (uint16_t)strtoul(key, nullptr, 16);
    _tiles.emplace_back(tileIndex, tile);
    return true;
}

bool TileCollectionLoader::endCollection()
{
    if (_tiles.empty())
        return false;

    //  convert our tiles vecrtor into a single sparse vector with capacity
    //  equal to the maximum tile index
    std::sort(_tiles.begin(), _tiles.end(),
              [](const TilePair& l, const TilePair& r) -> bool {
                return l.first < r.first;
              });
    uint16_t tileIndexMax = _tiles.back().first;
    //  create our sparse vector
    TileCollection::Container models(_tiles.get_allocator());
    models.resize(tileIndexMax+1);
    for (auto& tile : _tiles)
    {
        models[tile.first] = tile.second;
    }
    TileCollection collection(_name.c_str(), std::move(models));
    _collectionCb(std::move(collection));
    return true;
}

    }   // namespace ovengine
}   // namespace cinekine
