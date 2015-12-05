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

#include <functional>
#include <string>

namespace cinek { namespace ove {

class SceneService
{
public:    
    SceneService(Scene& context, MessageClientSender& sender);
    SceneService() = default;
    
    void loadScene(const std::string& name,
        std::function<void(const SceneLoadResponse&)> cb);

private:
    Scene* _context = nullptr;
    MessageClientSender* _sender = nullptr;
};

} /* namespace ove */ } /* namespace cinek */

#endif /* Overview_Services_Entity_hpp */
