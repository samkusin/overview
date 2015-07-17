//
//  UISystem.hpp
//  Overview
//
//  Created by Samir Sinha on 7/16/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_UI_System_hpp
#define Overview_UI_System_hpp

#include <cinek/allocator.hpp>

namespace cinek {

namespace overview {
    class Renderer;
}

namespace ovproto {

class UISystem
{
public:
    UISystem(uint32_t viewId, overview::Renderer& renderer);
    ~UISystem();
    
    void layout();
    
private:
    class Impl;
    unique_ptr<Impl> _impl;
};

} /* namesspace ovproto */

} /* namespace cinek */

#endif /* defined(Overview_UI_System_hpp) */
