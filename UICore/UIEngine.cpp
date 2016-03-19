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

static int sUIRootItem = -1;

void initLayout(int w, int h, UIContext* context)
{
    uiBeginLayout();

    context->frameState.keyEventLimit =
        sizeof(context->frameKeyEvents) / sizeof(context->frameKeyEvents[0]);
    context->frameState.keyEvents = context->frameKeyEvents;
        
    //  item 0 is our root UI item in which all UI is based
    int rootItem = createFrameLayout(UI_BUTTON0_DOWN | UI_KEY_DOWN,
        &context->frameState,
        nullptr,
        nullptr);
    
    uiSetSize(rootItem, w, h);
    
    //  sets the key focus to the master frame by default
    //  child layouts may override this item
    context->keyFocusItem = rootItem;
    
    sUIRootItem = rootItem;
}

int getLayoutRootItem()
{
    return sUIRootItem;
}

void finalizeLayout(UIContext* context)
{
    uiSetLayout(sUIRootItem, UI_FILL);
    
    uiEndLayout();
    
    if (context->keyFocusItem >= 0) {
        uiFocus(context->keyFocusItem);
    }
}

int createFrameLayout
(
    unsigned int eventFlags,
    FrameState* frameState,
    RenderCallback renderCb,
    void* context
)
{
    int item = uiItem();
    
    uiSetBox(item, UI_LAYOUT);
    uiSetEvents(item, eventFlags);
    
    OUIFrame* data = reinterpret_cast<OUIFrame*>(
        uiAllocHandle(item, sizeof(OUIFrame))
    );
    data->header.itemType = OUIItemType::frame;
    data->header.handler = OUIFrame::handler;
    data->renderCb = renderCb;
    data->callbackContext = context;
    data->state = frameState;
    data->state->init(item);
    
    return item;
}

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