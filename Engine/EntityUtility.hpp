//
//  EntityFactory.hpp
//  Overview
//
//  Created by Samir Sinha on 5/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Utility_hpp
#define Overview_Entity_Utility_hpp

#include "EntityTypes.hpp"

#include <ckjson/json.hpp>
#include <vector>

namespace cinek { namespace ove {

/**
 *  @class EntityUtility
 *  @brief Utility class to create entities via template definition.
 *
 *  This is the preferred method of creating entities when using the Overview
 *  Engine.  If an application desires exact control over creating entities,
 *  use the createEntity stanalone function instead.  One example of this is
 *  if an application doesn't want the small overhead of EntityStoreDictionary.
 *  If an application needs even finer control, it should resort to direct 
 *  entity creation via the EntityStore and explicit component creation by
 *  application-specific methods.
 */
class EntityUtility
{
    CK_CLASS_NON_COPYABLE(EntityUtility);
    
public:
    EntityUtility(EntityStoreDictionary& entityStoreDict,
                  CustomComponentCreateFn customCreateFn);
    
    const EntityStore& getStore(EntityContextType context=0) const;
    EntityStore& getStore(EntityContextType context=0);

    Entity createEntity(EntityContextType context, const char* templateNs,
                        const char* templateName);
    
    void destroyEntity(Entity entity);
    
    void addDefinitions(const char* ns, const JsonValue& definitions);
    void resetDefinitions();
    
private:
    EntityStoreDictionary* _entityStoreDictionary;
    CustomComponentCreateFn _customCreateFn;
    JsonDocument _entityDefinitions;
};

/**
 *  Optional method of creating an entity, bypassing the EntityUtility (this
 *  function is called by EntityUtility.)
 */
Entity createEntity
(
    EntityContextType context,
    EntityStore& store,
    const cinek::JsonValue& definitions,
    const char* name,
    const CustomComponentCreateFn& customCompFn
);

  
} /* namespace ove */ } /* namespace cinek */

#endif /* defined(Overview_Entity_Factory_hpp) */
