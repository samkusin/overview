//
//  SpriteCollectionLoader.cpp
//  Implementation
//
//  Copyright 2013 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/SpriteCollectionLoader.hpp"

#include "Engine/Model/Sprite.hpp"
#include "Engine/Model/SpriteCollection.hpp"
#include "Engine/Debug.hpp"
#include "Core/StreamBufRapidJson.hpp"
#include "Core/JsonUtilities.hpp"

#include <rapidjson/document.h>


namespace cinek {
    namespace overview {

SpriteCollectionLoader::SpriteCollectionLoader(
        const JsonValue& spriteConsts,
        std::function<cinek_bitmap_atlas(const char*)> atlasReqCb,
        std::function<cinek_bitmap_index(cinek_bitmap_atlas, const char*)> bitmapReqCb,
        std::function<void(SpriteCollection&&)> collectionCb,
        const Allocator& allocator) :
    _spriteConsts(spriteConsts),
    _atlasReqCb(atlasReqCb),
    _bitmapReqCb(bitmapReqCb),
    _collectionCb(collectionCb),
    _atlasId(kCinekBitmapAtlas_Invalid),
    _sprites(allocator)
{
}

bool SpriteCollectionLoader::startCollection(const char* name, uint32_t modelCount)
{
    //  start with a fresh model container
    _sprites.clear();
    _sprites.reserve(modelCount);
    _name = name;
    _atlasId = _atlasReqCb(name);
    return true;
}

bool SpriteCollectionLoader::parseAttribute(const char* key, const JsonValue& value)
{
    //  none
    return true;
}

bool SpriteCollectionLoader::parseModel(const char* key, JsonValue& object)
{
    const rapidjson::Value& stateConsts = _spriteConsts["states"];

    if (!object.HasMember("class"))
    {
        OVENGINE_LOG_ERROR("SpriteCollectionLoader - Sprite entry (%s) has no bitmap class.", key);
        return false;
    }
    if (!object.HasMember("states") || !object["states"].IsObject())
    {
        OVENGINE_LOG_ERROR("SpriteCollectionLoader - Sprite entry (%s) has no states entry.", key);
        return false;
    }

    const rapidjson::Value& states = object["states"];
    uint16_t stateCount = states.MemberEnd() - states.MemberBegin();
    if (!stateCount)
    {
        OVENGINE_LOG_ERROR("SpriteCollectionLoader - Sprite entry (%s) has a zero state count.", key);
        return false;
    }
    // create the template.
    cinek_bitmap_atlas bitmapClass = _atlasReqCb(object["class"].GetString());
    auto& jsonAnchor = object["anchor"];
    glm::ivec2 anchor(jsonAnchor["x"].GetInt(), jsonAnchor["y"].GetInt());
    AABB<Point> aabb;
    if (object.HasMember("aabb"))
    {
        auto& value = object["aabb"];
        aabb.min = parseVec3(value["min"]);
        aabb.max = parseVec3(value["max"]);
    }

    Sprite sprite(key, bitmapClass, anchor, aabb, stateCount);

    //  generate states for the sprite template.
    for (rapidjson::Value::ConstMemberIterator stateIt = states.MemberBegin(), stateItEnd = states.MemberEnd();
         stateIt != stateItEnd;
         ++stateIt)
    {
        // get frame count per state - need this to define animations.
        const char* stateName = stateIt->name.GetString();
        if (!stateConsts.HasMember(stateName))
            continue;

        AnimationStateId animId = stateConsts[stateName].GetUint();
        if (animId == kNullAnimation)
        {
            OVENGINE_LOG_ERROR("SpriteCollectionLoader - Sprite entry (%s) has an invalid state (%s)",
                               key, stateName);
            continue;
        }
        const rapidjson::Value& state = stateIt->value;
        if (!state.IsObject())
        {
            OVENGINE_LOG_ERROR("SpriteCollectionLoader - Sprite state entry (%s/%s) is not an object.",
                               key, stateName);
            continue;
        }
        if (state.HasMember("frames") && state["frames"].IsArray() &&state["frames"].Size())
        {
            const rapidjson::Value& frames = state["frames"];
            uint16_t frameCount = frames.Size();

            uint32_t duration = 0;
            if (state.HasMember("duration"))
            {
                duration = state["duration"].GetUint();
            }
            if (duration == 0)
            {
                duration = 1000;
                OVENGINE_LOG_WARN("SpriteCollectionLoader - Sprite state entry (%s/%s) has no duration.  Defaulting to %u.",
                                  key, stateName, duration);
            }

            //  initialize the state animation.
            SpriteAnimation* animation = sprite.createAnimation(
                                    animId,
                                    frameCount,
                                    duration);
            for (uint16_t frameIndex = 0; frameIndex < frames.Size(); ++frameIndex)
            {
                const rapidjson::Value& frame = frames[frameIndex];
                if (frame.IsString())
                {
                    cinek_bitmap_index bitmapIndex = _bitmapReqCb(bitmapClass, frame.GetString());
                    animation->setFrame(frameIndex, bitmapIndex);
                }
            }
        }
        else
        {
            OVENGINE_LOG_WARN("SpriteCollectionLoader - Sprite state entry (%s/%s) has no frames.",
                              key, stateName);
            continue;
        }
    }

    _sprites.emplace_back(std::move(sprite));
    return true;
}

bool SpriteCollectionLoader::endCollection()
{
    if (_sprites.empty())
        return false;

    SpriteCollection collection(_name.c_str(), std::move(_sprites));
    _collectionCb(std::move(collection));
    return true;
}

    }   // namespace overview
}   // namespace cinek
