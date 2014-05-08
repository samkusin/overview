//
//  Rect.hpp
//  Graphics
//
//  Created by Samir Sinha on 11/14/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_Render_Rect_hpp
#define CK_Graphics_Render_Rect_hpp

#include <cinek/framework/types.hpp>
#include <array>

namespace cinekine {
    namespace glx {

    /**
     * @class Rect
     * @brief Yet another rectangle class specialized for the Graphics library
     */
    struct Rect
    {
        /**
         * Creates a rectangle from the specified width and height
         * @param l     The left coordinate
         * @param t     The top coordinate
         * @param w     The width
         * @param h     The height
         */
        static Rect rectFromDimensions(int32_t x, int32_t y, int32_t w, int32_t h);
        /**
         * Constructor
         * @param l     The left coordinate
         * @param t     The top coordinate
         * @param r     The right coordinate
         * @param b     The bottom coordinate
         */
        Rect(int32_t l, int32_t t, int32_t r, int32_t b);

        int32_t left;       /**< The left coordinate */
        int32_t top;        /**< The top coordinate */
        int32_t right;      /**< The right coordinate */
        int32_t bottom;     /**< The bottom coordinate */

        /** @return The rectangle width */
        int32_t width() const;
        /** @return The rectangle height */
        int32_t height() const;
    };

    inline Rect Rect::rectFromDimensions(int32_t x, int32_t y, int32_t w, int32_t h)
    {
        return Rect (x, y, x+w, y+h);
    }

    inline Rect::Rect(int32_t l, int32_t t, int32_t r, int32_t b) :
        left(l), top(t), right(r), bottom(b)
    {
    }

    inline int32_t Rect::width() const {
        return (right-left);
    }

    inline int32_t Rect::height() const {
        return (bottom-top);
    }

    }   // namespace glx
}   // namespace cinekine

#endif
