//
//  RenderTarget.cpp
//  SampleCommon
//
//  Created by Samir Sinha on 1/7/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#include "RenderTarget.hpp"
#include "Texture.hpp"

#include <cinek/debug.h>
#include <cinek/allocator.hpp>

namespace cinek {
    namespace gfx {
    
RenderTarget::RenderTarget(bgfx::FrameBufferHandle h) :
    _bgfxHandle(h)
{
}

RenderTarget::RenderTarget() :
    _bgfxHandle(BGFX_INVALID_HANDLE)
{
}
        
RenderTarget::~RenderTarget()
{
    if (bgfx::isValid(_bgfxHandle)) {
        bgfx::destroyFrameBuffer(_bgfxHandle);
    }
}
        
RenderTarget::RenderTarget(RenderTarget&& other) :
    _bgfxHandle(other._bgfxHandle)
{
    other._bgfxHandle = BGFX_INVALID_HANDLE;
}

RenderTarget& RenderTarget::operator=(RenderTarget&& other)
{
    _bgfxHandle = other._bgfxHandle;
    other._bgfxHandle = BGFX_INVALID_HANDLE;
    return *this;
}
        
RenderTarget RenderTarget::create
(
    uint16_t w, uint16_t h,
    bgfx::TextureFormat::Enum format,
    int msaa
)
{
    RenderTarget target;
    
    target._bgfxHandle = bgfx::createFrameBuffer(w, h, format,
        (msaa+1)<<BGFX_TEXTURE_RT_MSAA_SHIFT);
    
    return target;
}
        
MultiTextureRenderTarget MultiTextureRenderTarget::createWithDepthTarget
(
    uint16_t w, uint16_t h,
    bgfx::TextureFormat::Enum format,
    bgfx::TextureFormat::Enum depthFormat,
    int msaa
)
{
    MultiTextureRenderTarget target;
    
    bgfx::TextureHandle th[2];
    bgfx::TextureFormat::Enum fmt[2];
    
    th[0] = bgfx::createTexture2D(w, h, 1, format,
        ((msaa+1) << BGFX_TEXTURE_RT_MSAA_SHIFT) |
        BGFX_TEXTURE_U_CLAMP |
        BGFX_TEXTURE_V_CLAMP
    );
    if (!bgfx::isValid(th[0]))
        return target;
    fmt[0] = format;
    
    th[1] = bgfx::createTexture2D(w, h, 1, depthFormat,
        BGFX_TEXTURE_RT_BUFFER_ONLY);
        
    if (!bgfx::isValid(th[1])) {
        bgfx::destroyTexture(th[0]);
        return target;
    }
    fmt[1] = depthFormat;
    
    const uint8_t numTextures = static_cast<uint8_t>(sizeof(th)/ sizeof(bgfx::TextureHandle));
    
    bgfx::FrameBufferHandle fbh = bgfx::createFrameBuffer(2,
        th,
        true);
    target._target = std::move(RenderTarget(fbh));
    
    Allocator allocator;
    target._textures = allocator.allocItems<Texture>(numTextures);
    for (uint8_t i = 0; i < numTextures; ++i) {
        ::new(&target._textures[i]) Texture(th[0], fmt[i]);
    }
    target._textureCount = numTextures;
    
    return target;
}
    
MultiTextureRenderTarget::MultiTextureRenderTarget() :
    _textures(nullptr),
    _textureCount(0)
{
}

MultiTextureRenderTarget::~MultiTextureRenderTarget()
{
    if (_textures) {
        for (uint32_t i = 0; i < _textureCount; ++i) {
            _textures[i].~Texture();
        }
        Allocator allocator;
        allocator.free(_textures);
        _textures = nullptr;
    }
}
        
MultiTextureRenderTarget::MultiTextureRenderTarget(MultiTextureRenderTarget&& other) :
    _target(std::move(other._target)),
    _textures(other._textures),
    _textureCount(other._textureCount)
{
    other._textures = nullptr;
    other._textureCount = 0;
}

MultiTextureRenderTarget& MultiTextureRenderTarget::operator=
(
    MultiTextureRenderTarget&& other
)
{
    _target = std::move(other._target);
    _textures = other._textures;
    _textureCount = other._textureCount;
    other._textures = nullptr;
    other._textureCount = 0;
    return *this;
}
    
Texture* MultiTextureRenderTarget::texture(uint32_t index)
{
    CK_ASSERT_RETURN_VALUE(index < _textureCount, nullptr);
    return &_textures[index];
}

    
    }   // namespace gfx
}   // namespace cinek