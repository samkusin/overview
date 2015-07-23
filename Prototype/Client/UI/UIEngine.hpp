//
//  UIEngine.hpp
//  Overview
//
//  Created by Samir Sinha on 7/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_UI_Engine_hpp
#define Overview_UI_Engine_hpp

#include "UITypes.hpp"

#include <cinek/vector.hpp>

namespace cinek { namespace ovproto {

extern void OUIHandler(int item, UIevent evt);

enum class OUIItemType
{
    frame,
    column,
    button
};

struct OUIHeader
{
    OUIItemType itemType;
    UISubscriber* subscriber;
    int itemId;
    
    void init(OUIItemType type);
};

struct OUIButtonData
{
    OUIHeader header;
    int iconId;
    const char* label;
};


} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_UI_Engine_hpp) */
