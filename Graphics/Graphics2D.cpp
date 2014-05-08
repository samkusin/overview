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
                _currentAtlas(),
                _polyVertsPos(std_allocator<glm::vec2>()),
                _polyVertsUV(std_allocator<glm::vec2>()),
                _polyVertsColor(std_allocator<glm::vec4>())
    {
        /**
         * @todo Evaluate whether we can customize this - 32 verts is plenty for
         *       most cases though, where
         */
        _polyVertsPos.reserve(32);
        _polyVertsUV.reserve(32);
        _polyVertsColor.reserve(32);

        //  build pattern texture for fills
        uint8_t solid = 255;
        _solidTexture = _renderer.createTextureFromBuffer(1,1, kCinekPixelFormat_A8, &solid);
    }

    Graphics2D::~Graphics2D()
    {

    }

    void Graphics2D::clearPolyBuffer()
    {
        _polyVertsColor.clear();
        _polyVertsPos.clear();
        _polyVertsUV.clear();
    }

    //  Draws a rectangle
    void Graphics2D::drawRect(const Rect& rect, const Style& style)
    {
        _polyVertsPos.emplace_back((float)rect.left, (float)rect.top);
        _polyVertsPos.emplace_back((float)rect.right, (float)rect.top);
        _polyVertsPos.emplace_back((float)rect.left, (float)rect.bottom);
        _polyVertsPos.emplace_back((float)rect.right, (float)rect.top);
        _polyVertsPos.emplace_back((float)rect.right, (float)rect.bottom);
        _polyVertsPos.emplace_back((float)rect.left, (float)rect.bottom);
        _polyVertsUV.emplace_back(0.0f, 0.0f);
        _polyVertsUV.emplace_back(0.0f, 0.0f);
        _polyVertsUV.emplace_back(0.0f, 0.0f);
        _polyVertsUV.emplace_back(0.0f, 0.0f);
        _polyVertsUV.emplace_back(0.0f, 0.0f);
        _polyVertsUV.emplace_back(0.0f, 0.0f);

        if (style.fillMethod != kFillMethod_NoFill)
        {
            glm::vec4 color(style.fillColor.r/255.0f,
                            style.fillColor.g/255.0f,
                            style.fillColor.b/255.0f,
                            style.fillColor.a/255.0f);
            _polyVertsColor.emplace_back(color);
            _polyVertsColor.emplace_back(color);
            _polyVertsColor.emplace_back(color);
            _polyVertsColor.emplace_back(color);
            _polyVertsColor.emplace_back(color);
            _polyVertsColor.emplace_back(color);
        }

        _renderer.drawVertices(*_solidTexture, Mesh::kTriangles,
                               _polyVertsPos,
                               _polyVertsUV,
                               _polyVertsColor);
        _polyVertsColor.clear();
        _polyVertsPos.clear();
        _polyVertsUV.clear();
    }

    void Graphics2D::drawPolygon(const glm::ivec2* vertices, size_t numVertices, const Style& style)
    {
        //  manually build a fan (convex polygons only of course)
        if (numVertices < 3)
            return;
        for (size_t i = 0; i < numVertices; ++i)
        {
            _polyVertsPos.emplace_back((float)vertices[i].x, (float)vertices[i].y);
            _polyVertsUV.emplace_back(0.0f, 0.0f);
        }
        if (style.fillMethod != kFillMethod_NoFill)
        {
            glm::vec4 color(style.fillColor.r/255.0f,
                            style.fillColor.g/255.0f,
                            style.fillColor.b/255.0f,
                            style.fillColor.a/255.0f);
            for (size_t c = 0; c < numVertices; ++c)
            {
                _polyVertsColor.emplace_back(color);
            }
        }
        _renderer.drawVertices(*_solidTexture, Mesh::kTriangleFan,
                               _polyVertsPos,
                               _polyVertsUV,
                               _polyVertsColor);

        _polyVertsColor.clear();
        _polyVertsPos.clear();
        _polyVertsUV.clear();
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
                                                     y - bitmapInfo->srcH,
                                                     bitmapInfo->srcW,
                                                     bitmapInfo->srcH);
            _renderer.drawTextureRect(atlas.getTexture(), srcRect, destRect, color);
        }
    }

    }   // namespace glx
}   // namespace cinekine
