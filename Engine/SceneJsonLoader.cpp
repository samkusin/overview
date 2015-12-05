//
//  SceneJsonLoader.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/4/15.
//
//

#include "Scenes/Scene.hpp"
#include "SceneJsonLoader.hpp"
#include "SceneDataContext.hpp"

#include "Engine/EntityDatabase.hpp"

#include <ckjson/json.hpp>

namespace cinek {
    namespace ove {
    
SceneJsonLoader::SceneJsonLoader
(
    SceneDataContext& context,
    EntityDatabase& entityDb
) :
    _context(&context),
    _entityDb(&entityDb)
{
}

SceneJsonLoader::~SceneJsonLoader() = default;
    
unique_ptr<Scene> SceneJsonLoader::operator()
(
    JsonValue& root
)
{
    //  generate scene objects on demand, with the 'entity' as the customer
    //  this means that our loader's primary task is to create entities
    //  in turn, each entity's task is to create the supporting objects it
    //  requires.
    
    
    //  tree-traversal then happens in two stages
    //
    
    
    
    return std::move(_scene);
}
    
    } /* namespace ove */
} /* namespace cinek */
