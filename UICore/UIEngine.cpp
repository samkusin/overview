//
//  UIEngine.cpp
//  Overview
//
//  Created by Samir Sinha on 7/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "UIEngine.hpp"

#include "oui.h"

namespace cinek { namespace uicore {

void OUIHandler(int item, UIevent event)
{
    OUIHeader* header = reinterpret_cast<OUIHeader*>(uiGetHandle(item));
    if (!header)
        return;
    
    if (header->handler) {
        header->handler(item, event);
    }
}

void OUIFrame::handler(int item, UIevent event)
{
    OUIFrame* frame = reinterpret_cast<OUIFrame*>(uiGetHandle(item));
    
    if (frame->frameHandler) {
        FrameEvent outEvent;
        outEvent.evtType = event;
        outEvent.item = item;
        outEvent.keymod = uiGetModifier();
        if (event == UI_KEY_DOWN || event == UI_KEY_UP || event == UI_CHAR)
        {
            outEvent.keycode = uiGetKey();
        }
        if (event == UI_SCROLL)
        {
            outEvent.scroll = uiGetScroll();
        }
        else
        {
            outEvent.cursor = uiGetCursor();
        }
        
        outEvent.size.x = uiGetWidth(item);
        outEvent.size.y = uiGetHeight(item);
        
        frame->frameHandler->onUIFrameEvent(frame->id, outEvent);
    }
}

void OUIButtonData::handler(int item, UIevent event)
{
    OUIButtonData* button = reinterpret_cast<OUIButtonData*>(uiGetHandle(item));
    if (button && button->fireHandler) {
        if (event == UI_BUTTON0_HOT_UP) {
            button->fireHandler->onUIButtonHit(button->id);
        }
    }
}

void OUIListBoxData::handler(int item, UIevent event)
{
}

} /* namespace uicore */ } /* namespace cinek */