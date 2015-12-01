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
    _sender(nullptr)
{
}

ViewAPI::ViewAPI
(
    ViewStack& viewStack,
    MessageClientSender& sender,
    EntityDatabase& entityUtil
) :
    _entityProxy(&entityUtil),
    _viewStack(&viewStack),
    _sender(&sender)
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
 

    } /* namespace ove */
} /* namespace cinek */
