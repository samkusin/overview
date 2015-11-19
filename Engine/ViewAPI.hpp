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

namespace cinek {
    namespace ove {

class ViewAPI
{
public:
    ViewAPI();
    ViewAPI(EntityUtility& entityUtil, ViewStack& viewStack);
    
    EntityService entityService() const;
    ViewService viewService(ViewController& controller) const;
    
private:
    EntityUtility* _entityUtility;
    ViewStack* _viewStack;
};


    } /* namespace ove */
} /* namespace cinek */


#endif /* ViewAPI_hpp */
