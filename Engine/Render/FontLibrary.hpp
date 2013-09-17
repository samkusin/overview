//
//  Render/FontLibrary.hpp
//  Overview
//
//  Created by Samir Sinha on 9/15/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Renderer_FontLibrary_hpp
#define Overview_Renderer_FontLibrary_hpp

#include "Font.hpp"
#include "cinek/cpp/ckstring.hpp"

#include <vector>

namespace cinekine {
    namespace ovengine {
    
    class Renderer;
    class Font;
    
    //  A FontLibrary contains a set of fonts.
    //  
    class FontLibrary
    {
        CK_CLASS_NON_COPYABLE(FontLibrary);

    public:
        FontLibrary(Renderer& renderer, size_t fontLimit);
        ~FontLibrary() = default;
        
        void loadFont(FontHandle slot, const char* fontname);
        void unloadFont(FontHandle slot);

        const Font* getFont(FontHandle slot) const {
            if (slot >= _fonts.size())
                return nullptr;
            return &_fonts[slot];
        }

    private:
        Renderer& _renderer;
        std::vector<Font, std_allocator<Font>> _fonts;
    };
    
    }   // namespace ovengine
}   // namespace cinekine

#endif