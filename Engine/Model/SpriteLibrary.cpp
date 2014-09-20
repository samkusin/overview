//
//  SpriteLibrary.cpp
//  Implementation
//
//  Copyright 2013 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/SpriteLibrary.hpp"
#include "Engine/Model/Sprite.hpp"

namespace cinekine {
    namespace ovengine {

SpriteLibrary::SpriteLibrary(size_t initTemplateLimit, const Allocator& allocator) :
    _allocator(allocator),
    _templatePool(initTemplateLimit, allocator),
    _nameToAnimIds(std_allocator<std::pair<string, AnimationStateId>>(allocator)),
    _nameToTemplates(std_allocator<std::pair<string, Sprite*>>(allocator))
{
}

const Sprite& SpriteLibrary::spriteByName(const string& templateName) const
{
    auto it = _nameToTemplates.find(templateName);
    if (it == _nameToTemplates.end())
        return emptySprite();
    return *it->second;
}

AnimationStateId SpriteLibrary::animationIDByName(const char* animationName) const
{
    auto it = _nameToAnimIds.find(animationName);
    if (it == _nameToAnimIds.end())
        return kNullAnimation;
    return it->second;
}

Sprite* SpriteLibrary::createOrModifyFromName(const char* name,
        cinek_bitmap_atlas classId,
        const glm::ivec2& anchor,
        const AABB<Point>& aabb,
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
        new(theTemplate) Sprite(classId, anchor, aabb, numStates, _allocator);
    }
    else
    {
        theTemplate = _templatePool.allocateAndConstruct(classId, anchor, aabb,
                                                         numStates,
                                                         _allocator);
        _nameToTemplates.emplace(templateName, theTemplate);
    }
    return theTemplate;
}

bool SpriteLibrary::mapAnimationStateNameToId(const char* name,
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