/**
 * @file    SpriteDatabaseLoader.cpp
 *
 * Container for SpriteTemplates.
 *
 * @note    Created by Samir Sinha on 7/5/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "SpriteDatabaseLoader.hpp"
#include "Engine/Debug.hpp"
#include "Framework/StreamBufRapidJson.hpp"
#include "rapidjson/document.h"

#include "cinek/rendermodel/spritetemplate.hpp"
#include "cinek/rendermodel/spritedatabase.hpp"


namespace cinekine {
    namespace ovengine {

SpriteDatabaseLoader::SpriteDatabaseLoader(rendermodel::SpriteDatabase& database) :
    _db(database)
{

}

bool SpriteDatabaseLoader::unserialize(std::streambuf& instream)
{
    typedef rapidjson::GenericDocument<rapidjson::UTF8<> > Document;
    typedef rapidjson::GenericValue<rapidjson::UTF8<> > Value;

    RapidJsonStdStreamBuf jsonStream(instream);

    Document jsonDoc;
    jsonDoc.ParseStream<0>(jsonStream);

    if (jsonDoc.HasParseError() || !jsonDoc.IsObject())
    {
        OVENGINE_LOG_ERROR("SpriteDatabaseJSONSerializer - failed to parse.");
        return false;
    }

    //  add or append to the states table.
    if (jsonDoc.HasMember("states"))
    {
        if (!jsonDoc["states"].IsObject())
        {
            OVENGINE_LOG_ERROR("SpriteDatabaseJSONSerializer - Invalid states entry in sprite database (not an object.)");
            return false;
        }

        const Value& states = jsonDoc["states"];
        for (Value::ConstMemberIterator it = states.MemberBegin(), itEnd = states.MemberEnd();
             it != itEnd;
             ++it)
        {
            const Value::Member& member = *it;
            if (!member.value.IsInt())
            {
                OVENGINE_LOG_WARN("SpriteDatabaseJSONSerializer - Skipping animation state definition (%s). Value not an integer.",
                              member.name.GetString());
                continue;
            }
            cinek_rendermodel_anim_id animId = member.value.GetInt();
            if (!_db.mapAnimationStateNameToId(member.name.GetString(), animId))
            {
                OVENGINE_LOG_WARN("SpriteDatabaseJSONSerializer - Skipping animation state definition (%s). Duplicate ID value found (%u).",
                              member.name.GetString(), animId);
                continue;
            }
        }
    }

    //  add or append to the sprites table.

    if (jsonDoc.HasMember("sprites") )
    {
        if (!jsonDoc["sprites"].IsObject())
        {
            OVENGINE_LOG_ERROR("SpriteDatabaseJSONSerializer - Invalid sprites entry in sprite database (not an object.)");
            return false;
        }

        const Value& sprites = jsonDoc["sprites"];
        for (Value::ConstMemberIterator it = sprites.MemberBegin(), itEnd = sprites.MemberEnd();
             it != itEnd;
             ++it)
        {
            const Value::Member& member = *it;
            const char* name = member.name.GetString();
            if (!member.value.IsObject())
            {
                OVENGINE_LOG_ERROR("SpriteDatabaseJSONSerializer - Sprite entry (%s) is not an object.", name);
                continue;
            }
            const Value& sprite = member.value;
            if (!sprite.HasMember("class"))
            {
                OVENGINE_LOG_ERROR("SpriteDatabaseJSONSerializer - Sprite entry (%s) has no bitmap class.", name);
                continue;
            }
            if (!sprite.HasMember("states") || !sprite["states"].IsObject())
            {
                OVENGINE_LOG_ERROR("SpriteDatabaseJSONSerializer - Sprite entry (%s) has no states entry.", name);
                continue;
            }

            const Value& states = sprite["states"];
            uint16_t stateCount = states.MemberEnd() - states.MemberBegin();
            if (!stateCount)
            {
                OVENGINE_LOG_ERROR("SpriteDatabaseJSONSerializer - Sprite entry (%s) has a zero state count.", name);
                continue;
            }

            // create the template.
            cinek_bitmap_atlas bitmapClass = _atlasRequest(sprite["class"].GetString());
            rendermodel::SpriteTemplate* spriteTemplate = _db.createOrModifyTemplateFromName(name, bitmapClass, stateCount);
            if (spriteTemplate == nullptr)
            {
                OVENGINE_LOG_ERROR("SpriteDatabaseJSONSerializer - Sprite entry (%s) failed to create template object.", name);
                continue;
            }

            //  generate states for the sprite template.
            for (Value::ConstMemberIterator stateIt = states.MemberBegin(), stateItEnd = states.MemberEnd();
                 stateIt != stateItEnd;
                 ++stateIt)
            {
                // get frame count per state - need this to define animations.
                const char* stateName = stateIt->name.GetString();
                cinek_rendermodel_anim_id animId = _db.findAnimationIDByName(stateName);
                if (animId == kCinekAnimation_Null)
                {
                    OVENGINE_LOG_ERROR("SpriteDatabaseJSONSerializer - Sprite entry (%s) has an invalid state (%s)",
                                       name, stateName);
                    continue;
                }
                const Value& state = stateIt->value;
                if (!state.IsObject())
                {
                    OVENGINE_LOG_ERROR("SpriteDatabaseJSONSerializer - Sprite state entry (%s/%s) is not an object.",
                                       name, stateName);
                    continue;
                }
                if (state.HasMember("frames") && state["frames"].IsArray() &&state["frames"].Size())
                {
                    const Value& frames = state["frames"];
                    uint16_t frameCount = frames.Size();

                    uint32_t duration;
                    if (state.HasMember("duration"))
                    {
                        duration = state["duration"].GetUint();
                    }
                    else
                    {
                        duration = 1000;
                        OVENGINE_LOG_WARN("SpriteDatabaseJSONSerializer - Sprite state entry (%s/%s) has no duration.  Defaulting to %u.",
                                          name, stateName, duration);
                    }

                    //  initialize the state animation.
                    rendermodel::SpriteAnimation* animation = spriteTemplate->createAnimation(
                                            animId,
                                            frameCount,
                                            duration);
                    for (uint16_t frameIndex = 0; frameIndex < frames.Size(); ++frameIndex)
                    {
                        const Value& frame = frames[frameIndex];
                        if (frame.IsString())
                        {
                            cinek_bitmap_index bitmapIndex = _indexRequest(bitmapClass, frame.GetString());
                            animation->setFrame(frameIndex, bitmapIndex);
                        }
                    }
                }
                else
                {
                    OVENGINE_LOG_WARN("SpriteDatabaseJSONSerializer - Sprite state entry (%s/%s) has no frames.",
                                      name, stateName);
                    continue;
                }
            }

        }
    }

    return true;
}


    }   // namespace ovengine
}   // namespace cinekine
