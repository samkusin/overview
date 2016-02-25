//
//  LoadTextureAsset.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/30/15.
//
//

#include "LoadTextureAsset.hpp"

namespace cinek {
    namespace ove {
 
const UUID LoadTextureAsset::kUUID = {
    0xe5, 0xaa, 0xb9, 0x81, 0xb0, 0x0d, 0x42, 0xee,
    0x80, 0xa2, 0xeb, 0x37, 0xb9, 0xfb, 0x91, 0x95
};


LoadTextureAsset::LoadTextureAsset
(
    std::string name,
    EndCallback cb
) :
    LoadFile(name, cb),
    _name(name),
    _memoryType(kNoMemory)
{
    auto extpos = _name.find_last_of('.');
    if (extpos != std::string::npos) {
        if (!_name.compare(extpos, std::string::npos, ".dds") ||
            !_name.compare(extpos, std::string::npos, ".pvr") ||
            !_name.compare(extpos, std::string::npos, ".ktx")) {
        
            _memoryType = kGfxMemory;
            _memory = nullptr;
        }
    }
    
    _name.erase(extpos);
    
    if (_memoryType == kNoMemory) {
        _memoryType = kBufferMemory;
        _buffer.data = nullptr;
        _buffer.size = 0;
    }
}

LoadTextureAsset::~LoadTextureAsset()
{
    switch (_memoryType) {
    
    case kBufferMemory:
        _allocator.free(_buffer.data);
        break;
        
    case kGfxMemory:
        //  gfx memory freed by bgfx subsystem
        break;
        
    default:
        CK_ASSERT(false);
        break;
    }
}

gfx::Texture LoadTextureAsset::acquireTexture()
{
    return std::move(_texture);
}

std::string LoadTextureAsset::acquireTextureName()
{
    return std::move(_name);
}


void LoadTextureAsset::onFileLoaded()
{
    switch (_memoryType) {
    
    case kBufferMemory:
        _texture = std::move(gfx::Texture::loadTextureFromMemory(
            _buffer.data, _buffer.size));
        break;
        
    case kGfxMemory:
        //  gfx memory freed by bgfx subsystem
        _texture = std::move(gfx::Texture::loadTextureRaw(_memory));
        break;
        
    default:
        CK_ASSERT(false);
        break;
    }
    
    LoadFile::onFileLoaded();
}

uint8_t* LoadTextureAsset::acquireBuffer(uint32_t size)
{
    switch (_memoryType) {
    
    case kBufferMemory:
        _buffer.data = reinterpret_cast<uint8_t*>(_allocator.alloc(size));
        _buffer.size = size;
        return _buffer.data;
        
    case kGfxMemory:
        _memory = bgfx::alloc(size);
        return _memory->data;
    
    default:
        CK_ASSERT(false);
        break;
    }
    
    return nullptr;
}
    
    }  /* namespace ove */
}  /* namespace cinek */
