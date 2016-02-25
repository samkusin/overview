//
//  LoadTextureAsset.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/30/15.
//
//

#ifndef Overview_Task_LoadTextureAsset_hpp
#define Overview_Task_LoadTextureAsset_hpp

#include "Engine/Tasks/LoadFile.hpp"
#include "CKGfx/Texture.hpp"

#include <cinek/task.hpp>
#include <cinek/allocator.hpp>
#include <ckio/file.h>

#include <string>

namespace cinek {
    namespace ove {

class LoadTextureAsset : public LoadFile
{
public:
    static const UUID kUUID;
    
    LoadTextureAsset(std::string name, EndCallback cb=0);
    virtual ~LoadTextureAsset();
    
    gfx::Texture acquireTexture();
    std::string acquireTextureName();
    
    virtual const TaskClassId& classId() const override { return kUUID; }
    
protected:
    virtual void onFileLoaded() override;
    
    virtual uint8_t* acquireBuffer(uint32_t size) override;
    
private:
    Allocator _allocator;
    gfx::Texture _texture;
    std::string _name;
    enum { kNoMemory, kBufferMemory, kGfxMemory } _memoryType;
    
    struct Buffer
    {
        uint8_t* data;
        uint32_t size;
    };
    union
    {
        Buffer _buffer;
        const bgfx::Memory* _memory;
    };
};
    
    }  /* namespace ove */
}  /* namespace cinek */


#endif /* Overview_Task_LoadAssetManifest_hpp */
