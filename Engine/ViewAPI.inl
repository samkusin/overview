//
//  ViewAPI.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/19/15.
//
//

#include "ViewAPI.hpp"

namespace cinek {
    namespace ove {

template<typename SceneType, typename SceneService>
ViewAPI<SceneType, SceneService>::ViewAPI() :
    _entityProxy(nullptr),
    _viewStack(nullptr),
    _sender(nullptr),
    _sceneType(nullptr)
{
}

template<typename SceneType, typename SceneService>
ViewAPI<SceneType, SceneService>::ViewAPI
(
    ViewStack& viewStack,
    MessageClientSender& sender,
    EntityDatabase& entityUtil,
    SceneType& scene
) :
    _entityProxy(&entityUtil),
    _viewStack(&viewStack),
    _sender(&sender),
    _sceneType(&scene)
{
}

template<typename SceneType, typename SceneService>
EntityService ViewAPI<SceneType, SceneService>::entityService() const
{
    return EntityService(*_entityProxy, *_sender);
}

template<typename SceneType, typename SceneService>
ViewService ViewAPI<SceneType, SceneService>::viewService(ViewController& controller) const
{
    return ViewService(*_viewStack, controller);
}

template<typename SceneType, typename SceneService>
auto ViewAPI<SceneType, SceneService>::sceneService() const -> SceneService
{
    return SceneService(*_sceneType, *_sender);
}
 

    } /* namespace ove */
} /* namespace cinek */
