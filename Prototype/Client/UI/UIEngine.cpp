//
//  UIEngine.cpp
//  Overview
//
//  Created by Samir Sinha on 7/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "UIEngine.hpp"

#include "oui.h"

namespace cinek { namespace ovproto {

void OUIHandler(int item, UIevent event)
{
    OUIHeader* header = reinterpret_cast<OUIHeader*>(uiGetHandle(item));
    if (!header)
        return;
    
    switch (header->itemType)
    {
    default:
        break;
    }
    
    if (header->subscriber)
    {
        header->subscriber->onUIEvent(header->itemId, event);
    }
}

void OUIHeader::init(OUIItemType type)
{
    itemType = type;
    itemId = -1;
    subscriber = nullptr;
}

} /* namespace ovproto */ } /* namespace cinek */