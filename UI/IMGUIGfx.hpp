/**
 *  @file   IMGUIGfx.hpp
 *
 *  @note   Created by Samir Sinha on 9/29/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Cinekine_IMGUIGfx_hpp
#define Cinekine_IMGUIGfx_hpp

#include "IMGUITypes.hpp"

namespace cinekine {
    namespace imgui {

class Gfx
{

public:
    Gfx(size_t queueSize, size_t stringBufferSize, 
        const Allocator& allocator=Allocator());
    ~Gfx() = default;

    void clearCommandQueue();

    const DrawCommands& getCommandQueue() const {
        return _cmds;
    }

    //  cursor commands
    void setCursor(int32_t x, int32_t y);
    void moveCursor(int32_t xOff, int32_t yOff);

    //  drawing commands
    void drawBackground(const Style& style, int32_t w, int32_t h);
    void drawText(Alignment alignment, int32_t x, int32_t,
                  const char* text, const ColorRGBA& rgba);

private:
    Allocator _allocator;
    DrawCommands _cmds;
    StringStack _strings;

    int32_t _cursorX, _cursorY;
};


    }   // namespace imgui
}   // namespace cinekine

#endif
