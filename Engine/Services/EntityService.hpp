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

#include <string>
#include <functional>

namespace cinek { namespace ove {

class EntityService
{
public:
    EntityService(EntityDatabase* context);
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
     *  Destroys the selected entity.
     *
     *  @param  entity      The entity to destroy
     */
    void destroyEntity(Entity entity);
    /**
     *  Clones the selected entity.
     *
     *  @param  context     The cloned entity context
     *  @param  entity      The entity to clone
     *  @return The cloned enity
     */
    Entity cloneEntity(EntityContextType context, Entity entity);
    /**
     *  @param  entity      The entity to check
     *  @return True if the entity has not been destroyed
     */
    bool isEntityValid(Entity e) const;
    /**
     *  Loads template defintions from a file path.  Typically done during
     *  level load or initialization.
     *
     *  @param name         Where to load the definitions from
     *  @param manifest     The manifest object
     */
    void addDefinitions(std::string name, std::shared_ptr<AssetManifest> manifest);
    /**
     *  Clears the specified definitions
     */
    void clearDefinitions(const std::string& name);
    /**
     *  Retrieves the manifest for the given namespace.
     *
     *  @param  ns  The namespace
     *  @return The AssetManifest or null if not found
     */
    const AssetManifest* getDefinitions(const std::string& ns) const;
    /**
     *  Enumerates the added definitions by manifest.  The supplied delegate
     *  is called for every manifest.
     *
     *  @param fn   A delegate invoked for every registered manifest.  Its 
     *              signature should match the declaration:
     *              
     *              void fn(const std::string& ns, AssetManifest& manifest)
     */
    void enumerateDefinitions(const std::function<void(const std::string&, const AssetManifest&)>& cb);
    
    
private:
    EntityDatabase* _context = nullptr;
};


inline EntityService::EntityService
(
    EntityDatabase* context
) :
    _context(context)
{
}
    

} /* namespace ove */ } /* namespace cinek */

#endif /* Overview_Services_Entity_hpp */
