/**
 *  @file   IMGUIClient.hpp
 *
 *  @note   Created by Samir Sinha on 9/12/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "IMGUITypes.hpp"

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

#ifndef Cinekine_IMGUIClient_hpp
#define Cinekine_IMGUIClient_hpp

#include "IMGUITypes.hpp"

namespace cinekine {
    namespace imgui {

/**
 *  @class ClientInterface
 *  @brief The GUI API as exposed to Director Applications.
 *
 *  imgui::Frame implements this interface.
 */
class ClientInterface
{
public:
    virtual ~ClientInterface() {}

    /**
     * Begins a scroll area at the specified scroll position.
     * @param  rect       The scroll area's rectangle
     * @param  scrollPos  Initial scroll position
     * @return            If the mouse is within the scroll area
     */
    virtual bool beginScrollArea(const Rect& rect, int32_t scrollPos) = 0;
    /**
     * Ends a scroll area.
     * @return The final scroll position to be passed back into
     *         beginScrollArea.
     */
    virtual int32_t endScrollArea() = 0;
    /** Indents the current widget cursor. */
    virtual void indent() = 0;
    /** Unindents the current widget cursor. */
    virtual void unindent() = 0;
    /** Adds an invisible separator - basically vertical space. */
    virtual void separator() = 0;
    /** Adds a visible separator. */
    virtual void separatorLine() = 0;
    /**
     * Adds a text label.
     * @param text  Text string
     */
    virtual void label(const char* text) = 0;
};

    }   // namespace imgui
}   // namespace cinekine

#endif