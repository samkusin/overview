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

#include "Graphics2D.hpp"
#include "RendererCLI.hpp"
#include "BitmapLibrary.hpp"
#include "FontLibrary.hpp"
#include "Style.hpp"
#include "Rect.hpp"

#include "glm/glm.hpp"

namespace cinekine {
    namespace glx {
 
    Graphics2D::Graphics2D(RendererCLI& renderer, BitmapLibrary& bitmapLibrary, 
                           FontLibrary& fontLibrary) :
                _renderer(renderer),
                _bitmapLibrary(bitmapLibrary),
                _fontLibrary(fontLibrary),
                _currentAtlas()
    {
    }

    Graphics2D::~Graphics2D()
    {

    }

    
    //  Draws a rectangle
    void Graphics2D::drawRect(const Rect& rect, const Style& style)
    {
        glm::ivec2 verts[4];
        verts[0].x = rect.left;
        verts[0].y = rect.top;
        verts[1].x = rect.right;
        verts[1].y = rect.top;
        verts[2].x = rect.right;
        verts[2].y = rect.bottom;
        verts[3].x = rect.left;
        verts[3].y = rect.bottom;
        drawPolygon(verts, 4, style);
    }

    void Graphics2D::drawPolygon(const glm::ivec2* vertices, size_t numVertices, const Style& style)
    {

    }
    
    void Graphics2D::drawText(const char* text, int32_t x, int32_t y,
                              const Style& style)
    {
        const Font& font = _fontLibrary.getFont(style.textFont);
        if (!font)
            return;
    
        const char* curtext = text;
        
        while (*curtext)
        {
            int c = (unsigned char)*curtext;
            if (c == '\t')
            {
                /** @todo: tabstops for drawText */
            }
            else
            {
                const stbtt_bakedchar& bakedChar = font.getChar(c);
                Rect src(bakedChar.x0, bakedChar.y0, bakedChar.x1, bakedChar.y1);
                Rect dest = Rect::rectFromDimensions(x+bakedChar.xoff,
                                                     y+bakedChar.yoff,
                                                     src.width(), src.height());
                
                _renderer.drawTextureRect(font.getTexture(), src, dest, style.textColor);
                x += bakedChar.xadvance;
            }
            ++curtext;
        }
    }
    
    void Graphics2D::setBitmapAtlas(cinek_bitmap_atlas atlas)
    {
        _currentAtlas = _bitmapLibrary.getAtlas(atlas);
    }
    
    void Graphics2D::drawBitmapFromAtlas(cinek_bitmap_index bitmapIndex, 
                                         int32_t x, int32_t y, float alpha)
    {
        if (!_currentAtlas)
            return;

        const BitmapAtlas& atlas = *_currentAtlas.get();
        const glx::BitmapInfo* bitmapInfo = atlas.getBitmapFromIndex(bitmapIndex);
        if (bitmapInfo)
        {
            RGBAColor color(255,255,255,(uint8_t)(alpha*255));
            Rect srcRect(bitmapInfo->x, bitmapInfo->y,
                         bitmapInfo->x+bitmapInfo->w, bitmapInfo->y+bitmapInfo->h);
            Rect destRect = Rect::rectFromDimensions(x + bitmapInfo->offX, 
                                                     y - bitmapInfo->srcH + bitmapInfo->offY,
                                                     bitmapInfo->w,
                                                     bitmapInfo->h);
            _renderer.drawTextureRect(atlas.getTexture(), srcRect, destRect, color);
        }
    }

    }   // namespace glx
}   // namespace cinekine
