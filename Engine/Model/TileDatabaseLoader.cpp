//
//  TileDatabaseLoader.cpp
//  Overview
//
//  Created by Samir Sinha on 8/24/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "TileDatabaseLoader.hpp"
#include "TileDatabase.hpp"
#include "ModelTypes.hpp"

#include "Engine/Debug.hpp"
#include "Core/StreamBufRapidJson.hpp"
#include "Core/JsonUtilities.hpp"

#include <cstring>


namespace cinekine {
    namespace ovengine {

static uint16_t parseFlagsToUint(const JsonValue& flagsDef, const char* flags)
{
    uint16_t result = 0;

    const char* end = flags + strlen(flags);
    const char* current = flags;
    while (current < end)
    {
        const char* next = strchr(current, ',');
        if (!next)
            next = end;
        size_t slen = next - current;
        if (slen)
        {
            for (auto memberIt = flagsDef.MemberBegin(), memberItEnd = flagsDef.MemberEnd();
                 memberIt != memberItEnd;
                 ++memberIt)
            {
                const auto& member = *memberIt;
                const char* flagStr = member.name.GetString();
                if (!strncmp(current, flagStr, slen) && strlen(flagStr) == slen)
                {
                    result |= parseUint(member.value);
                    break;
                }
            }
        }

        current = next + 1;
    }

    return result;
}

TileDatabaseLoader::TileDatabaseLoader(TileDatabase& database) :
    _db(database)
{

}

bool TileDatabaseLoader::unserialize(std::streambuf& instream,
                    std::function<cinek_bitmap_atlas(const char*)> atlasReqCb,
                    std::function<cinek_bitmap_index(cinek_bitmap_atlas, const char*)> bitmapReqCb)
{
    RapidJsonStdStreamBuf jsonStream(instream);

    JsonDocument jsonDoc;
    jsonDoc.ParseStream<0>(jsonStream);

    if (jsonDoc.HasParseError() || !jsonDoc.IsObject())
    {
        OVENGINE_LOG_ERROR("TileDatabaseLoader - failed to parse.");
        return false;
    }

    const auto& classes = jsonDoc["classes"];
    const auto& flags = jsonDoc["flags"];
    const auto& categories = jsonDoc["categories"];

    if (classes.IsNull() || !classes.IsObject())
    {
        OVENGINE_LOG_ERROR("TileDatabaseLoader - no classes entry found.");
        return false;
    }
    if (flags.IsNull() || !flags.IsObject())
    {
        OVENGINE_LOG_ERROR("TileDatabaseLoader - no flags found.");
        return false;
    }
    if (categories.IsNull() || !categories.IsObject())
    {
        OVENGINE_LOG_ERROR("TileDatabaseLoader - no categories found.");
        return false;
    }

    for (auto it = categories.MemberBegin(), itEnd = categories.MemberEnd();
         it != itEnd;
         ++it)
    {
        const auto& member = *it;
        const char* categoryName = member.name.GetString();
        if (!member.value.IsObject())
        {
            OVENGINE_LOG_ERROR("TileDatabaseLoader - "
                               "Tile Category %s is not an object.",categoryName);
            continue;
        }

        const auto& category = member.value;
        uint8_t categoryIndex = parseUint(category["index"]);
        if (!categoryIndex)
        {
            OVENGINE_LOG_ERROR("TileDatabaseLoader - Category %s has no index",
                                categoryName);
            continue;
        }

        const auto& tiles = category["tiles"];
        if (tiles.IsNull() && !tiles.IsObject())
        {
            OVENGINE_LOG_ERROR("TileDatabaseLoader - "
                               "Tile Category %s index has no tile map entry.",
                               categoryName);
            continue;
        }

        cinek_bitmap_atlas bitmapAtlasId = atlasReqCb(categoryName);
        if (bitmapAtlasId == kCinekBitmapAtlas_Invalid)
        {
            continue;
        }

        for (auto tileIt = tiles.MemberBegin(), tileItEnd = tiles.MemberEnd();
             tileIt != tileItEnd;
             ++tileIt)
        {
            const auto& tile =  *tileIt;
            TileId tileIndex;
            uint8_t tileClass = 0;
            uint16_t tileFlags = 0;
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
                    if (!strcmp(tileAttrName, "index"))
                    {
                        tileIndex = parseUint(tileAttr.value);
                    }
                    else if (!strcmp(tileAttrName, "class"))
                    {
                        auto& className = classes[tileAttr.value.GetString()];
                        if (!className.IsNull())
                        {
                            tileClass = parseUint(className);
                        }
                    }
                    else if (!strcmp(tileAttrName, "flags"))
                    {
                        const char* flagsStr = tileAttr.value.GetString();
                        tileFlags = parseFlagsToUint(flags, flagsStr);
                    }
                }
            }
            else if (tile.value.IsUint())
            {
                tileIndex = (TileId)(tile.value.GetUint());
            }
            else
            {
                OVENGINE_LOG_ERROR("TileDatabaseLoader - "
                                   "Tile Category %s, bitmap %s has no mapping.",
                                   categoryName,
                                   tile.name.GetString());
                continue;
            }
            TileInfo tileInfo= {
                {
                    bitmapAtlasId,
                    bitmapReqCb(bitmapAtlasId, tile.name.GetString())
                },
                categoryIndex,
                tileClass,
                tileFlags
            };
            _db.mapTileToInfo(tileIndex, tileInfo);
        }
    }

    return true;
}

    }   // namespace ovengine
}   // namespace cinekine
