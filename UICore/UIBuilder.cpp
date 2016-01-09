//
//  UIBuilder.cpp
//  Overview
//
//  Created by Samir Sinha on 7/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "UIBuilder.hpp"
#include "UIEngine.hpp"

#include "oui.h"

namespace cinek { namespace uicore {

/*
    layout.frame(layout_flags).events(UI_BUTTON0_DOWN).size(w,h).
        end();
        
    layout.frame().events(UI_BUTTON0_DOWN, ui_subscriber, ID_FRAME).size(w,h).
        column(UI_TOP|UI_HFILL).margins(10,10,10,10).
            button(BND_ICON_GHOST, "Item 1", ui_subscriber, ID_OK).
            hgroup().margins(5,5,5,5).
                radio(BND_ICON_REC, "Radio 1", &radio1).
                radio(BND_ICON_REC, "Radio 2", &radio1).
                radio(BND_ICON_REC, "Radio 3", &radio1).
            end().
        end().
        row().
            ...
        end().
    end();
 
*/

Layout::Layout() :
    _size(0),
    _topItem(-1),
    _mode(kFreeLayout)
{
}

Layout::~Layout()
{
    if ((_size & 0x80000000))
    {
        vector<int>* vec = reinterpret_cast<vector<int>*>(_data);
        vec->~vector();
        _size = 0;
    }
}

void Layout::pushTop()
{
    if (_topItem < 0)
        return;
    
    if (!(_size & 0x80000000) && _size >= kMaxItemsInData)
    {
        int32_t* src = reinterpret_cast<int32_t*>(_data);
        vector<int> items;
        items.reserve(kMaxItemsInData*2);

        for (int i = 0; i < _size; ++i)
        {
            items.push_back(src[i]);
        }
    
        vector<int>& target = *::new(_data) vector<int>();
        target = std::move(items);
        _size |= 0x80000000;
    }
    
    if ((_size & 0x80000000))
    {
        vector<int>& vec = *reinterpret_cast<vector<int>*>(_data);
        vec.push_back(_topItem);
    }
    else
    {
        int32_t* items = reinterpret_cast<int32_t*>(_data);
        items[_size++] = _topItem;
    }
}

int Layout::popItem()
{
    int32_t item = -1;
    if ((_size & 0x80000000))
    {
        vector<int>& vec = *reinterpret_cast<vector<int>*>(_data);
        if (!vec.empty())
        {
            item = vec.back();
            vec.pop_back();
        }
    }
    else if (_size > 0)
    {
        int32_t* items = reinterpret_cast<int32_t*>(_data);
        item = items[--_size];
    }
    
    return item;
}

void Layout::insertItem(int32_t item, int32_t parent)
{
    uiInsert(parent, item);

    uint32_t flags = uiGetLayout(item);
    
    switch (_mode)
    {
    case kColumnLayout:
        {
            flags &= ~(UI_VFILL);
            flags |= UI_HFILL;
        }
        break;
    default:
        break;
    }
    
    uiSetLayout(item, flags);
}

Layout& Layout::frame
(
    int id,
    unsigned int eventFlags,
    FrameHandler* frameHandler,
    RenderCallback renderCb,
    void* context,
    unsigned int layoutFlags
)
{
    pushTop();
    
    _topItem = uiItem();
    _mode = kFreeLayout;
    
    OUIFrame* data = reinterpret_cast<OUIFrame*>(
        uiAllocHandle(_topItem, sizeof(OUIFrame))
    );
    data->header.itemType = OUIItemType::frame;
    data->header.handler = OUIFrame::handler;
    data->id = id;
    data->frameHandler = frameHandler;
    data->renderCb = renderCb;
    data->callbackContext = context;
    
    if (layoutFlags) {
        uiSetBox(_topItem, UI_LAYOUT);
        uiSetLayout(_topItem, layoutFlags);
    }
    else {
        uiSetBox(_topItem, UI_ROW);
    }

    uiSetEvents(_topItem, eventFlags);
    uiSetSize(_topItem, 0, 0);
    
    return *this;
}

Layout& Layout::column(unsigned int layoutFlags)
{
    pushTop();
    
    _topItem = uiItem();
    _mode = kColumnLayout;
    
    OUIHeader* header = reinterpret_cast<OUIHeader*>(
        uiAllocHandle(_topItem, sizeof(OUIHeader))
    );
    header->itemType = OUIItemType::column;
    header->handler = nullptr;
    
    uiSetBox(_topItem, UI_COLUMN);
    uiSetLayout(_topItem, layoutFlags);
    
    return *this;
}

Layout& Layout::setSize(int w, int h)
{
    uiSetSize(_topItem, w, h);
    return *this;
}

Layout& Layout::button
(
    int id,
    ButtonHandler* btnHandler,
    int iconId,
    const char* label
)
{
    int item = uiItem();
    OUIButtonData* data = reinterpret_cast<OUIButtonData*>(
        uiAllocHandle(item, sizeof(OUIButtonData))
    );
    data->header.itemType = OUIItemType::button;
    data->header.handler = OUIButtonData::handler;
    data->id = id;
    data->fireHandler = btnHandler;
    data->iconId = iconId;
    data->label = label;
 
    uiSetSize(item, 0, UITHEME_WIDGET_HEIGHT);
    uiSetEvents(item, UI_BUTTON0_HOT_UP);
    
    insertItem(item, _topItem);
    
    return *this;
}

Layout& Layout::listbox
(
    DataProvider* dataProvider,
    int id,
    ListboxLayout layout,
    int* selected
)
{
    int item = uiItem();
    OUIListBoxData* data = reinterpret_cast<OUIListBoxData*>(
        uiAllocHandle(item, sizeof(OUIListBoxData))
    );
    data->header.itemType = OUIItemType::listbox;
    data->header.handler = OUIListBoxData::handler;
    data->provider = dataProvider;
    data->providerId = id;
    data->layout = layout;
    data->anchorIndex = -1;
    data->selected = selected;
    
    //  default height of two items
    uiSetLayout(item, UI_FILL);
    // for selection and scroll
    uiSetEvents(item, UI_BUTTON0_DOWN | UI_BUTTON0_CAPTURE);
    
    insertItem(item, _topItem);
    
    return *this;
}

Layout& Layout::end()
{
    int parent = popItem();
    if (parent >= 0)
    {
        const OUIHeader* hdr = reinterpret_cast<const OUIHeader*>(uiGetHandle(parent));
        switch (hdr->itemType)
        {
        case OUIItemType::frame:        _mode = kFreeLayout;            break;
        case OUIItemType::column:       _mode = kColumnLayout;          break;
        default:
            break;
        }

        insertItem(_topItem, parent);
    
        _topItem = parent;
        
    }
    return *this;
}

} /* namespace uicore */ } /* namespace cinek */