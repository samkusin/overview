//
//  TileDatabaseLoader.cpp
//  Overview
//
//  Created by Samir Sinha on 8/24/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "TileDatabaseLoader.hpp"

#include "Engine/Debug.hpp"
#include "Engine/Utils/StreamBufRapidJson.hpp"
#include "rapidjson/document.h"

#include "cinek/rendermodel/tiledatabase.hpp"


namespace cinekine {
    namespace ovengine {
        
    TileDatabaseLoader::TileDatabaseLoader(rendermodel::TileDatabase& database) :
        _db(database)
    {
        
    }
    
    bool TileDatabaseLoader::unserialize(std::streambuf& instream)
    {
        typedef rapidjson::GenericDocument<rapidjson::UTF8<> > Document;
        typedef rapidjson::GenericValue<rapidjson::UTF8<> > Value;
        
        rapidjson::StdStreamBuf jsonStream(instream);
        
        Document jsonDoc;
        jsonDoc.ParseStream<0>(jsonStream);
        
        if (jsonDoc.HasParseError() || !jsonDoc.IsObject())
        {
            OVENGINE_LOG_ERROR("TileDatabaseLoader - failed to parse.");
            return false;
        }
        
        //  determine tilemask - this becomes the tile limit.
        cinek_tile tileMask = 0x03ff;
        
        if (jsonDoc.HasMember("mask") && jsonDoc["mask"].IsString())
        {
            cinek_tile newMask = strtoul(jsonDoc["mask"].GetString(), NULL, 16);
            if (newMask == 0)
                tileMask = newMask;
        }
        
        //  generate tile atlas map
        if (jsonDoc.HasMember("atlases") && jsonDoc["atlases"].IsObject())
        {
            const Value& atlases = jsonDoc["atlases"];
            for (auto it = atlases.MemberBegin(), itEnd = atlases.MemberEnd();
                 it != itEnd;
                 ++it)
            {
                const Value::Member& member = *it;
                if (!member.value.IsObject())
                {
                    OVENGINE_LOG_ERROR("TileDatabaseLoader - Tile Atlas %s entry is not an object.", member.name.GetString());
                    continue;
                }
                const Value& atlas = member.value;
                if (!member.value.HasMember("tileAtlas") || !atlas["tileAtlas"].IsUint())
                {
                    OVENGINE_LOG_ERROR("TileDatabaseLoader - Tile Atlas %s index is not an integer.",
                                       member.name.GetString());
                    continue;
                }
                if (!atlas.HasMember("tiles") || !atlas["tiles"].IsObject())
                {
                    OVENGINE_LOG_ERROR("TileDatabaseLoader - Tile Atlas %s index has no tile map entry.",
                                       member.name.GetString());
                    continue;
                }
    
                cinek_bitmap_atlas bitmapAtlasId = _atlasRequest(member.name.GetString());
                if (bitmapAtlasId == kCinekBitmapAtlas_Invalid)
                {
                    continue;
                }
                uint8_t tileAtlasId = atlas["tileAtlas"].GetUint();           
                const Value& tiles = atlas["tiles"];
                for (auto tileIt = tiles.MemberBegin(), tileItEnd = tiles.MemberEnd();
                     tileIt != tileItEnd;
                     ++tileIt)
                {
                    const Value::Member& tile =  *tileIt;
                    cinek_tile tileIndex = (cinek_tile)(tile.value.GetUint());
                    cinek_bitmap tileBitmap = {
                        bitmapAtlasId,
                        this->_indexRequest(bitmapAtlasId, tile.name.GetString())
                    };
                    _db.mapTileToBitmap(tileAtlasId+tileIndex, tileBitmap);
                }
            }
        }
        

        return true;
    }
        
        
    }   // namespace ovengine
}   // namespace cinekine
