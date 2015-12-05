//
//  ViewAPI.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/19/15.
//
//

#ifndef Overview_ViewAPI_hpp
#define Overview_ViewAPI_hpp

#include "EngineTypes.hpp"

#include "Services/ViewService.hpp"
#include "Services/EntityService.hpp"
#include "Services/SceneService.hpp"

namespace cinek {
    namespace ove {
    
class ViewAPI
{
public:
    ViewAPI();
    ViewAPI(ViewStack& viewStack,
        MessageClientSender& client,
        EntityDatabase& entityUtil,
        Scene& scene);
    
    EntityService entityService() const;
    ViewService viewService(ViewController& controller) const;
    SceneService sceneService() const;
    
private:
    EntityDatabase* _entityProxy;
    ViewStack* _viewStack;
    MessageClientSender* _sender;
    Scene* _scene;
};


    } /* namespace ove */
} /* namespace cinek */


#endif /* ViewAPI_hpp */
