//
//  SpriteCollectionLoader.cpp
//  Implementation
//
//  Copyright 2013 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Sprite/SpriteCollectionLoader.hpp"
#include "Engine/Sprite/Sprite.hpp"
#include "Engine/Sprite/SpriteCollection.hpp"
#include "Engine/Debug.hpp"

#include "CKGfx/BitmapLibrary.hpp"
#include "CKGfx/BitmapAtlas.hpp"

#include <cinek/json/json.hpp>


namespace cinek {
    namespace overview {

SpriteCollectionLoader::SpriteCollectionLoader
(
    const gfx::BitmapLibrary& bitmapLibrary,
    const JsonValue& spriteConsts,
    const RequestAtlasCb& loadAtlasCb,
    const CollectionLoadedCb& collectionLoadedCb,
    const Allocator& allocator
):
    _bitmapLibrary(&bitmapLibrary),
    _spriteConsts(&spriteConsts),
    _reqAtlasCb(loadAtlasCb),
    _collectionLoadedCb(collectionLoadedCb),
    _sprites(allocator)
{
}

bool SpriteCollectionLoader::startCollection(const char* name, uint32_t modelCount)
{
    //  start with a fresh model container
    _sprites.clear();
    _sprites.reserve(modelCount);
    _name = name;
    return true;
}

bool SpriteCollectionLoader::parseAttribute(const char* key, const JsonValue& value)
{
    //  none
    return true;
}

bool SpriteCollectionLoader::parseModel(const char* key, JsonValue& object)
{
    const rapidjson::Value& stateConsts = (*_spriteConsts)["states"];

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
    auto& jsonAnchor = object["anchor"];
    glm::ivec2 anchor(jsonAnchor["x"].GetInt(), jsonAnchor["y"].GetInt());
    AABB<Point> aabb;
    if (object.HasMember("aabb"))
    {
        auto& value = object["aabb"];
        aabb.min = parseVec3(value["min"]);
        aabb.max = parseVec3(value["max"]);
    }

    auto bitmapClass = _reqAtlasCb(object["class"].GetString());
    auto bitmapAtlas = _bitmapLibrary->atlas(bitmapClass);
    if (!bitmapAtlas)
        return false;
    
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
                if (frame.IsString() && bitmapClass)
                {
                    auto bitmapIndex = bitmapAtlas->bitmapIndexFromName(frame.GetString());
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
    _collectionLoadedCb(std::move(collection));
    
    return true;
}

    }   // namespace overview
}   // namespace cinek
