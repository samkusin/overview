//
//  ComponentCreationCallback.cpp
//  Overview
//
//  Created by Samir Sinha on 7/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "ComponentCreationCallback.hpp"
#include "AppContext.hpp"

#include "Client/Services/EntityService.hpp"
#include "Client/Services/RenderService.hpp"

#include "Loadout.hpp"
#include "Party.hpp"
#include "Character.hpp"
#include "RigidBody.hpp"

#include "Engine/Utils/JsonParseExtensions.hpp"

#include <cinek/json/json.hpp>
#include <cinek/debug.hpp>

namespace cinek {
    namespace ovproto {
        namespace component {
    
void customComponentCreateCb
(
    AppContext context,
    RenderContext render,
    Entity entity,
    const cinek::JsonValue& definitions,
    const char* componentName,
    const cinek::JsonValue& data
)
{
    EntityService entityService(context);
    RenderService renderService(render);
    
    if (!strcmp(componentName, "rigid_body"))
    {
        auto comp = entityService.table<component::RigidBody>().addDataToEntity(entity);
        auto mass = data.HasMember("mass") ? data["mass"].GetDouble() : 1.0;
        auto inertia = data.HasMember("inertia") ? overview::parseVec3(data["inertia"]) : ckm::vec3(1,1,1);
        comp->init(mass, inertia.x, inertia.y, inertia.z);
    }
    else if (!strcmp(componentName, "loadout"))
    {
        const char* type = data.HasMember("type") ? data["type"].GetString() : "";
        if (type[0])
        {
            auto data = entityService.table<component::Loadout>().addDataToEntity(entity);
            if (!strcmp(type, "player"))
            {
                data->type = component::Loadout::Type::kPlayer;
                entityService.entityGroupTable(component::Loadout::kGroupId_Player).
                    addDataToEntity(entity);
            }
            else if (!strcmp(type, "small_starship"))
            {
                data->type = component::Loadout::Type::kSmallShip;
                entityService.entityGroupTable(component::Loadout::kGroupId_SmallShip).
                    addDataToEntity(entity);
            }
            else
            {
                //  unsupported loadout type
                CK_ASSERT(false);
            }
        }
    }
    else if (!strcmp(componentName, "party"))
    {
        const char* type = data.HasMember("type") ? data["type"].GetString() : "";
        if (type[0])
        {
            auto data = entityService.table<component::Party>().addDataToEntity(entity);
            if (!strcmp(type, "player"))
            {
                data->type = component::Party::Type::kPlayer;
                entityService.entityGroupTable(component::Party::kGroupId_Player).
                    addDataToEntity(entity);
            }
            else if (!strcmp(type, "small_starship"))
            {
                data->type = component::Party::Type::kSmallShip;
                entityService.entityGroupTable(component::Party::kGroupId_SmallShip).
                    addDataToEntity(entity);
            }
            else
            {
                //  unsupported loadout type
                CK_ASSERT(false);
            }
        }
    }
    else if (!strcmp(componentName, "character"))
    {
        entityService.table<component::Character>().addDataToEntity(entity);
    }
}

void customComponentDestroyCb
(
    AppContext context,
    RenderContext render,
    Entity entity,
    overview::ComponentId componentId
)
{
    EntityService entityService(context);
    RenderService renderService(render);
    
    switch (componentId)
    {
    case component::kLoadout:
        {
            auto table = entityService.table<component::Loadout>();
            auto data = table.dataForEntity(entity);
            switch(data->type)
            {
            case component::Loadout::Type::kPlayer:
                entityService.entityGroupTable(component::Loadout::kGroupId_Player).
                    removeDataFromEntity(entity);
                break;
            case component::Loadout::Type::kSmallShip:
                entityService.entityGroupTable(component::Loadout::kGroupId_SmallShip).
                    removeDataFromEntity(entity);
                break;
            default:
                break;
            };
        }
        break;
    case component::kParty:
        {
            auto table = entityService.table<component::Party>();
            auto data = table.dataForEntity(entity);
            switch(data->type)
            {
            case component::Party::Type::kPlayer:
                entityService.entityGroupTable(component::Party::kGroupId_Player).
                    removeDataFromEntity(entity);
                break;
            case component::Party::Type::kSmallShip:
                entityService.entityGroupTable(component::Party::kGroupId_SmallShip).
                    removeDataFromEntity(entity);
                break;
            default:
                break;
            };
        }
        break;
    default:
        break;
    };
}
        

        } /* namespace component */
    } /* namespace ovproto */
} /* namespace cinek */