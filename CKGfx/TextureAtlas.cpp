//
//  TextureAtlas.cpp
//  Overview
//
//  Created by Samir Sinha on 4/5/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "CKGfx/TextureAtlas.hpp"
#include "External/stb/stb_image.h"

#include <cinek/string.hpp>
#include <cinek/file.hpp>
#include <algorithm>

namespace cinek {
    namespace gfx {
    
bool TextureAtlas::Texture::operator==(const char* texName) const
{
    return !strcmp(name, texName);
}

bool TextureAtlas::Texture::matchesHandle(TextureHandle h) const
{
    return (h.iter == handleIter);
}

static int stbi_read_cb(void* user, char* data, int size)
{
    return (int)getFileOps().readCb(getFileOps().context,
        reinterpret_cast<FileHandle>(user),
        (uint8_t*)data,
        size);
}

static void stbi_skip_cb(void* user, int n)
{
    getFileOps().seekCb(getFileOps().context, reinterpret_cast<FileHandle>(user),
                    FileOps::kSeekCur,
                    n);
}

static int stbi_eof_cb(void *user)
{
    return getFileOps().eofCb(getFileOps().context, reinterpret_cast<FileHandle>(user)) != 0;
}

////////////////////////////////////////////////////////////////////////////////

TextureAtlas::TextureAtlas
(
    uint16_t textureCount,
    const Allocator& allocator
) :
    _allocator(allocator),
    _textures(allocator)
{
}

TextureAtlas::TextureAtlas(TextureAtlas&& other) :
    _allocator(std::move(other._allocator)),
    _nextTextureHandle(std::move(_nextTextureHandle)),
    _textures(std::move(_textures))
{
}

TextureAtlas& TextureAtlas::operator=(TextureAtlas&& other)
{
    _allocator = std::move(other._allocator);
    _nextTextureHandle = std::move(other._nextTextureHandle);
    _textures = std::move(other._textures);
    return *this;
}

TextureAtlas::~TextureAtlas()
{
    for (auto& texture : _textures)
    {
        destroy(texture);
    }
    _textures.clear();
    _freed.clear();
}

void TextureAtlas::destroy(Texture& texture)
{
    if (bgfx::isValid(texture.bgfxHandle))
    {
        bgfx::destroyTexture(texture.bgfxHandle);
        texture.bgfxHandle = BGFX_INVALID_HANDLE;
    }
    if (texture.name)
    {
        _allocator.free(texture.name);
        texture.name = nullptr;
    }
    texture.handleIter = 0;
}

TextureHandle TextureAtlas::loadTexture(const char* path)
{
    //  find texture with existing path - if it doesn't exist, create it
    TextureHandle handle = textureHandleFromName(path);
    if (!handle)
    {
        //  load using an image loader api
        stbi_io_callbacks cbs = { &stbi_read_cb, &stbi_skip_cb, &stbi_eof_cb };
        FileHandle fh = getFileOps().openCb(getFileOps().context, path, FileOps::kReadAccess);
        int width = 0;
        int height = 0;
        int comp = 0;
        stbi_uc* data = stbi_load_from_callbacks
                        (
                            &cbs, reinterpret_cast<void*>(fh),
                            &width,
                            &height,
                            &comp,
                            0
                        );
        if (!data)
            return nullptr;
        
        Texture texture;
        texture.bgfxHandle = bgfx::createTexture2D
                             (
                                width, height, 1,
                                bgfx::TextureFormat::RGBA8,
                                BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT,
                                bgfx::copy(data, width*height*comp)
                             );
        stbi_image_free(data);
        
        // allocate a texture handle
        handle = ++_nextTextureHandle;
        if (_freed.empty())
        {
            handle.offs = (uint32_t)_textures.size();
            _textures.emplace_back();
        }
        else
        {
            handle.offs = _freed.back();
            _freed.pop_back();
        }
        
        texture.name = duplicateCString(path, _allocator);
        texture.handleIter = handle.iter;
        texture.refCnt = 1;
        
        _textures[handle.offs] = texture;
    }
    else
    {
        Texture& texture = _textures[handle.offs];
        if (!texture.matchesHandle(handle))
            return TextureHandle();
        ++_textures[handle.offs].refCnt;
    }
    
    return handle;
        
}

void TextureAtlas::unloadTexture(TextureHandle handle)
{
    if (!handle)
        return;
    
    Texture& texture = _textures[handle.offs];
    if (!texture.matchesHandle(handle))
        return;
    
    --texture.refCnt;
    if (texture.refCnt)
        return;
    
    destroy(texture);
    _freed.push_back(handle.offs);
}

//  use to retrieve texture information.
TextureHandle TextureAtlas::textureHandleFromName(const char* name) const
{
    auto it = std::find(_textures.begin(), _textures.end(), name);
    if (it == _textures.end())
        return nullptr;
    
    TextureHandle h;
    h.iter = it->handleIter;
    h.offs = (uint32_t)(it - _textures.begin());
    return h;
    
}

bgfx::TextureHandle TextureAtlas::texture(TextureHandle handle) const
{
    if (!handle)
        return BGFX_INVALID_HANDLE;
    
    auto& texture = _textures[handle.offs];
    return texture.bgfxHandle;
}
        
    }   // namespace gfx
}   // namespace cinek
