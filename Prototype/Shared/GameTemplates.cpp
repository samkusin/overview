//
//  GameTemplates.cpp
//  Overview
//
//  Created by Samir Sinha on 10/16/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Shared/GameTemplates.hpp"
#include "Engine/Model/EntityTemplateCollectionLoader.hpp"
#include "Engine/Model/TileGridMap.hpp"
#include "Engine/Model/RoomGraph.hpp"
#include "Engine/Model/TileCollectionLoader.hpp"
#include "Engine/Model/SpriteCollectionLoader.hpp"
#include "Graphics/BitmapLibrary.hpp"
#include "Core/FileStreamBuf.hpp"
#include "Core/StreamBufRapidJson.hpp"

namespace cinek { namespace overview {

    GameTemplates::GameTemplates(const InitParams& params,
                                 const Allocator& allocator) :
        _allocator(allocator),
        _tileLibrary(params.tileSlotLimit, _allocator)
    {
        FileStreamBuf gameStream(params.gameDefinitionPath);
        RapidJsonStdStreamBuf jsonStream(gameStream);
        _gameDefinition.ParseStream<0>(jsonStream);
    }

    GameTemplates::~GameTemplates() = default;

    void GameTemplates::loadTileCollection(const char* pathname,
                                           glx::BitmapLibrary& bitmapLibrary)
    {
        FileStreamBuf dbStream(pathname);
        if (!dbStream)
            return;

        TileCollectionLoader tileLoader(_gameDefinition["model"],
                                        [&bitmapLibrary](const char* atlasName) -> cinek_bitmap_atlas
                                        {
                                            char path[MAX_PATH];
                                            snprintf(path, sizeof(path), "textures/%s", atlasName);
                                            return bitmapLibrary.loadAtlas(path);
                                        },
                                        [&bitmapLibrary](cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
                                        {
                                            const glx::BitmapAtlas* bitmapAtlas = bitmapLibrary.getAtlas(atlas).get();
                                            if (!bitmapAtlas)
                                                return kCinekBitmapIndex_Invalid;
                                            return bitmapAtlas->getBitmapIndex(name);
                                        },
                                        [this](TileCollection&& collection)
                                        {
                                            _tileLibrary.mapCollectionToSlot(std::move(collection), 0);
                                        },
                                        _allocator);
        unserializeFromJSON(dbStream, tileLoader);

    }

    void GameTemplates::loadSpriteCollection(const char* pathname,
                                             glx::BitmapLibrary& bitmapLibrary)
    {
        FileStreamBuf dbStream(pathname);
        if (!dbStream)
        {
            OVENGINE_LOG_ERROR("GameTemplates.loadSpriteCollection: "
                               "Cannot find collection %s", pathname);
            return;
        }
        SpriteCollectionLoader spriteLoader(_gameDefinition["model"]["sprites"],
                            [&bitmapLibrary](const char* atlasName) -> cinek_bitmap_atlas
                            {
                                char path[MAX_PATH];
                                snprintf(path, sizeof(path), "textures/%s", atlasName);
                                return bitmapLibrary.loadAtlas(path);
                            },
                            [&bitmapLibrary]( cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
                            {
                                const glx::BitmapAtlas* bitmapAtlas = bitmapLibrary.getAtlas(atlas).get();
                                if (!bitmapAtlas)
                                    return kCinekBitmapIndex_Invalid;
                                return bitmapAtlas->getBitmapIndex(name);
                            },
                            [this](SpriteCollection&& collection)
                            {
                                _spriteCollection = std::move(collection);
                            },
                            _allocator);
        unserializeFromJSON(dbStream, spriteLoader);
    }

    void GameTemplates::loadEntityTemplateCollection(const char* pathname)
    {
        FileStreamBuf dbStream(pathname);
        if (!dbStream)
        {
            OVENGINE_LOG_ERROR("loadEntityTemplateCollection: "
                               "Cannot find collection %s", pathname);
            return;
        }
        EntityTemplateCollectionLoader loader(JsonValue(),
                                              [this](EntityTemplateCollection&& collection)
                                              {
                                                  _entityTemplColl = std::move(collection);
                                              },
                                              _allocator);


        unserializeFromJSON(dbStream, loader);
    }

} /* namespace overview */ } /* namespace cinek */
