//
//  EntityStoreDictionary.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/11/15.
//
//

#ifndef Overview_EntityStoreDictionary_hpp
#define Overview_EntityStoreDictionary_hpp

#include "EntityTypes.hpp"

#include <ckentity/entitystore.hpp>
#include <vector>

namespace cinek {
    namespace ove {

/**
 *  @class EntityStoreDictionary
 *  @brief Defines a collection of EntityStore objects mapped to the entity
 *         context.
 *  
 *  The EntityStoreDictionary defines a collection of EntityStore objects.
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
 *  EntityStoreDictionary helps applications achieve this goal.
 */
class EntityStoreDictionary
{
    CK_CLASS_NON_COPYABLE(EntityStoreDictionary);
    
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
    EntityStoreDictionary(const std::vector<EntityStore::InitParams>& stores);
    
    EntityStoreDictionary() = default;
    EntityStoreDictionary(EntityStoreDictionary&& other);
    EntityStoreDictionary& operator=(EntityStoreDictionary&& other);
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
     *  Runs garbage collection pass on all EntityStores
     */
    void gc();
    
private:
    std::vector<EntityStore> _stores;
};

    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_EntityStoreDictionary_hpp */
