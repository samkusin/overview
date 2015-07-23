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

namespace cinek { namespace ovproto {

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

UILayout::UILayout() :
    _size(0),
    _topItem(-1),
    _mode(kFreeLayout)
{
}

UILayout::~UILayout()
{
    if ((_size & 0x80000000))
    {
        vector<int>* vec = reinterpret_cast<vector<int>*>(_data);
        vec->~vector();
        _size = 0;
    }
}

void UILayout::pushTop()
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

int32_t UILayout::popItem()
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

void UILayout::insertItem(int32_t item, int32_t parent)
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

UILayout& UILayout::frame(uint32_t layoutFlags)
{
    pushTop();
    
    _topItem = uiItem();
    _mode = kFreeLayout;
    
    OUIHeader* header = reinterpret_cast<OUIHeader*>(
        uiAllocHandle(_topItem, sizeof(OUIHeader))
    );
    header->init(OUIItemType::frame);
    if (layoutFlags)
    {
        uiSetLayout(_topItem, layoutFlags);
    }
    uiSetBox(_topItem, UI_COLUMN);
    
    return *this;
}

UILayout& UILayout::column(uint32_t layoutFlags)
{
    pushTop();
    
    _topItem = uiItem();
    _mode = kColumnLayout;
    
    OUIHeader* header = reinterpret_cast<OUIHeader*>(
        uiAllocHandle(_topItem, sizeof(OUIHeader))
    );
    header->init(OUIItemType::column);
    
    uiSetBox(_topItem, UI_COLUMN);
    if (layoutFlags)
    {
        uiSetLayout(_topItem, layoutFlags);
    }
    
    return *this;
}

UILayout& UILayout::events
(
    uint32_t flags,
    UISubscriber* subscriber,
    int32_t evtId
)
{
    OUIHeader* header = reinterpret_cast<OUIHeader*>(uiGetHandle(_topItem));
    header->subscriber = subscriber;
    header->itemId = evtId;
    
    uiSetEvents(_topItem, flags);
    
    return *this;
}

UILayout& UILayout::size(int32_t w, int32_t h)
{
    uiSetSize(_topItem, w, h);
    return *this;
}

UILayout& UILayout::button
(
    int32_t iconId,
    const char* label,
    UISubscriber* subscriber,
    int32_t evtId
)
{
    int item = uiItem();
    OUIButtonData* data = reinterpret_cast<OUIButtonData*>(
        uiAllocHandle(item, sizeof(OUIButtonData))
    );
    data->header.init(OUIItemType::button);
    if (subscriber)
    {
        data->header.subscriber = subscriber;
        data->header.itemId = evtId;
    }
    data->iconId = iconId;
    data->label = label;
 
    uiSetSize(item, 0, UITHEME_WIDGET_HEIGHT);
    uiSetEvents(item, UI_BUTTON0_HOT_UP);
    
    insertItem(item, _topItem);
    
    return *this;
}

UILayout& UILayout::end()
{
    int32_t parent = popItem();
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

} /* namespace ovproto */ } /* namespace cinek */