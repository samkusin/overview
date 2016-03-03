//
//  EntityDatabase.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/11/15.
//
//

#ifndef Overview_entityDatabase_hpp
#define Overview_entityDatabase_hpp

#include "EngineTypes.hpp"
#include "AssetManifest.hpp"

#include <ckentity/entitystore.hpp>
#include <cinek/allocator.hpp>
#include <cinek/task.hpp>
#include <unordered_map>
#include <vector>

namespace cinek {
    namespace ove {

class EntityComponentFactory
{
public:
    virtual ~EntityComponentFactory() {}
    
    virtual void onCustomComponentCreateFn(Entity entity,
                        EntityStore& store,
                        const std::string& templateName,
                        const std::string& componentName,
                        const cinek::JsonValue& definitions,
                        const cinek::JsonValue& compTemplate) = 0;
    
    virtual void onCustomComponentEntityDestroyFn(Entity entity) = 0;
    
    virtual void onCustomComponentEntityCloneFn(Entity target, Entity origin) = 0;
};


/**
 *  @class EntityDatabase
 *  @brief Defines a collection of EntityStore objects mapped to the entity
 *         context.
 *  
 *  The EntityDatabase defines a collection of EntityStore objects.
 *  These stores are mapped to an index value into a std vector.  Of special
 *  note, this index is also the Entity context.  So it's possible to have
 *  various entity stores and maintain unique entity IDs per dictionary.
 *
 *  One requirement for Entity driven applications is efficient and regular
 *  access to entity components.  While it is possible to collect all entities
 *  and their components into a single EntityStore, this approach is less
 *  efficient when dealing with large sets of entities and various components.
 *
 *  For example, out of 100 component tables, entity set "A" may use tables
 *  1-50, while entity set "B" uses tables 51-100 with little to any overlap
 *  (where set "A" uses a scattering of components 51-100 and vice-versa.)
 *
 *  The goal of this approach is to keep these sets together in memory.
 *  Applications should manage what entities go to which stores.  The
 *  EntityDatabase helps applications achieve this goal.
 */

class EntityDatabase
{
    CK_CLASS_NON_COPYABLE(EntityDatabase);
    
public:
    /**
     *  Constructor that initializes the EntityStore objects in the dictionary.
     *
     *  @param stores   List of initializers for each EntityStore.  The size
     *                  of the list determines the number of EntityStore objects
     *                  in the dictionary.  Note that the number of stores is
     *                  also limited to the range of EntityContextType values
     *                  or the Key size (whatever value is smaller.)  If there
     *                  are more initializers than this limit, then initializers
     *                  beyond the limit are ignored.
     */
    EntityDatabase(const std::vector<EntityStore::InitParams>& stores,
        EntityComponentFactory& provider);
    
    EntityDatabase() = default;
    EntityDatabase(EntityDatabase&& other);
    EntityDatabase& operator=(EntityDatabase&& other);
    /** 
     *  @return The number of stores in the dictionary 
     */
    EntityContextType size() const { return _stores.size(); }
    /**
     *  Operation is O(1).
     *  @param index    The key index to lookup.  If index lies outside the
     *                  range of valid stores, returns the default store (#0).
     *  @return         The read-only EntityStore for the given Key
     */
    const EntityStore& getStore(EntityContextType index) const;
    /**
     *  Operation is O(1).
     *  @param index    The key index to lookup.  If index lies outside the
     *                  range of valid stores, returns the default store (#0).
     *  @return         The EntityStore for the given Key
     */
    EntityStore& getStore(EntityContextType index);
    /**
     *  Creates an entity from a definition template.
     *
     *  @param  context The entity's context if applicable
     *  @param  ns      The namespace
     *  @param  templateName    The name of the template
     *  @return The Entity ID created
     */
    Entity createEntity(EntityContextType context, const std::string& ns,
                        const std::string& templateName);
    /**
     *  @param  entity  The entity to check
     *  @return Whether the entity is still valid
     */
    bool isValid(Entity entity) const;
    /**
     *  Duplicates an entity
     *  
     *  @param  context The cloned entity's context
     *  @param  entity  The source entity
     *  @return The cloned entity
     */
    Entity cloneEntity(EntityContextType context, Entity source);
    /**
     *  Components are destroyed during the garbage collection phase.   This
     *  method flags the entity for destruction.
     *
     *  @param  The entity to destroy.
     */
    void destroyEntity(Entity entity);
    /**
     *  Runs garbage collection pass on all EntityStores
     */
    void gc();
    /**
     *  Adds the supplied manifest, mapping it to a namespace.
     *
     *  @param  name        The manifest name
     *  @param  manifest    The manifest object
     */
    void setManifest(std::string name, std::shared_ptr<AssetManifest> manifest);
    /**
     *  @param  name        The manifest to clear
     */
    void clearManifest(const std::string& name);
    /**
     *  Obtains a handle to the manifest identified by name.
     *
     *  @param  name        The name of the manifest to obtain
     *  @return A handle to the manifest
     */
    const AssetManifest* getManifest(const std::string& name) const;
    
private:
    std::vector<EntityStore> _stores;
    std::unordered_map<std::string, std::shared_ptr<AssetManifest>> _manifests;
    EntityComponentFactory* _factory;
};

    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_entityDatabase_hpp */
