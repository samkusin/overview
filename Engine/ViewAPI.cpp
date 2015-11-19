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
    _entityUtility(nullptr),
    _viewStack(nullptr)
{
}

ViewAPI::ViewAPI
(
    EntityUtility& entityUtil,
    ViewStack& viewStack
) :
    _entityUtility(&entityUtil),
    _viewStack(&viewStack)
{
}

EntityService ViewAPI::entityService() const
{
    return EntityService(_entityUtility);
}

ViewService ViewAPI::viewService(ViewController& controller) const
{
    return ViewService(_viewStack, &controller);
}
 

    } /* namespace ove */
} /* namespace cinek */
