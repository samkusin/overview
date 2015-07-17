//
//  RenderInterface.hpp
//  Overview
//
//  Created by Samir Sinha on 7/7/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_RenderInterface_hpp
#define Overview_App_RenderInterface_hpp

#include "AppContext.hpp"
#include "Client/Render/RenderDefines.hpp"

#include <cinek/json/json.hpp>

namespace cinek {
    namespace ovproto {

class RenderInterface
{
public:
    RenderInterface(AppInterface& context);
    
    //  register an object list handler executed before any rendering is
    //  performed
    void registerObjectListHandler(uint32_t viewIndex,
        const overview::BuildRenderObjectListCb& cb);

    //  remove an object list handler
    void unregisterObjectListHandler(uint32_t viewIndex);
    
    
private:
    AppContext* _context;
};
    
    } /* namespace ovproto */
} /* namespace cinek */

#endif