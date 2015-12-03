//
//  SceneService.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/02/15.
//
//

#ifndef Overview_Services_Scene_hpp
#define Overview_Services_Scene_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/Messages/Scene.hpp"

namespace cinek { namespace ove {

template<typename SceneType>
class SceneService
{
public:    
    SceneService(SceneType& context, MessageClientSender& sender);
    SceneService() = default;

private:
    SceneType* _context = nullptr;
    MessageClientSender* _sender = nullptr;
};

} /* namespace ove */ } /* namespace cinek */

#endif /* Overview_Services_Entity_hpp */
