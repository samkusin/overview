//
//  Texture.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/25/15.
//
//

#ifndef CK_Graphics_Texture_hpp
#define CK_Graphics_Texture_hpp

#include "GfxTypes.hpp"

#include <bgfx/bgfx.h>

namespace cinek {
    namespace gfx {

    //  Simple wrapper for a bgfx Texture and texture operations
    //
    class Texture
    {
        CK_CLASS_NON_COPYABLE(Texture);
    
    public:
        static Texture loadTextureFromFile(const char* pathname);
    
    public:
        Texture();
        ~Texture();
        
        Texture(Texture&& other);
        Texture& operator=(Texture&& other);
        
        operator bool() const { return bgfx::isValid(_bgfxHandle); }
        
        bgfx::TextureHandle bgfxHandle() const { return _bgfxHandle; }
        
    private:
        bgfx::TextureHandle _bgfxHandle;
    };
    
    }   // namespace gfx
}   // namespace cinek


#endif
