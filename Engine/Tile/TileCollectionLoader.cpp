//
//  TileCollectionLoader.cpp
//  Implementation
//
//  Copyright 2013 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Tile/TileCollectionLoader.hpp"
#include "Engine/Tile/TileCollection.hpp"
#include "Engine/Debug.hpp"
#include "CKGfx/BitmapLibrary.hpp"
#include "CKGfx/BitmapAtlas.hpp"

#include <cinek/json/jsonstreambuf.hpp>
#include <cinek/json/json.hpp>
#include <cstring>
#include <cstdlib>

namespace cinek {
    namespace overview {

TileCollectionLoader::TileCollectionLoader
(
    const gfx::BitmapLibrary& bitmapLibrary,
    const JsonValue& modelConsts,
    const RequestAtlasCb& atlasReqCb,
    const CollectionLoadedCb& collectionCb,
    const Allocator& allocator
) :
    _bitmapLibrary(&bitmapLibrary),
    _modelConsts(&modelConsts),
    _atlasReqCb(atlasReqCb),
    _collectionCb(collectionCb),
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
    
    auto atlas = _bitmapLibrary->atlas(_atlasId);
    if (!atlas)
        return false;

    for (auto attrIt = object.MemberBegin(), attrItEnd = object.MemberEnd();
         attrIt != attrItEnd;
         ++attrIt)
    {
        auto& attr = *attrIt;
        const char* attrName = attr.name.GetString();
        if (!strcmp(attrName, "name"))
        {
            tile.bitmap.atlas = _atlasId;
            tile.bitmap.index = atlas->bitmapIndexFromName(attr.value.GetString());
        }
        else if (!strcmp(attrName, "categories"))
        {
            const char* flagsStr = attr.value.GetString();
            tile.categories = parseFlagsToUint((*_modelConsts)["categories"], flagsStr);
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
        else if (!strcmp(attrName, "collide"))
        {
            const char* collisionShape = attr.value["type"].GetString();
            if (!strcasecmp(collisionShape, "box"))
                tile.collision.shape = CollisionInfo::Shape::kBox;
            else if (!strcasecmp(collisionShape, "plane"))
                tile.collision.shape = CollisionInfo::Shape::kPlane;
            else
                tile.collision.shape = CollisionInfo::Shape::kNone;

            if (attr.value.HasMember("access"))
            {
                const char* flagsStr = attr.value["access"].GetString();
                tile.collision.access = parseFlagsToUint((*_modelConsts)["access"], flagsStr);
            }
            if (attr.value.HasMember("impedence"))
            {
                tile.collision.impedence = parseInt(attr.value["impedence"]);
            }
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

    }   // namespace overview
}   // namespace cinek
