//
//  RenderTarget.hpp
//  SampleCommon
//
//  Created by Samir Sinha on 1/7/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_RenderTarget_hpp
#define CK_Graphics_RenderTarget_hpp

#include "GfxTypes.hpp"

#include <bgfx/bgfx.h>

namespace cinek {
    namespace gfx {

    //  Simple wrapper for a bgfx FrameBuffer
    //
    class RenderTarget
    {
        CK_CLASS_NON_COPYABLE(RenderTarget);
     
    public:
        static RenderTarget create(uint16_t w, uint16_t h,
            bgfx::TextureFormat::Enum format,
            int msaa=0);
     
    public:
        explicit RenderTarget(bgfx::FrameBufferHandle h);
        RenderTarget();
        
        ~RenderTarget();
        
        RenderTarget(RenderTarget&& other);
        RenderTarget& operator=(RenderTarget&& other);
        
        operator bool() const { return bgfx::isValid(_bgfxHandle); }
        
        bgfx::FrameBufferHandle bgfxHandle() const { return _bgfxHandle; }
        
        
    private:
        bgfx::FrameBufferHandle _bgfxHandle;
    };
    
    
    class MultiTextureRenderTarget
    {
        CK_CLASS_NON_COPYABLE(MultiTextureRenderTarget);
        
    public:

        
        static MultiTextureRenderTarget createWithDepthTarget(uint16_t w, uint16_t h,
            bgfx::TextureFormat::Enum format,
            bgfx::TextureFormat::Enum depthFormat,
            int msaa=0);
    
    public:
        MultiTextureRenderTarget();
        ~MultiTextureRenderTarget();
        
        MultiTextureRenderTarget(MultiTextureRenderTarget&& other);
        MultiTextureRenderTarget& operator=(MultiTextureRenderTarget&& other);
        
        operator bool() const { return bgfx::isValid(_target.bgfxHandle()); }
        explicit operator RenderTarget&() { return _target; }
    
        Texture* texture(uint32_t index=0);
        uint32_t textureCount() const { return _textureCount; }
        
        const RenderTarget& target() const { return _target; }
        RenderTarget& target() { return _target; }
    
    private:
        RenderTarget _target;
        Texture* _textures;
        uint32_t _textureCount;
    };
    
    }   // namespace gfx
}   // namespace cinek

#endif /* CK_Graphics_RenderTarget_hpp */
