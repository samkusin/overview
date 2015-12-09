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
#include <ckio/file.h>

#include <string>

namespace cinek {
    namespace ove {

class LoadTextureAsset : public LoadFile
{
public:
    static const UUID kUUID;
    
    LoadTextureAsset(std::string name, EndCallback cb=0);
    
    gfx::Texture acquireTexture();
    
    virtual const TaskClassId& classId() const override { return kUUID; }
    
protected:
    virtual void onFileLoaded() override;
    
private:
    gfx::Texture _texture;
};
    
    }  /* namespace ove */
}  /* namespace cinek */


#endif /* Overview_Task_LoadAssetManifest_hpp */
