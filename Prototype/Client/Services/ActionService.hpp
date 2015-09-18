//
//  ActionService.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Service_Action_hpp
#define Overview_App_Service_Action_hpp

#include "Prototype/Client/AppContext.hpp"
#include "Prototype/Sim/ActionTypes.hpp"

namespace cinek {
    namespace ovproto {

class ActionService
{
public:
    ActionService() = default;
    ActionService(AppContext& context);
    
    ActionHandle addAction(Entity origin, const ActionPhrase& actionPhrase);
    void removeAction(ActionHandle h);
    
private:
    AppObjects* _context = nullptr;
};
    
    } /* namespace ovproto */
} /* namespace cinek */


#endif /* defined(Overview_App_Service_Action_hpp) */
