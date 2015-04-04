//
//  EntityTemplateCollection.cpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Engine/Entity/EntityTemplateCollection.hpp"

namespace cinek { namespace overview {

    EntityTemplateCollection::EntityTemplateCollection(const char* name,
                                                       vector<EntityTemplate>&& templates) :
        ModelCollection<EntityTemplate>(name, std::move(templates))
    {
    }


    auto EntityTemplateCollection::operator[](const std::string& name) const -> const Type&
    {
        for (auto& entityTempl : _models)
        {
            if (entityTempl.name() == name)
                return entityTempl;
        }
        return _nullEntity;
    }

} /* namespace overview */ } /* namespace cinek */
