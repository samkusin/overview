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
        //  source format is derived from the path file extension
        static Texture loadTextureFromFile(const char* pathname);
        //  source format is passed by caller
        static Texture loadTextureFromMemory(const uint8_t* data, size_t len);
        //  loads texture directly from graphics memory, without any pre-processing
        static Texture loadTextureRaw(const bgfx::Memory* memory);
    
    public:
        Texture(bgfx::TextureHandle handle, bgfx::TextureFormat::Enum format);
        Texture();
        ~Texture();
        
        Texture(Texture&& other);
        Texture& operator=(Texture&& other);
        
        explicit operator bool() const { return bgfx::isValid(_bgfxHandle); }
        
        bgfx::TextureHandle bgfxHandle() const { return _bgfxHandle; }
        bgfx::TextureFormat::Enum bgfxFormat() const { return _bgfxFormat; }
        
        //  releases the backend handle from management by the Texture
        bgfx::TextureHandle release();
        
    private:
        bgfx::TextureHandle _bgfxHandle;
        bgfx::TextureFormat::Enum _bgfxFormat;
    };
    
    }   // namespace gfx
}   // namespace cinek


#endif
