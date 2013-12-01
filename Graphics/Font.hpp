//
//  Font.hpp
//  Graphics 
//
//  Created by Samir Sinha on 9/15/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_Font_hpp
#define CK_Graphics_Font_hpp

#include "Texture.hpp"
#include "External/stb/stb_truetype.h"

#include "cinek/cpp/ckalloc.hpp"
#include "cinek/cpp/ckvector.hpp" 

namespace cinekine {
    namespace glx {

    //  Fonts 
    class Font
    {
        Font& operator=(const Font&) = delete;

    public:
        typedef vector<stbtt_bakedchar> BakedChars;
        Font(TexturePtr& texture,
             BakedChars&& bakedChars,
             int32_t height,
             int32_t minCharCode, int32_t undefinedCharCode);
        Font() = default;
        ~Font() = default;

        Font(Font&& other);
        Font& operator=(Font&& other);
        
        operator bool() const {
            return (_texture != nullptr);
        }
        
        const stbtt_bakedchar& getChar(int32_t c) const;
        const Texture& getTexture() const {
            return *_texture;
        }
        int32_t getHeight() const {
            return _height;
        }
        int32_t getMinCodepoint() const {
            return _minChar;
        }
        int32_t getMaxCodepoint() const {
            return _minChar + (int32_t)_bakedChars.size() - 1;
        }

    private:
        TexturePtr _texture;
        vector<stbtt_bakedchar> _bakedChars;
        int32_t _minChar, _undefinedChar;
        int32_t _height;
    };

    inline const stbtt_bakedchar& Font::getChar(int32_t c) const {
        int32_t normalizedCh = c-_minChar;
        if (normalizedCh < 0 || normalizedCh >= _bakedChars.size())
            return _bakedChars[_undefinedChar - _minChar];
        return _bakedChars[normalizedCh];
    }
    
    }   // namespace glx
}   // namespace cinekine

#endif