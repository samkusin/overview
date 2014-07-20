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
    _allocator(allocator),
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
    int granularity = object["granularity"].GetInt();

    Block block(key, granularity, _allocator);

    //  parse our grids
    for (auto attrIt = object.MemberBegin(), attrItEnd = object.MemberEnd();
         attrIt != attrItEnd;
         ++attrIt)
    {
        auto& attr = *attrIt;
        const char* attrName = attr.name.GetString();
        const char* kBlockPrefix = "maps_";
        const size_t kBlockPrefixLen = strlen(kBlockPrefix);
        if (!strncmp(attrName, kBlockPrefix, 4))
        {
            Block::Class blockClass = Block::kClass_Count;
            if (!strcmp(attrName+kBlockPrefixLen, "1x1"))
                blockClass = Block::kClass_1x1;
            else if (!strcmp(attrName+kBlockPrefixLen, "3x3"))
                blockClass = Block::kClass_3x3;
            if (blockClass == Block::kClass_Count)
            {
                OVENGINE_LOG_WARN("BlockCollectionLoader.parseModel - '%s' "
                                  "has an invalid class '%s'",
                                  key, attrName);
                continue;
            }
            if (!attr.value.IsArray())
            {
                OVENGINE_LOG_WARN("BlockCollectionLoader.parseModel - '%s'.'%s' "
                                  "has no valid grid definition",
                                  key, attrName);
                continue;
            }
            auto& grid = block.grid(blockClass);
            for (auto rowIt = attr.value.Begin(), rowItEnd = attr.value.End();
                 rowIt != rowItEnd;
                 ++rowIt)
            {
                auto& row = *rowIt;
                if (!row.IsArray())
                {
                    OVENGINE_LOG_WARN("BlockCollectionLoader.parseModel - '%s'.'%s' "
                                      "row is not an array.",
                                      key, attrName);
                    break;
                }
                uint32_t rowIndex = rowIt - attr.value.Begin();
                auto strip = grid.atRow(rowIndex, 0);
                for (auto colIt = row.Begin(), colItEnd = row.End();
                     colIt != colItEnd && strip.first != strip.second;
                     ++colIt)
                {
                    *strip.first = parseUint(*colIt);
                    ++strip.first;
                }
            }
        }
    }
    _models.emplace_back(std::move(block));
    return true;
}

bool BlockCollectionLoader::endCollection()
{
    BlockCollection collection(_name.c_str(), _tilesetName.c_str(), std::move(_models));
    _collectionCb(std::move(collection));
    return true;
}


    }   // namespace ovengine
}   // namespace cinekine