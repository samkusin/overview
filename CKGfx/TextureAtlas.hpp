//
//  TextureAtlas.hpp
//  Overview
//
//  Created by Samir Sinha on 4/5/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_TextureAtlas_hpp
#define CK_Graphics_TextureAtlas_hpp

#include "GfxTypes.hpp"

#include <cinek/vector.hpp>
#include <bgfx/bgfx.h>

namespace cinek {
    namespace gfx {

    class TextureAtlas
    {
        CK_CLASS_NON_COPYABLE(TextureAtlas);

    public:
        TextureAtlas
        (
            uint16_t textureCount,
            const Allocator& allocator
        );
        TextureAtlas(TextureAtlas&& other);
        TextureAtlas& operator=(TextureAtlas&& other);
        
        ~TextureAtlas();
        
        //  use to build the atlas.
        TextureHandle loadTexture(const char* path);
        void unloadTexture(TextureHandle handle);

        //  use to retrieve texture information.
        TextureHandle textureHandleFromName(const char* name) const;
        bgfx::TextureHandle texture(TextureHandle handle) const;

    private:
        Allocator _allocator;
        TextureHandle _nextTextureHandle;
        
        struct Texture
        {
            char* name;
            bgfx::TextureHandle bgfxHandle;
            uint32_t handleIter;
            uint32_t refCnt;
            
            bool operator==(const char* texName) const;
            bool matchesHandle(TextureHandle handle) const;
        };
        vector<Texture> _textures;
        vector<uint32_t> _freed;
        
        void destroy(Texture& texture);
    };

    }   // namespace gfx
}   // namespace cinek

#endif
