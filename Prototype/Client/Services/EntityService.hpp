//
//  EntityService.hpp
//  Overview
//
//  Created by Samir Sinha on 7/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Service_Entity_hpp
#define Overview_App_Service_Entity_hpp

#include "Client/AppContext.hpp"
#include "Engine/Entity/EntityTypes.hpp"
#include "Engine/Entity/EntityDataTable.hpp"
#include "Engine/Entity/EntityGroupMap.hpp"
#include "Engine/Entity/EntityStore.hpp"

namespace cinek {
    namespace ovproto {

class EntityService
{
public:
    EntityService() = default;
    EntityService(AppContext& context);
    
    //  Allocates an entity with no components
    //
    Entity create(Entity::context_type context=0);
    
    //  Creates an entity using a template, with the specified context.
    //  The context is stored within the Entity ID and can be used for filtering
    //
    Entity create(const char* templateName, Entity::context_type context=0);
    
    //  Destroys the entity
    //
    void destroy(Entity entity);

    //  Returns a Component Table
    //
    template<typename _Component>
    overview::component::Table<_Component> table() const;
    
    //  Returns an EntityGroup Table
    overview::component::EntityGroupTable entityGroupTable(overview::EntityGroupMapId id) const;
    
    //  Sets the Custom Component generation handler
    //
    void setCreateComponentCallback(const overview::CustomComponentCreateFn& cb);

    //  Sets the Custom Component destruction handler
    void setDestroyComponentCallback(const overview::CustomComponentDestroyFn& cb);
    
private:
    AppObjects* _context = nullptr;
};

template<typename _Component>
overview::component::Table<_Component> EntityService::table() const
{
    return _context->entityStore->table<_Component>();
}

inline auto EntityService::entityGroupTable(overview::EntityGroupMapId id) const
    -> overview::component::EntityGroupTable
{
    return _context->entityStore->entityGroupTable(id);
}

    
    } /* namespace ovproto */
} /* namespace cinek */


#endif /* defined(Overview_App_Service_Entity_hpp) */
