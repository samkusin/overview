/**
 *  @file   IMGUIFrame.hpp
 *
 *  @note   Created by Samir Sinha on 9/12/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "IMGUITypes.hpp"

#include <vector>

//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

// This is a heavily modified version of the source altered from 
// https://github.com/AdrienHerubel/imgui

#ifndef Cinekine_IMGUIFrame_hpp
#define Cinekine_IMGUIFrame_hpp

#include "IMGUITypes.hpp"

namespace cinekine {
    namespace imgui {

/**
 *  @class Frame
 *  @brief Manages GUI elements within a single interactive frame.
 *
 *  Applications can create one or more IMGUIFrame instances.  Typically
 *  events are filtered by an application controller to an instance that
 *  has focus.  Of course, in this case its up to an application, or a parent
 *  controler object to determine which IMGUIFrame has such focus.
 *
 *  IMGUIFrame often references coordinates (x, y, width, height, etc.)  These
 *  coordinates can be screen coordinates, or 'virtual' coordinates.
 */
class Frame
{
public:
    /** 
     * Defines the rectangle encasing the IMGUI.  All GUI elements are
     * positioned relative to the specified rectangle.
     * @param rect      The frame's rectangle.
     * @param queueSize Default size of the Frame's command queue.
     * @param allocator A compliant imgui::Allocator.
     */
    Frame(const Rect& rect, size_t queueSize, const Allocator& allocator=Allocator());
    ~Frame() = default;

    /**
     * Begins a IMGUI command batch. This method will flush the current
     * command queue.
     * @param  input User input used to determine widget state.
     */
    void begin(const Input& input);
    /**
     * Ends a frame.
     */
    void end();
    /**
     * Retrieves the current command queue as a C array.  Note the
     * returned pointer references data within the current command queue.
     * Any subsequent calls to other Frame methods may invalidate the
     * pointer.  Basically the caller should immediately consume the queue.
     * @param  numCommands Recieves the queue size.
     * @return A pointer to commands in the command queue.
     */
    const DrawCommand* getDrawCommandQueue(size_t* numCommands) const {
        *numCommands = _cmds.size();
        return _cmds.data();
    }
    /**
     * Begins a scroll area at the specified scroll position.
     * @param  name       Name label
     * @param  rect       The scroll area's rectangle
     * @param  scrollPos  Initial scroll position
     * @return            If the mouse is within the scroll area
     */
    bool beginScrollArea(const char* name, const Rect& rect,
                         int32_t scrollPos);
    /**
     * Ends a scroll area.
     * @return The final scroll position to be passed back into
     *         beginScrollArea.
     */
    int32_t endScrollArea();
    /** Indents the current widget cursor. */
    void indent();
    /** Unindents the current widget cursor. */
    void unindent();
    /** Adds an invisible separator - basically vertical space. */
    void separator();
    /** Adds a visible separator. */
    void separatorLine();
    /**
     * Adds a text label.
     * @param text  Text string
     */
    void label(const char* text);

private:
    Rect _frame;
    std::vector<DrawCommand, imgui_allocator<DrawCommand>>  _cmds;
};

    }   // namespace imgui
}   // namespace cinekine

#endif

