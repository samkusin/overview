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
    
    if (frame->state) {
        frame->state->thisItem = item;
    
        switch (event) {
        
        case UI_KEY_DOWN:
        case UI_KEY_UP:
            if (frame->state->keyEventCount < frame->state->keyEventLimit) {
                KeyEvent& keyevt = frame->state->keyEvents[frame->state->keyEventCount];
                keyevt.type = event;
                keyevt.key = uiGetKey();
                keyevt.mod = uiGetModifier();
                ++frame->state->keyEventCount;
            }
            break;
        default:
            frame->state->evtType = event;
            break;
        }

        
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
    OUIListBoxData* lb = reinterpret_cast<OUIListBoxData*>(uiGetHandle(item));
    if (lb && lb->state) {
        ListboxState* state = lb->state;
        if (event == UI_BUTTON0_DOWN) {
            if (state->hoverItem >= 0) {
                state->highlightItem = state->hoverItem;
            }
            if (uiGetClicks() > 1 && state->highlightItem == state->hoverItem) {
                state->selectedItem = state->highlightItem;
            }
        }
    }
}

} /* namespace uicore */ } /* namespace cinek */