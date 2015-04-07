//
//  Font.cpp
//  Overview
//
//  Created by Samir Sinha on 9/15/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Font.hpp"

namespace cinek {
    namespace glx {

Font::Font(TexturePtr& texture,
           vector<stbtt_bakedchar>&& bakedChars,
           int32_t height,
           int32_t minCharCode, int32_t undefinedCharCode) :
    _texture(texture),
    _bakedChars(std::move(bakedChars)),
    _minChar(minCharCode),
    _undefinedChar(undefinedCharCode),
    _height(height)
{
}

Font::Font(Font&& other) :
    _texture(std::move(other._texture)),
    _bakedChars(std::move(other._bakedChars)),
    _minChar(other._minChar),
    _undefinedChar(other._minChar),
    _height(other._height)
{
    other._height = 0;
    other._minChar = 0;
    other._undefinedChar = 0;
}

Font& Font::operator=(Font&& other)
{
    _texture = std::move(other._texture);
    _bakedChars = std::move(other._bakedChars);
    _height = other._height;
    _minChar = other._minChar;
    _undefinedChar = other._undefinedChar;
    other._height = other._minChar = other._undefinedChar = 0;

    return *this;
}

    }   // namespace glx
}   // namespace cinek
