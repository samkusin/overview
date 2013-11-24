//
//  FontLibrary.cpp
//  Overview
//
//  Created by Samir Sinha on 9/15/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "FontLibrary.hpp"
#include "Renderer.hpp"
#include "RenderDebug.hpp"
#include "RenderUtils.hpp"
#include "Stream/FileStreamBuf.hpp"
#include "External/stb/stb_truetype.h"

namespace cinekine {
    namespace glx {

//  Modified form of stbtt_BakeFontBitmap
//  
//  if pixels is null, the minimum rect width and height encompassing the
//      font is returned via 'pw' and 'ph'. in this case, pw and ph should
//      contain maximum values (i.e. we can limit a font to within a 
//      texture of size 'pw' x 'ph')
//  if pixels is not null, the font is rendered into the pixel buffer of
//      size pw x ph
//  
//  returned is the last glyph character code rendered.
//  
static uint16_t bakeFontToBuffer(const unsigned char *data,
    int offset,                     // font location (use offset=0 for plain .ttf)
    uint16_t height,                // height of font in pixels
    unsigned char *pixels, uint16_t *pw, uint16_t *ph,  // bitmap to be filled in
    int firstChar, int lastChar,    // characters to bake
    Font::BakedChars* chardata)
{
    //  determine dimensions on first pass.  
    uint16_t right = 1;
    uint16_t bottom = 1;
    uint16_t x = right, y = bottom;

    int charIndex = 0;
    const int kNumChars = lastChar - firstChar + 1;

    if (pixels && chardata)
    {
        chardata->reserve(kNumChars);
    }
    
    stbtt_fontinfo f;
    stbtt_InitFont(&f, data, offset);
    float scale = stbtt_ScaleForPixelHeight(&f, (float)height);

    for (; charIndex < kNumChars; ++charIndex)
    {
        int g = stbtt_FindGlyphIndex(&f, firstChar + charIndex);
        int advance, lsb, x0, y0, x1, y1;
        stbtt_GetGlyphHMetrics(&f, g, &advance, &lsb);
        stbtt_GetGlyphBitmapBox(&f, g, scale,scale, &x0,&y0,&x1,&y1);
      
        uint16_t gw = (uint16_t)(x1-x0);
        uint16_t gh = (uint16_t)(y1-y0);
        if (x + gw + 1 >= *pw)
        {
            if (x > right)
                right = x;
            y = bottom;
            x = 1;
        }
        if (y + gh + 1 >= *ph)
            break;
        if (pixels && chardata)
        {
            stbtt_MakeGlyphBitmap(&f, pixels+x+y* (*pw), gw,gh,*pw, scale,scale, g);
            chardata->push_back({
                    x, y,
                    (uint16_t)(x+gw), (uint16_t)(y+gh),
                    (float)x0, (float)y0, scale* advance
            });
        }
        x = x + gw + 2;
        if (y+gh+2 > bottom)
            bottom = y+gh+2;
    }

    if (!pixels)
    {
        *pw = right;
        *ph = bottom;
    }

    return charIndex;
}


FontLibrary::FontLibrary(Renderer& renderer, size_t fontLimit) :
    _renderer(renderer),
    _fonts(std_allocator<Font>(_renderer.getAllocator()))
{
    //  TODO - make reserve size configurable
    //  NOTE, we're using this method to prepopulate the font library
    //  since Font objects have their copy constructor disabled.  Vector initialization
    //  requires that the Vector object have a copy constructor enabled, though we already
    //  have a move constructor for Font (hence why push_back works.
    //  
    //  Is this really the case?  Isn't there a better way to do this in C++11?
    _fonts.reserve(10);
    while (_fonts.size() < 10)
    {
        _fonts.push_back(Font());
    }
}
    
bool FontLibrary::loadFont(FontHandle slot, const char* fontname, uint16_t height)
{
    if (slot >= _fonts.size())
    {
        RENDER_LOG_ERROR("Failed to load slot [%u].  Font '%s' not found.",
                          slot, fontname);
        return false;
    }
    //  clear out the slot first to release some resources.
    //
    unloadFont(slot);

    //  ingest a ttf binary using stbtt
    //  we'll need to precalculate the desired texture size and prefer a 'reasonably'
    //  dimensioned 2d rectangular GL-compliant texture.
    //  
    const int kMinCharCode = 32;
    const int kMaxCharCode = 255;
    const uint16_t kRightLimit = 512;
    const uint16_t kBottomLimit = 512;

    uint16_t bufferWidth = kRightLimit;
    uint16_t bufferHeight = kBottomLimit;
    
    char fontPath[MAX_PATH];
    snprintf(fontPath, sizeof(fontPath), "static/fonts/%s", fontname);

    FileStreamBuf ttfStream(fontPath, std::ios_base::in | std::ios_base::binary);
    if (!ttfStream)
    {
        RENDER_LOG_ERROR("Failed to load font %s in slot [%u]", fontname, slot);
        return false;
    }
    size_t fileSize = ttfStream.availableChars();
    uint8_t* sourceData = (uint8_t*)_renderer.getAllocator().alloc(fileSize);
    if (!sourceData)
        return false;
    size_t readCount = ttfStream.sgetn((char*)sourceData, fileSize);
    if (readCount == fileSize)
    {
        int charCodeLimit = bakeFontToBuffer(sourceData, 0, height,
                                             NULL, &bufferWidth, &bufferHeight,
                                             kMinCharCode, kMaxCharCode,
                                             NULL);

        //  we must calculate our font atlas coords using the same texture dimensions as
        //  the generated SDL texture.  so make sure our dimensions play nice with (the vast
        //  majority) of renderers.
        bufferWidth = glx::powerOf2(bufferWidth);
        bufferHeight = glx::powerOf2(bufferHeight);

        //  allocate our target buffer - it will be used to generate the texture
        uint8_t* buffer = (uint8_t*)_renderer.getAllocator().alloc(bufferWidth*bufferHeight);
        if (buffer)
        {
            memset(buffer, 0, bufferWidth*bufferHeight);
            //  bake into buffer - just crib the code from stbtt_BakeFontBitmap for now.
            Font::BakedChars bakedChars;
            bakeFontToBuffer(sourceData, 0, height,
                             buffer, &bufferWidth, &bufferHeight,
                             kMinCharCode, charCodeLimit,
                             &bakedChars);

            //  create font using baskedChars/texture and add to library
            unique_ptr<Texture> texture = _renderer.createTextureFromBuffer(bufferWidth, bufferHeight,
                                                                  kCinekPixelFormat_A8,
                                                                  buffer);
            _renderer.getAllocator().free(buffer);
            
            if (texture)
            {
                Font font(texture, std::move(bakedChars), height, kMinCharCode, kMinCharCode);
                RENDER_LOG_INFO("Font %s loaded in slot [%u]: texture:(%u,%u), height: %d, codepoint:[%x,%x].",
                                fontname, slot,
                                bufferWidth, bufferHeight,
                                font.getHeight(), font.getMinCodepoint(), font.getMaxCodepoint());
                _fonts[slot] = std::move(font);
            }
            else
            {
                RENDER_LOG_ERROR("Failed to load font %s in slot [%u] - unable to initialize texture.",
                                 fontname, slot);
                return false;
            }
        }
    }
    else
    {
        RENDER_LOG_ERROR("Failed to load font %s in slot [%u] - unable to read data (read: %u/%u bytes).",
                         fontname, slot, readCount, fileSize);
    }
    _renderer.getAllocator().free(sourceData);
    return readCount == fileSize;
}

void FontLibrary::unloadFont(FontHandle slot)
{
    if (slot >= _fonts.size())
        return;
    if (_fonts[slot])
    {
        RENDER_LOG_INFO("Unloaded font slot [%u].", slot);
        _fonts[slot] = Font();
    }
}   
    
    }   // namespace glx
}   // namespace cinekine
