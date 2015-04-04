//
//  SpriteCollection.cpp
//  Implementation
//
//  Copyright 2013 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Sprite/SpriteCollection.hpp"
#include "Engine/Sprite/Sprite.hpp"

namespace cinek {
    namespace overview {

SpriteCollection::SpriteCollection(const char* name, vector<Sprite>&& sprites) :
    ModelCollection<Sprite>(name, std::move(sprites))
{
    //  Indices reference items the _models vector.  Indices are guaranteed not
    //  to change for the lifespan of this SpriteCollection
    std::sort(_models.begin(), _models.end(),
        [](const Type& model1, const Type& model2) -> bool {
            return model1.name() < model2.name();
        });
}

const Sprite& SpriteCollection::find(const std::string& templateName) const
{
    auto it = std::lower_bound(_models.begin(), _models.end(), templateName,
        [](const Type& model, const std::string& name) -> bool {
            return model.name() < name;
        });
    if (it == _models.end() || it->name() != templateName)
        return _nullSprite;

    return *it;
}

int SpriteCollection::index(const std::string& templateName) const
{
    auto it = std::lower_bound(_models.begin(), _models.end(), templateName,
        [](const Type& model, const std::string& name) -> bool {
            return model.name() < name;
        });
    if (it == _models.end() || it->name() != templateName)
        return -1;

    return (int)(it - _models.begin());
}

    }   // namespace overview
}   //  namespace cinek