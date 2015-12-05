//
//  SceneJsonLoader.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/4/15.
//
//

#ifndef Overview_SceneJsonLoader_hpp
#define Overview_SceneJsonLoader_hpp

#include "Engine/EngineTypes.hpp"

#include <cinek/allocator.hpp>
#include <ckjson/jsontypes.hpp>

namespace cinek {
    namespace ove {
    
/**
 *  Scene Loading
 *
 *  Traverses the Scene tree, constructing entities depending on node type.
 *  Upon finding an 'Entity' node, dispatch control to a separate tree traversal
 *  task.
 *
 */
    
class SceneJsonLoader
{
public:
    SceneJsonLoader(SceneDataContext& context, EntityDatabase& entityDb);
    ~SceneJsonLoader();
    
    unique_ptr<Scene> operator()(JsonValue& root);
    
private:
    SceneDataContext* _context;
    EntityDatabase* _entityDb;
    unique_ptr<Scene> _scene;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_SceneJsonLoader_hpp */
