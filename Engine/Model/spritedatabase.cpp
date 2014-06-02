/**
 * @file    rendermodel/spritetemplate.cpp
 *
 * Sprite related data shared across multiple sprite instances.
 *
 * @note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "SpriteDatabase.hpp"
#include "Sprite.hpp"

namespace cinekine {
    namespace ovengine {

SpriteDatabase::SpriteDatabase(size_t initTemplateLimit, const Allocator& allocator) :
    _allocator(allocator),
    _templatePool(initTemplateLimit, allocator),
    _nameToAnimIds(std_allocator<std::pair<string, AnimationStateId>>(allocator)),
    _nameToTemplates(std_allocator<std::pair<string, Sprite*>>(allocator))
{
}

const Sprite* SpriteDatabase::spriteByName(const string& templateName) const
{
    auto it = _nameToTemplates.find(templateName);
    if (it == _nameToTemplates.end())
        return nullptr;
    return it->second;
}

AnimationStateId SpriteDatabase::animationIDByName(const char* animationName) const
{
    auto it = _nameToAnimIds.find(animationName);
    if (it == _nameToAnimIds.end())
        return kNullAnimation;
    return it->second;
}

Sprite* SpriteDatabase::createOrModifyFromName(const char* name,
        cinek_bitmap_atlas classId,
        const glm::ivec2& anchor,
        uint16_t numStates)
{
    //  check if a sprite of that name exists, if so use that sprite's id,
    //  and replace the old template with a new one.
    //  otherwise just create a new one.
    string templateName(name);
    auto itTemplate = _nameToTemplates.find(templateName);
    Sprite* theTemplate;
    if (itTemplate != _nameToTemplates.end())
    {
        theTemplate = itTemplate->second;
        theTemplate->~Sprite();
        new(theTemplate) Sprite(classId, anchor, numStates, _allocator);
    }
    else
    {
        theTemplate = _templatePool.allocateAndConstruct(classId, anchor, numStates, _allocator);
        _nameToTemplates.emplace(templateName, theTemplate);
    }
    return theTemplate;
}

bool SpriteDatabase::mapAnimationStateNameToId(const char* name,
        AnimationStateId id)
{
    for(const auto& val : _nameToAnimIds)
    {
        if (val.second == id)
            return false;
    }
    _nameToAnimIds[string(name)] = id;
    return true;
}


    }   // namespace ovengine
}   //  namespace cinekine