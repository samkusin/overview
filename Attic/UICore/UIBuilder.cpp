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

#include <cinek/debug.h>

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

Layout::Layout(Context* context) :
    _context(context),
    _size(0),
    _topItem(-1),
    _innerItem(-1),
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
 
    _topItem = createFrame(_context, eventFlags, frameState, renderCb, context);
    _topLayout = UI_FILL;   // to be set or altered upon end()
    
    _innerItem = _topItem;
    
    return *this;
}

Layout& Layout::beginWindow(const char* title)
{
    pushTop();
    
    //  frame
    _topItem = uiItem();
    _topLayout = UI_LEFT | UI_TOP;
    
    uiSetBox(_topItem, UI_LAYOUT);
    
    OUIWindow* window = reinterpret_cast<OUIWindow*>(
        uiAllocHandle(_topItem, sizeof(OUIWindow))
    );
    window->header.itemType = OUIItemType::window;
    window->header.handler = nullptr;
    if (title && *title) {
        window->title = _context->stringStack.create(title);
    }
    else {
        window->title = nullptr;
    }
    //  inner contents of the window excluding the title bar
    _innerItem = uiItem();
    uiSetBox(_innerItem, UI_COLUMN);
    uiSetLayout(_innerItem, UI_FILL);
    uiSetMargins(_innerItem, 0, _context->theme.windowTitleBarHeight, 0, 0);
    
    OUIHeader* inner = reinterpret_cast<OUIHeader*>(
        uiAllocHandle(_innerItem, sizeof(OUIHeader))
    );
    inner->itemType = OUIItemType::group;
    inner->handler = nullptr;
    
    uiInsert(_topItem, _innerItem);
    
    return *this;
}

Layout& Layout::beginRow()
{
    pushTop();
    
    _topItem = uiItem();
    _topLayout = UI_FILL;  // to be set or altered upon end()
    
    uiSetBox(_topItem, UI_ROW);
    
    OUIHeader* header = reinterpret_cast<OUIHeader*>(
        uiAllocHandle(_topItem, sizeof(OUIHeader))
    );
    header->itemType = OUIItemType::column;
    header->handler = nullptr;

    _innerItem = _topItem;

    return *this;
}

Layout& Layout::beginColumn()
{
    pushTop();
    
    _topItem = uiItem();
    _topLayout = UI_FILL;  // to be set or altered upon end()
    
    uiSetBox(_topItem, UI_COLUMN);
    
    OUIHeader* header = reinterpret_cast<OUIHeader*>(
        uiAllocHandle(_topItem, sizeof(OUIHeader))
    );
    header->itemType = OUIItemType::column;
    header->handler = nullptr;

    _innerItem = _topItem;

    return *this;
}

Layout& Layout::beginStack
(
    const char* title,
    bool* state
)
{
    pushTop();
    
    //  frame
    _topItem = uiItem();
    _topLayout = UI_HFILL;
    
    uiSetBox(_topItem, UI_COLUMN);
    uiSetMargins(_topItem, _context->theme.widgetMargins.l,
        _context->theme.widgetMargins.t,
        _context->theme.widgetMargins.r,
        _context->theme.widgetMargins.b);
    
    OUIStack* stack = reinterpret_cast<OUIStack*>(
        uiAllocHandle(_topItem, sizeof(OUIStack))
    );
    stack->header.itemType = OUIItemType::stack;
    stack->header.handler = nullptr;
    if (title && *title) {
        stack->title = _context->stringStack.create(title);
    }
    else {
        stack->title = nullptr;
    }
    stack->state = state;
    
    //  inner contents of the window excluding the title bar
    _innerItem = uiItem();
    uiSetBox(_innerItem, UI_COLUMN);
    uiSetLayout(_innerItem, UI_TOP | UI_HFILL);
    uiSetMargins(_innerItem, 0, _context->theme.widgetHeight, 0, 0);
    
    OUIHeader* inner = reinterpret_cast<OUIHeader*>(
        uiAllocHandle(_innerItem, sizeof(OUIHeader))
    );
    inner->itemType = OUIItemType::group;
    inner->handler = nullptr;
    
    uiInsert(_topItem, _innerItem);
    
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

Layout& Layout::setMargins(int l, int t, int r, int b)
{
    uiSetMargins(_topItem, l, t, r, b);
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

    uiSetLayout(item, UI_HFILL);
    uiSetSize(item, 0, _context->theme.widgetHeight);
    uiSetMargins(item, _context->theme.widgetMargins.l,
        _context->theme.widgetMargins.t,
        _context->theme.widgetMargins.r,
        _context->theme.widgetMargins.b);
    
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
    data->label = _context->stringStack.create(label);
 
    uiInsert(_innerItem, item);
    
    return *this;
}

Layout& Layout::listbox
(
    DataProvider* dataProvider,
    int id,
    ListboxType lbtype,
    ListboxState* state
)
{
    int item = uiItem();
 
    uiSetLayout(item, UI_FILL);
    uiSetMargins(item, _context->theme.widgetMargins.l,
        _context->theme.widgetMargins.t,
        _context->theme.widgetMargins.r,
        _context->theme.widgetMargins.b);
    
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
    
    uiInsert(_innerItem, item);
    
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
        uiInsert(_context->rootItem, child);
    }
    
    return *this;
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
        vec.push_back({ _topItem, _innerItem, _topLayout });
    }
    else
    {
        State* items = reinterpret_cast<State*>(_data);
        items[_size++] = { _topItem, _innerItem, _topLayout };
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
    _innerItem = item.innerItem;
    _topLayout = item.layout;
}

} /* namespace uicore */ } /* namespace cinek */