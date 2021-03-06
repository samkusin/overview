//
//  InitializeScene.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/8/16.
//
//

#ifndef Overview_Task_InitializeScene_hpp
#define Overview_Task_InitializeScene_hpp

#include "Engine/SceneJsonLoader.hpp"
#include "CKGfx/GfxTypes.hpp"

#include <cinek/task.hpp>
#include <cinek/allocator.hpp>

namespace cinek {
    namespace ove {

class InitializeScene : public Task
{
public:
    static const UUID kUUID;
 
    static unique_ptr<InitializeScene> create
    (
        std::shared_ptr<AssetManifest> inputManifest,
        SceneJsonLoader loader,
        EndCallback cb
    );
    
    InitializeScene
    (
        std::shared_ptr<AssetManifest> inputManifest,
        SceneJsonLoader loader,
        EndCallback cb
    );
    
    SceneJsonLoader::SceneBodyList acquireBodyList();
    
    virtual const TaskClassId& classId() const override { return kUUID; }

protected:
    virtual void onUpdate(uint32_t deltaTimeMs) override;

private:
    std::shared_ptr<AssetManifest> _manifest;
    SceneJsonLoader _loader;
    SceneJsonLoader::SceneBodyList _bodyList;
};
    
    }  /* namespace ove */
}  /* namespace cinek */


#endif /* Overview_Task_InitializeScene_hpp */
