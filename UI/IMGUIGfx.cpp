/**
 *  @file   IUIGfx.cpp
 *
 *  @note   Created by Samir Sinha on 9/29/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "IMGUIGfx.hpp"

namespace cinekine {
    namespace imgui {

Gfx::Gfx(size_t queueSize, size_t stringBufferSize,
         const Allocator& allocator) :
    _allocator(allocator),
    _cmds(imgui_allocator<DrawCommand>(allocator)),
    _strings(_allocator, stringBufferSize),
    _cursorX(0), _cursorY(0)
{
    _cmds.reserve(queueSize);
}

void Gfx::clearCommandQueue()
{
    _cmds.clear();
}

void Gfx::setCursor(int32_t x, int32_t y)
{
    _cursorX = x;
    _cursorY = y;
}

void Gfx::moveCursor(int32_t xOff, int32_t yOff)
{
    _cursorX += xOff;
    _cursorY += yOff;
}

void Gfx::drawBackground(const Style& style, int32_t w, int32_t h)
{
    //  draw background
    int32_t x = _cursorX + style.margin.l;
    int32_t y = _cursorY + style.margin.t;
    int32_t w2 = w - style.margin.r - style.margin.l;
    int32_t h2 = h - style.margin.t - style.margin.b;

    if (style.background.color.a)
    {
        _cmds.emplace_back(DrawCommand::kFillRect, kDrawSolid, style.background.color);
        _cmds.back().rect.r = { x, y, w2, h2 };
        _cmds.back().rect.cornerRadius = style.border.radius;
    }
    //  draw border
    if (style.hasBorder())
    {
        _cmds.emplace_back(DrawCommand::kRect);
        _cmds.back().styles = style.border.styles;
        _cmds.back().colors = style.border.colors;
        _cmds.back().widths = style.border.widths;
        _cmds.back().rect.r = { x, y, w2, h2 };
        _cmds.back().rect.cornerRadius = style.border.radius;
    }       
}

void Gfx::drawText(Alignment alignment, int32_t x, int32_t y,
                  const char* text, const ColorRGBA& rgba)
{
    /*_cmds.emplace_back(DrawCommand::kText, kDrawSolid, rgba);
    DrawCommand& cmd = _cmds.back();
    cmd.text.str = _strings.push(text);
    cmd.text.align = alignment;
    cmd.text.x = x;
    cmd.text.y = y;
    */
}

    }   // namespace imgui
}   // namespace cinekine

