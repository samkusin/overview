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

static const Layout::Style kDefaultButtonStyle = {
    Layout::Style::has_layout,
    UI_HFILL,
};

static const Layout::Style kDefaultListboxStyle = {
    Layout::Style::has_layout,
    UI_FILL,
};

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
    _topLayout(0)
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

Layout& Layout::beginFrame
(
    unsigned int eventFlags,
    FrameState* frameState,
    RenderCallback renderCb,
    void* context
)
{
    pushTop();
 
    _topItem = createFrameLayout(eventFlags, frameState, renderCb, context);
    _topLayout = UI_FILL;   // to be set or altered upon end()
    
    return *this;
}

Layout& Layout::beginWindow()
{
    pushTop();
     
    _topItem = uiItem();
    _topLayout = UI_FILL;  // to be set or altered upon end()
    
    uiSetBox(_topItem, UI_COLUMN);
    
    OUIHeader* header = reinterpret_cast<OUIHeader*>(
        uiAllocHandle(_topItem, sizeof(OUIHeader))
    );
    header->itemType = OUIItemType::window;
    header->handler = nullptr;
    
    return *this;
}

Layout& Layout::beginColumn()
{
    pushTop();
    
    _topItem = uiItem();
    _topLayout = UI_VFILL;  // to be set or altered upon end()
    
    uiSetBox(_topItem, UI_COLUMN);
    
    OUIHeader* header = reinterpret_cast<OUIHeader*>(
        uiAllocHandle(_topItem, sizeof(OUIHeader))
    );
    header->itemType = OUIItemType::column;
    header->handler = nullptr;

    return *this;
}

Layout& Layout::beginRow()
{
    pushTop();
    
    _topItem = uiItem();
    _topLayout = UI_HFILL;  // to be set or altered upon end()
    
    uiSetBox(_topItem, UI_ROW);
    
    OUIHeader* header = reinterpret_cast<OUIHeader*>(
        uiAllocHandle(_topItem, sizeof(OUIHeader))
    );
    header->itemType = OUIItemType::column;
    header->handler = nullptr;

    return *this;
}

Layout& Layout::setSize(int w, int h)
{
    uiSetSize(_topItem, w, h);
    return *this;
}

Layout& Layout::setLayout(unsigned int layout)
{
    _topLayout = layout;
    return *this;
}

Layout& Layout::setMargins(Box box)
{
    uiSetMargins(_topItem, box.l, box.t, box.r, box.b);
    return *this;
}

Layout& Layout::button
(
    int id,
    ButtonHandler* btnHandler,
    int iconId,
    const char* label,
    const Style* style
)
{
    int item = uiItem();

    applyStyleToItem(item, style, &kDefaultButtonStyle);

    uiSetSize(item, 0, UITHEME_WIDGET_HEIGHT);
    
    //  for firing callback
    uiSetEvents(item, UI_BUTTON0_HOT_UP);

    OUIButtonData* data = reinterpret_cast<OUIButtonData*>(
        uiAllocHandle(item, sizeof(OUIButtonData))
    );
    data->header.itemType = OUIItemType::button;
    data->header.handler = OUIButtonData::handler;
    data->id = id;
    data->fireHandler = btnHandler;
    data->iconId = iconId;
    data->label = label;
 
    uiInsert(_topItem, item);
    
    return *this;
}

Layout& Layout::listbox
(
    DataProvider* dataProvider,
    int id,
    ListboxType lbtype,
    ListboxState* state,
    const Style* style
)
{
    int item = uiItem();
    
    applyStyleToItem(item, style, &kDefaultListboxStyle);
    
    // for selection and scroll
    uiSetEvents(item, UI_BUTTON0_DOWN | UI_BUTTON0_CAPTURE);

    OUIListBoxData* data = reinterpret_cast<OUIListBoxData*>(
        uiAllocHandle(item, sizeof(OUIListBoxData))
    );
    data->header.itemType = OUIItemType::listbox;
    data->header.handler = OUIListBoxData::handler;
    data->provider = dataProvider;
    data->providerId = id;
    data->lbtype = lbtype;
    data->viewAnchor = { 0, 0 };
    data->state = state;
    data->state->init(item);
    
    uiInsert(_topItem, item);
    
    return *this;
}

Layout& Layout::end()
{
    CK_ASSERT(_topItem >= 0);
    
    int child = _topItem;
    
    uiSetLayout(_topItem, _topLayout);
    
    popItem();
    
    if (_topItem >= 0)
    {
        /*
        const OUIHeader* hdr = reinterpret_cast<const OUIHeader*>(uiGetHandle(_topItem));
        switch (hdr->itemType)
        {
        case OUIItemType::frame:
            break;
        case OUIItemType::column:
            break;
        default:
            break;
        }
        */
        uiInsert(_topItem, child);
    }
    else {
        uiInsert(uicore::getLayoutRootItem(), child);
    }
    return *this;
}

void Layout::applyStyleToItem
(
    int item,
    const Style* style,
    const Style* defaultStyle
)
{
    if (!style || !(style->mask & Style::has_layout)) {
        if (defaultStyle) {
            applyStyleToItem(item, defaultStyle, nullptr);
        }
    }
    else {
        uiSetLayout(item, style->layout);
    }
    
    if (!style || !(style->mask & Style::has_margins)) {
        if (defaultStyle) {
            applyStyleToItem(item, defaultStyle, nullptr);
        }
    }
    else {
        uiSetMargins(item,
            style->margins.l, style->margins.t,
            style->margins.r, style->margins.b);
    }
}

void Layout::pushTop()
{
    if (_topItem < 0)
        return;
    
    if (!(_size & 0x80000000) && _size >= kMaxItemsInData)
    {
        State* src = reinterpret_cast<State*>(_data);
        vector<State> items;
        items.reserve(kMaxItemsInData*2);

        for (int i = 0; i < _size; ++i)
        {
            items.push_back(src[i]);
        }
    
        vector<State>& target = *::new(_data) vector<State>();
        target = std::move(items);
        _size |= 0x80000000;
    }
    
    if ((_size & 0x80000000))
    {
        vector<State>& vec = *reinterpret_cast<vector<State>*>(_data);
        vec.push_back({ _topItem, _topLayout });
    }
    else
    {
        State* items = reinterpret_cast<State*>(_data);
        items[_size++] = { _topItem, _topLayout };
    }
}

void Layout::popItem()
{
    State item { -1, 0 };
    
    if ((_size & 0x80000000))
    {
        vector<State>& vec = *reinterpret_cast<vector<State>*>(_data);
        if (!vec.empty())
        {
            item = vec.back();
            vec.pop_back();
        }
    }
    else if (_size > 0)
    {
        State* items = reinterpret_cast<State*>(_data);
        item = items[--_size];
    }
    
    _topItem = item.item;
    _topLayout = item.layout;
}

} /* namespace uicore */ } /* namespace cinek */