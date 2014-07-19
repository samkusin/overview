//
//  BlockCollectionLoader.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/BlockCollectionLoader.hpp"
#include "Engine/Model/BlockCollection.hpp"
#include "Engine/Debug.hpp"

#include "Core/StreamBufRapidJson.hpp"
#include "Core/JsonUtilities.hpp"

#include <cstring>

namespace cinekine {
    namespace ovengine {

BlockCollectionLoader::BlockCollectionLoader(
                        std::function<void(BlockCollection&&)> collectionCb,
                        const Allocator& allocator) :
    _collectionCb(collectionCb),
    _models(allocator)
{
}

bool BlockCollectionLoader::startCollection(const char* name, uint32_t modelCount)
{
    _models.clear();
    _models.reserve(modelCount);
    _name = name;
    _tilesetName = "";
    return true;
}

bool BlockCollectionLoader::parseAttribute(const char* key, const JsonValue& value)
{
    if (!strcmp(key, "tileset"))
    {
        _tilesetName = value.GetString();
    }
    return true;
}

bool BlockCollectionLoader::parseModel(const char* key, JsonValue& object)
{

}

bool BlockCollectionLoader::endCollection()
{

}

bool unserializeFromJSON(BlockCollection& outputCollection,
                         std::streambuf& instream)
{
    RapidJsonStdStreamBuf jsonStream(instream);

    JsonDocument jsonDoc;
    jsonDoc.ParseStream<0>(jsonStream);

    if (jsonDoc.HasParseError() || !jsonDoc.IsObject())
    {
        OVENGINE_LOG_ERROR("unserializeFromJSON - failed to parse the block stream.");
        return false;
    }

    //  iterate through all collections
    for (auto collectionIter = jsonDoc.MemberBegin(),
              collectionIterEnd = jsonDoc.MemberEnd();
         collectionIter != collectionIterEnd;
         ++collectionIter)
    {

    }

    const auto& flags = jsonDoc["flags"];
    const auto& collection = jsonDoc["collection"];

    if (flags.IsNull() || !flags.IsObject())
    {
        OVENGINE_LOG_ERROR("unserializeFromJSON - no flags found.");
        return false;
    }
    if (collection.IsNull() || !collection.IsObject())
    {
        OVENGINE_LOG_ERROR("unserializeFromJSON - no collection found.");
        return false;
    }

    const char* collectionName = collection["name"].GetString();
    if (!collectionName[0])
    {
        OVENGINE_LOG_ERROR("unserializeFromJSON - collection does not have a name.");
        return false;
    }

    const auto& tiles = collection["tiles"];
    if (tiles.IsNull() && !tiles.IsObject())
    {
        OVENGINE_LOG_ERROR("unserializeFromJSON - "
                           "Tile collection '%s' has no tile map entry.",
                           collectionName);
        return false;
    }

    cinek_bitmap_atlas bitmapAtlasId = atlasReqCb(collectionName);
    if (bitmapAtlasId == kCinekBitmapAtlas_Invalid)
    {
        OVENGINE_LOG_ERROR("unserializeFromJSON - "
                           "Tile collection '%s' has an invalid atlas.",
                           collectionName);
        return false;
    }

    //  Reserve our tile vector for the collection.
    size_t tileCount = 0;
    for (auto tileIt = tiles.MemberBegin(), tileItEnd = tiles.MemberEnd();
         tileIt != tileItEnd;
         ++tileIt, ++tileCount) {}

    vector<Tile> tileCollectionTiles;
    tileCollectionTiles.reserve(tileCount);

    for (auto tileIt = tiles.MemberBegin(), tileItEnd = tiles.MemberEnd();
         tileIt != tileItEnd;
         ++tileIt)
    {
        const auto& tile =  *tileIt;
        const char* tileName = nullptr;
        uint32_t tileFlags = 0;
        AABB aabb;
        TileId tileIndex = 0;
        if (tile.name.IsString())
        {
            tileIndex = parseUint(tile.name);
        }
        if (tileIndex == 0)
        {
            OVENGINE_LOG_ERROR("unserializeFromJSON - "
                               "Tile collection %s, '%s' is not a valid index",
                               collectionName,
                               tile.name.GetString());
            continue;
        }
        if (tile.value.IsObject())
        {
            const auto& tileData = tile.value;
            for (auto tileDataIt = tileData.MemberBegin(),
                      tileDataItEnd = tileData.MemberEnd();
                 tileDataIt != tileDataItEnd;
                 ++tileDataIt)
            {
                const auto& tileAttr = *tileDataIt;
                const char* tileAttrName = tileAttr.name.GetString();
                if (!strcmp(tileAttrName, "name"))
                {
                    tileName = tileAttr.value.GetString();
                }
                else if (!strcmp(tileAttrName, "flags"))
                {
                    const char* flagsStr = tileAttr.value.GetString();
                    tileFlags = parseFlagsToUint(flags, flagsStr);
                }
                else if (!strcmp(tileAttrName, "aabb"))
                {
                    aabb.min = parseVec3(tileAttr.value["min"]);
                    aabb.max = parseVec3(tileAttr.value["max"]);
                }
            }
        }
        else if (tile.value.IsString())
        {
            tileName = tile.value.GetString();
        }
        if (!tileName)
        {
            OVENGINE_LOG_ERROR("unserializeFromJSON - "
                               "Tile collection %s, tile %s has no valid mapping.",
                               collectionName,
                               tile.name.GetString());
            continue;
        }
        Tile tileInfo;
        tileInfo.bitmap = { bitmapAtlasId,
                            bitmapReqCb(bitmapAtlasId, tileName) };
        tileInfo.flags = tileFlags;
        tileInfo.aabb = aabb;
        tileCollectionTiles.push_back(tileInfo);
    }

    TileCollection tc(collectionName, std::move(tileCollectionTiles));
    outputCollection = std::move(tc);

    return true;
}

    }   // namespace ovengine
}   // namespace cinekine
