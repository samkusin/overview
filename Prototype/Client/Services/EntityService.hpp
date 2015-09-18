//
//  EntityService.hpp
//  Overview
//
//  Created by Samir Sinha on 7/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Service_Entity_hpp
#define Overview_App_Service_Entity_hpp

#include "Prototype/Client/AppContext.hpp"
#include "Engine/Entity/EntityTypes.hpp"

#include <ckentity/entitydatatable.hpp>
#include <ckentity/entitygroupmap.hpp>
#include <ckentity/entitystore.hpp>

namespace cinek {
    namespace ovproto {

class EntityService
{
public:
    EntityService() = default;
    EntityService(AppContext& context);
    
    //  Allocates an entity with no components
    //
    Entity create(Entity context=0);
    
    //  Creates an entity using a template, with the specified context.
    //  The context is stored within the Entity ID and can be used for filtering
    //
    Entity create(const char* templateName, Entity context=0);
    
    //  Destroys the entity
    //
    void destroy(Entity entity);

    //  Returns a Component Table
    //
    template<typename _Component>
    component::Table<_Component> table() const;
    
    //  Returns an EntityGroup Table
    EntityGroupTable entityGroupTable(EntityGroupMapId id) const;
    
    EntityComponentDestroyFn entityComponentDestroyFn() const;
    
private:
    AppObjects* _context = nullptr;
};

template<typename _Component>
component::Table<_Component> EntityService::table() const
{
    return _context->entityStore->table<_Component>();
}

inline auto EntityService::entityGroupTable(EntityGroupMapId id) const
    -> EntityGroupTable
{
    return _context->entityStore->entityGroupTable(id);
}

    
    } /* namespace ovproto */
} /* namespace cinek */


#endif /* defined(Overview_App_Service_Entity_hpp) */
