/**
 *  @file   IMGUIFrame.cpp
 *
 *  @note   Created by Samir Sinha on 9/12/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "IMGUIFrame.hpp"
#include "IMGUIGfx.hpp"

#include <cstdlib>

namespace cinekine {
    namespace imgui {

///////////////////////////////////////////////////////////////////////////////
    
static void* DefaultAlloc(void* cxt, size_t sz)
{
    return malloc(sz);
}
void DefaultFree(void* cxt, void* p)
{
    free(p);
}
    
Allocator::Allocator() : cxt(NULL), alloc(DefaultAlloc), free(DefaultFree) {}

///////////////////////////////////////////////////////////////////////////////

Frame::Frame(Gfx& gfx, const Rect& rect, const Style& style,
             const Allocator& allocator) :
    _frame(rect),
    _gfx(gfx),
    _styleStack(imgui_allocator<Style>(allocator)),
    _blockWidth(0), _blockHeight(0),
    _nextAreaId(0), _nextWidgetId(0),
    _scrollId(0)
{
    _styleStack.reserve(16);
    _styleStack.push_back(style);
}

void Frame::begin(const Input& input)
{
    const Style& style = _styleStack.back();

    _gfx.setCursor(_frame.x, _frame.y);
    _blockWidth = _frame.w;
    _blockHeight = _frame.h;

    //  draw the background
    if (style.background.color.a)
    {
        _gfx.drawBackground(style, _blockWidth, _blockHeight);
    }


    //  redefine the width and height of frame's working region.
    _blockWidth -= (style.margin.r + style.margin.l);
    _blockHeight -= (style.margin.t + style.margin.b);
    _gfx.moveCursor(style.margin.l, style.margin.t);

    _nextWidgetId = 0;
    _nextAreaId = 0;
    _scrollId = 0;
}

void Frame::end()
{

}

bool Frame::beginScrollArea(const Rect& rect, int32_t scrollPos)
{
    const Style& style = _styleStack.back();

    ++_nextAreaId;
    _nextWidgetId = 0;
    _scrollId = (_nextAreaId << 16) | _nextWidgetId;

    //_scrollTop = rect.y + rect.h - style.padding.b;
    //_scrollBottom = rect.y + style.padding.t;
    //_scrollRight = rect.x + rect.w - style.padding.r;

    //  draw region
    //_gfx.drawBackground(rect, 6, style.background.color);

    //  update cursor
    //_cursorX = rect.x + style.padding.l;
    //_cursorY = rect.y + style.padding.t + scrollPos;
    //_cursorW = rect.w - style.padding.r - style.padding.l;

    return false;
}

int32_t Frame::endScrollArea()
{
    return 0;
}

void Frame::indent()
{

}


void Frame::unindent()
{

}


void Frame::separator()
{

}


void Frame::separatorLine()
{

}


void Frame::label(const char* text)
{

}

    }   // namespace imgui
}   // namespace cinekine
