//
//  SceneService.inl
//  EnginePrototype
//
//  Created by Samir Sinha on 12/02/15.
//
//

#include "SceneService.hpp"
#include "Engine/Scenes/Scene.hpp"

namespace cinek {
    namespace ove {

template<typename SceneType>
SceneService<SceneType>::SceneService
(
    SceneType& context,
    MessageClientSender& sender
) :
    _context(&context),
    _sender(&sender)
{
}

    } /* namespace ove */
} /* namespace cinek */
