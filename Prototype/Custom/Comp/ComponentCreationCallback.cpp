//
//  ComponentCreationCallback.cpp
//  Overview
//
//  Created by Samir Sinha on 7/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "ComponentCreationCallback.hpp"
#include "AppContext.hpp"

#include "Prototype/Client/Services/EntityService.hpp"
#include "Prototype/Client/Services/RenderService.hpp"

#include "Loadout.hpp"
#include "Party.hpp"
#include "Character.hpp"
#include "RigidBody.hpp"

#include "Engine/Utils/JsonParseExtensions.hpp"

#include <ckjson/json.hpp>
#include <cinek/debug.hpp>

namespace cinek {
    namespace ovproto {
    
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
    //RenderService renderService(render);
    
    if (!strcmp(componentName, "rigid_body"))
    {
        auto comp = entityService.table<RigidBodyComponent>().addDataToEntity(entity);
        auto mass = data.HasMember("mass") ? data["mass"].GetDouble() : 1.0;
        auto inertia = data.HasMember("inertia") ? overview::parseVec3(data["inertia"]) : ckm::vec3(1,1,1);
        comp->init(mass, inertia.x, inertia.y, inertia.z);
    }
    else if (!strcmp(componentName, "loadout"))
    {
        const char* type = data.HasMember("type") ? data["type"].GetString() : "";
        if (type[0])
        {
            auto data = entityService.table<LoadoutComponent>().addDataToEntity(entity);
            if (!strcmp(type, "player"))
            {
                data->type = LoadoutComponent::Type::kPlayer;
                entityService.entityGroupTable(LoadoutComponent::kGroupId_Player).
                    addDataToEntity(entity);
            }
            else if (!strcmp(type, "small_starship"))
            {
                data->type = LoadoutComponent::Type::kSmallShip;
                entityService.entityGroupTable(LoadoutComponent::kGroupId_SmallShip).
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
            auto data = entityService.table<PartyComponent>().addDataToEntity(entity);
            if (!strcmp(type, "player"))
            {
                data->type = PartyComponent::Type::kPlayer;
                entityService.entityGroupTable(PartyComponent::kGroupId_Player).
                    addDataToEntity(entity);
            }
            else if (!strcmp(type, "small_starship"))
            {
                data->type = PartyComponent::Type::kSmallShip;
                entityService.entityGroupTable(PartyComponent::kGroupId_SmallShip).
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
        entityService.table<CharacterComponent>().addDataToEntity(entity);
    }
}

void customComponentDestroyCb
(
    AppContext context,
    RenderContext render,
    EntityDataTable& table,
    ComponentRowIndex rowIndex
)
{
    EntityService entityService(context);
    //RenderService renderService(render);
    
    switch (table.id())
    {
    case kLoadoutComponent:
        {
            auto data = table.rowset().at<LoadoutComponent>(rowIndex);
            auto entity = table.rowset().entityAt(rowIndex);
            if (data)
            {
                switch(data->type)
                {
                case LoadoutComponent::Type::kPlayer:
                    entityService.entityGroupTable(LoadoutComponent::kGroupId_Player).
                        removeDataFromEntity(entity);
                    break;
                case LoadoutComponent::Type::kSmallShip:
                    entityService.entityGroupTable(LoadoutComponent::kGroupId_SmallShip).
                        removeDataFromEntity(entity);
                    break;
                default:
                    break;
                };
            }
        }
        break;
    case kPartyComponent:
        {
            auto data = table.rowset().at<PartyComponent>(rowIndex);
            auto entity = table.rowset().entityAt(rowIndex);
            if (data)
            {
                switch(data->type)
                {
                case PartyComponent::Type::kPlayer:
                    entityService.entityGroupTable(PartyComponent::kGroupId_Player).
                        removeDataFromEntity(entity);
                    break;
                case PartyComponent::Type::kSmallShip:
                    entityService.entityGroupTable(PartyComponent::kGroupId_SmallShip).
                        removeDataFromEntity(entity);
                    break;
                default:
                    break;
                };
            }
        }
        break;
    default:
        break;
    };
}
        

    } /* namespace ovproto */
} /* namespace cinek */