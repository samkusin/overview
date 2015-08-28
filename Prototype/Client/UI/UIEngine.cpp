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
        UIeventdata data;
        data.item = item;
        data.keymod = uiGetModifier();
        if (event == UI_KEY_DOWN || event == UI_KEY_UP || event == UI_CHAR)
        {
            data.keycode = uiGetKey();
        }
        if (event == UI_SCROLL)
        {
            data.scroll = uiGetScroll();
        }
        else
        {
            data.cursor = uiGetCursor();
        }
        header->subscriber->onUIEvent(header->itemId, event, data);
    }
}

void OUIHeader::init(OUIItemType type)
{
    itemType = type;
    itemId = -1;
    subscriber = nullptr;
}

} /* namespace ovproto */ } /* namespace cinek */