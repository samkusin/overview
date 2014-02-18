//
//  Render/FontLibrary.hpp
//  Overview
//
//  Created by Samir Sinha on 9/15/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_FontLibrary_hpp
#define CK_Graphics_FontLibrary_hpp

#include "Font.hpp"
#include "RenderTypes.hpp"
#include "cinek/framework/string.hpp"

#include <vector>

namespace cinekine {
    namespace glx {
    
    class RendererCLI;
    class Font;

    //  A FontLibrary contains a set of fonts.
    //  
    class FontLibrary
    {
        CK_CLASS_NON_COPYABLE(FontLibrary);

    public:
        FontLibrary(RendererCLI& renderer, size_t fontLimit,
                    const Allocator& allocator=Allocator());
        ~FontLibrary() = default;
        
        bool loadFont(FontHandle slot, const char* pathname, uint16_t height);
        void unloadFont(FontHandle slot);

        const Font& getFont(FontHandle slot) const {
            if (slot >= _fonts.size())
                return _nullfont;
            return _fonts[slot];
        }

    private:
        static const Font _nullfont;
        Allocator _allocator;
        RendererCLI& _renderer;
        vector<Font> _fonts;
    };
    
    }   // namespace glx
}   // namespace cinekine

#endif