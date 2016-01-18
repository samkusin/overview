//
//  Texture.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/25/15.
//
//

#include "Texture.hpp"
#include "External/stb/stb_image.h"

#include <cinek/file.hpp>


namespace cinek {
    namespace gfx {


static int stbi_read_cb(void* user, char* data, int size)
{
    return (int)file::read(reinterpret_cast<FileHandle>(user), (uint8_t*)data, size);
}

static void stbi_skip_cb(void* user, int n)
{
    file::seek(reinterpret_cast<FileHandle>(user), file::kSeekCur, n);
}

static int stbi_eof_cb(void *user)
{
    return file::eof(reinterpret_cast<FileHandle>(user)) != 0;
}


Texture Texture::loadTextureFromFile(const char* pathname)
{
    Texture texture;
    
    //  load using an image loader api
    stbi_io_callbacks cbs = { &stbi_read_cb, &stbi_skip_cb, &stbi_eof_cb };
    FileHandle fh = file::open(pathname, file::kReadAccess);
    if (!fh) {
        //  return a place holder texture (remember to increment refcnt)
        return Texture();
    }
    int width = 0;
    int height = 0;
    int comp = 0;
    stbi_uc* data = stbi_load_from_callbacks
                    (
                        &cbs, reinterpret_cast<void*>(fh),
                        &width,
                        &height,
                        &comp,
                        4
                    );
    if (data) {
        texture._bgfxHandle = bgfx::createTexture2D
                              (
                                width, height, 1,
                                bgfx::TextureFormat::RGBA8,
                                BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT,
                                bgfx::copy(data, width*height*4)
                              );
        texture._bgfxFormat = bgfx::TextureFormat::RGBA8;
        stbi_image_free(data);
    }
    
    file::close(fh);
    
    return texture;
}

Texture Texture::loadTextureFromMemory(const uint8_t* data, size_t len)
{
    Texture texture;
    
    int width = 0;
    int height = 0;
    int comp = 0;
    stbi_uc* bmpMemory = stbi_load_from_memory(data, (int)len, &width, &height, &comp, 4);
    
    if (bmpMemory) {
        texture._bgfxHandle = bgfx::createTexture2D
                              (
                                width, height, 1,
                                bgfx::TextureFormat::RGBA8,
                                BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT,
                                bgfx::copy(bmpMemory, width*height*4)
                              );
        texture._bgfxFormat = bgfx::TextureFormat::RGBA8;
        stbi_image_free(bmpMemory);
    }
    
    return texture;

}

Texture::Texture
(
    bgfx::TextureHandle handle,
    bgfx::TextureFormat::Enum format
) :
    _bgfxHandle(handle),
    _bgfxFormat(format)
{
}

Texture::Texture() :
    Texture(BGFX_INVALID_HANDLE, bgfx::TextureFormat::Unknown)
{
}

Texture::~Texture()
{
    if (bgfx::isValid(_bgfxHandle)) {
        bgfx::destroyTexture(_bgfxHandle);
        _bgfxHandle = BGFX_INVALID_HANDLE;
    }
}

Texture::Texture(Texture&& other) :
    _bgfxHandle(other._bgfxHandle),
    _bgfxFormat(other._bgfxFormat)
{
    other._bgfxHandle = BGFX_INVALID_HANDLE;
    other._bgfxFormat = bgfx::TextureFormat::Unknown;
}

Texture& Texture::operator=(Texture&& other)
{
    _bgfxHandle = other._bgfxHandle;
    _bgfxFormat = other._bgfxFormat;
    other._bgfxHandle = BGFX_INVALID_HANDLE;
    other._bgfxFormat = bgfx::TextureFormat::Unknown;
    return *this;
}

bgfx::TextureHandle Texture::release()
{
    bgfx::TextureHandle h = _bgfxHandle;
    _bgfxHandle = BGFX_INVALID_HANDLE;
    _bgfxFormat = bgfx::TextureFormat::Unknown;
    return h;
}
    
    }   // namespace gfx
}   // namespace cinek
