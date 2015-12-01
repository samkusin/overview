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
    LoadFile(name, cb)
{

}

gfx::Texture LoadTextureAsset::acquireTexture()
{
    return std::move(_texture);
}


void LoadTextureAsset::onFileLoaded()
{
    _texture = std::move(gfx::Texture::loadTextureFromMemory(buffer().data(), buffer().size()));
    
    LoadFile::onFileLoaded();
}
    
    }  /* namespace ove */
}  /* namespace cinek */
