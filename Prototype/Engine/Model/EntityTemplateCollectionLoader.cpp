//
//  EntityTemplateCollectionLoader.cpp
//  Implementation
//
//  Copyright 2013 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/EntityTemplateCollectionLoader.hpp"

#include "Engine/Debug.hpp"
#include "Core/StreamBufRapidJson.hpp"
#include "Core/JsonUtilities.hpp"

#include <cstring>
#include <cstdlib>

namespace cinek {
    namespace overview {

EntityTemplateCollectionLoader::EntityTemplateCollectionLoader(
        const JsonValue& tileFlagConsts,
        std::function<void(EntityTemplateCollection&&)> collectionCb,
        const Allocator& allocator) :
    _tileFlagConsts(tileFlagConsts),
    _collectionCb(collectionCb),
    _templates(allocator)
{
}

bool EntityTemplateCollectionLoader::startCollection(const char* name, uint32_t modelCount)
{
    //  start with a fresh model container
    _name = name;
    _templates.reserve(modelCount);

    return true;
}

bool EntityTemplateCollectionLoader::parseAttribute(const char* key, const JsonValue& value)
{
    //  none
    return true;
}

bool EntityTemplateCollectionLoader::parseModel(const char* key, JsonValue& object)
{
    std::string spriteName;
    std::string controllerName;
    for (auto attrIt = object.MemberBegin(), attrItEnd = object.MemberEnd();
         attrIt != attrItEnd;
         ++attrIt)
    {
        auto& attr = *attrIt;
        const char* attrName = attr.name.GetString();
        if (!strcmp(attrName, "sprite"))
        {
            spriteName = attr.value.GetString();
        }
        if (!strcmp(attrName, "controller"))
        {
            controllerName = attr.value.GetString();
        }
    }

    EntityTemplate et(key, spriteName, controllerName);
    _templates.emplace_back(std::move(et));
    return true;
}

bool EntityTemplateCollectionLoader::endCollection()
{
    //  convert our tiles vecrtor into a single sparse vector with capacity
    //  equal to the maximum tile index
    std::sort(_templates.begin(), _templates.end(),
              [](const EntityTemplate& l, const EntityTemplate& r) -> bool {
                return l.name() < r.name();
              });

    EntityTemplateCollection collection(_name.c_str(), std::move(_templates));
    _collectionCb(std::move(collection));
    return true;
}

    }   // namespace overview
}   // namespace cinek
