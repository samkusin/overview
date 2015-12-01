//
//  EntityService.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/11/15.
//
//

#ifndef Overview_Services_Entity_hpp
#define Overview_Services_Entity_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/EntityTypes.hpp"
#include "Engine/Messages/Entity.hpp"

#include <ckentity/entitygroupmap.hpp>
#include <ckentity/entitydatatable.hpp>

namespace cinek { namespace ove {

class EntityService
{
public:
    EntityService(EntityDatabase& context, MessageClientSender& sender);
    EntityService() = default;

    /**
     *  Creates an entity using the given template.
     *
     *  @param  storeId      The context (store) used
     *  @param  templateNs   The tempalte definition namespace
     *  @param  templateName The template definition name
     *  @return A valid Entity or 0 if the template does not exist
     */
    Entity createEntity(EntityContextType storeId, const std::string& ns,
                        const std::string& templateName);
    /**
     *  Retrieve a component table by explicit type Component.
     *
     *  @param  storeId      The context (store) used
     */
    template<typename Component> component::Table<Component> getTable(EntityContextType storeId);
    /**
     *  Returns an EntityGroup table given its ID.
     *  
     *  @param  storeId      The context (store) used
     *  @param  id           The EntityGroupTable's ID for lookup
     */
    EntityGroupTable entityGroupTable(EntityContextType storeId, EntityGroupMapId id) const;
    /**
     *  @param  entity       The entity to check
     *  @return True if the entity has not been destroyed
     */
    bool isEntityValid(Entity e) const;
    /**
     *  Loads template defintions from a file path.  Typically done during
     *  level load or initialization.
     *
     *  @param name Where to load the definitions from
     *  @param cb   The callback executed on completion with result 
     */
    void loadDefinitions(const std::string& name,
                         std::function<void(const EntityLoadDefinitionsResponse&)> cb);
    /**
     *  Clears the specified definitions
     */
    void clearDefinitions(const std::string& name);
    /**
     *  Creates a component by explicit type that is attached to the specified
     *  entity.
     */
    template<typename Component> Component* createData(Entity entity);
    /** 
     *  Returns whether the entity has the specified component
     *  @param  entity      The entity
     *  @param  compId      The component Id
     *  @return True if the entity possesses the component
     */
    bool hasComponent(Entity entity, ComponentId compId) const;
    /**
     *  Returns a (const) component by explicit type attached to the specified
     *  entity.
     *  @param  entity      The entity.
     *  @return A const pointer to the component data or nullptr if not found.
     */
    template<typename Component> const Component* getData(Entity entity) const;
    /**
     *  Returns a component by explicit type attached to the specified entity.
     *  @param  entity      The entity.
     *  @return A  pointer to the component data or nullptr if not found.
     */
    template<typename Component> Component* getData(Entity entity);
    
private:
    EntityDatabase* _context = nullptr;
    MessageClientSender* _sender = nullptr;
};


inline EntityService::EntityService
(
    EntityDatabase& context,
    MessageClientSender& sender
) :
    _context(&context),
    _sender(&sender)
{
}
    

} /* namespace ove */ } /* namespace cinek */

#endif /* Overview_Services_Entity_hpp */
