//
//  LoadAssetManifest.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/29/15.
//
//

#ifndef Overview_Task_LoadAssetManifest_hpp
#define Overview_Task_LoadAssetManifest_hpp

#include "Engine/AssetManifest.hpp"
#include "Engine/Tasks/LoadFile.hpp"

#include <cinek/allocator.hpp>

#include <vector>

namespace cinek {
    namespace ove {

class LoadAssetManifest : public LoadFile
{
public:
    static const UUID kUUID;
    
    LoadAssetManifest(EndCallback cb=0) : LoadFile(cb) {}
    LoadAssetManifest(std::string name, EndCallback cb=0);

    unique_ptr<AssetManifest> acquireManifest();
    
    virtual const TaskClassId& classId() const override { return kUUID; }
    
private:
    virtual void onFileLoaded() override;
    
private:
    unique_ptr<AssetManifest> _manifest;
};
    
    }  /* namespace ove */
}  /* namespace cinek */


#endif /* Overview_Task_LoadAssetManifest_hpp */
