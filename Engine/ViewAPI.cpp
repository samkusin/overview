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

ViewAPI::ViewAPI() :
    _entityProxy(nullptr),
    _viewStack(nullptr),
    _sender(nullptr),
    _scene(nullptr)
{
}

ViewAPI::ViewAPI
(
    ViewStack& viewStack,
    MessageClientSender& sender,
    EntityDatabase& entityUtil,
    Scene& scene
) :
    _entityProxy(&entityUtil),
    _viewStack(&viewStack),
    _sender(&sender),
    _scene(&scene)
{
}

EntityService ViewAPI::entityService() const
{
    return EntityService(*_entityProxy, *_sender);
}

ViewService ViewAPI::viewService(ViewController& controller) const
{
    return ViewService(*_viewStack, controller);
}

auto ViewAPI::sceneService() const -> SceneService
{
    return SceneService(*_scene, *_sender);
}
 

    } /* namespace ove */
} /* namespace cinek */
