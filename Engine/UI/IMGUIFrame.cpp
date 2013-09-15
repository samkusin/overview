/**
 *  @file   IMGUIFrame.cpp
 *
 *  @note   Created by Samir Sinha on 9/12/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "IMGUIFrame.hpp"
#include <cstdlib>

namespace cinekine {
    namespace imgui {

///////////////////////////////////////////////////////////////////////////////
    
static void* DefaultAlloc(void* cxt, size_t sz) {
    return malloc(sz);
}
void DefaultFree(void* cxt, void* p) {
    free(p);
}
    
Allocator::Allocator() : cxt(NULL), alloc(DefaultAlloc), free(DefaultFree) {}

///////////////////////////////////////////////////////////////////////////////

Frame::Frame(const Rect& rect, size_t queueSize, const Allocator& allocator) :
    _frame(rect),
    _cmds(imgui_allocator<DrawCommand>(allocator))
{
    _cmds.reserve(queueSize);
}

void Frame::begin(const Input& input)
{
    _cmds.clear();
}

void Frame::end()
{

}

bool Frame::beginScrollArea(const char* name, const Rect& rect,
                                          int32_t scrollPos)
{
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
