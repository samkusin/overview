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
 * 
 * @file    Graphics2D.hpp
 * @author  Samir Sinha
 * @date    11/26/2013
 * @brief   Convenience utility for 2D graphics using a Renderer implementation
 * @copyright Cinekine
 */

#ifndef CK_Graphics_Graphics2D_hpp
#define CK_Graphics_Graphics2D_hpp

#include "RenderTypes.hpp"
#include "Style.hpp"
#include "Texture.hpp"

#include "cinek/cpp/ckvector.hpp"

#include <memory>

namespace cinekine {
    namespace glx {

    class RendererCLI;
    class BitmapLibrary;
    class BitmapAtlas;
    class FontLibrary;
    class Texture;
    struct Style;
    struct Rect;

    /**
     * @class Graphics2D
     * @brief A utility for rendering 2D-specific graphics via the Renderer
     */
    class Graphics2D
    {
    public:
        Graphics2D(RendererCLI& renderer, BitmapLibrary& bitmapLibrary, 
                   FontLibrary& fontLibrary);
        ~Graphics2D();

        //  Draws a rectangle
        void drawRect(const Rect& rect, const Style& style);
        /**
         * Draws a convex polygon given an array of vertices
         * @param vertices    The polygon's vertices
         * @param numVertices The vertex count
         * @param style       The draw style
         */
        void drawPolygon(const glm::ivec2* vertices, size_t numVertices,
                         const Style& style);
        //  Draws text
        void drawText(const char* text, int32_t x, int32_t y,
                      const Style& style);
        //  Sets the current bitmap atlas
        void setBitmapAtlas(cinek_bitmap_atlas atlas);
        //  Draws a bitmap from the current atlas with alpha
        void drawBitmapFromAtlas(cinek_bitmap_index bitmapIndex, 
                                 int32_t x, int32_t y, float alpha);
    
    private:
        RendererCLI& _renderer;
        BitmapLibrary& _bitmapLibrary;
        FontLibrary& _fontLibrary;
        std::shared_ptr<BitmapAtlas> _currentAtlas;
        TexturePtr _solidTexture;

        vector<glm::vec2> _polyVertsPos;
        vector<glm::vec2> _polyVertsUV;
        vector<glm::vec4> _polyVertsColor;

        void clearPolyBuffer();
    };

    }   // namespace glx
}   // namespace cinekine

#endif
