/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef Overview_View_hpp
#define Overview_View_hpp

#include "ViewCreate.hpp"

#include "cinek/types.hpp"
#include "cinek/allocator.hpp"

namespace cinekine {
    namespace ovengine {

    class Theater;

    /**
     * @class ViewShared
     * @brief External component/server calls to View objects
     */
    class View
    {
    public:
        virtual ~View() {}

        //  Executes rendering code specific to the View implementation.
        virtual void render() = 0;

        /** Mouse button constants */
        enum MouseButton
        {
            kMouseButtonLeft,       /**< Left mouse button */
            kMouseButtonRight,      /**< Right mouse button */
            kMouseButtonCenter      /**< Center mouse button */
        };
        /**
         * Handles mouse down events within the view
         * @param button Button index
         * @param x      X coordinate within view
         * @param y      Y coordinate within view
         */
        virtual void onMouseButtonDown(MouseButton button, int32_t x, int32_t y) {}
        /**
         * Handles mouse up events within the view
         * @param button Button index
         * @param x      X coordinate within view
         * @param y      Y coordinatw within view
         */
        virtual void onMouseButtonUp(MouseButton button, int32_t x, int32_t y) {}

        /** The area where the mouse move event occurred */
        enum MouseRegion
        {
            kMouseRegionOver,       /**< Mouse motion within the view */
            kMouseRegionEnter,      /**< Mouse entered view */
            kMouseRegionExit        /**< Mouse exited view */
        };
        /**
         * Mouse movement event
         * @param region The mouse region where the motion occurred
         * @param x      X coordinate within view
         * @param y      Y coordinate within view
         */
        virtual void onMouseMove(MouseRegion region, int32_t x, int32_t y) {}
    };



    }   // namespace ovengine
}   // namespace cinekine

#endif
