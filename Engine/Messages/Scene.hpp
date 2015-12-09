//
//  Scene.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/2/15.
//
//

#ifndef Overview_Messages_Scene_hpp
#define Overview_Messages_Scene_hpp

#include "Core.hpp"

namespace cinek {
    namespace ove {
   
const ckmsg::ClassId kMsgSceneLoad = MakeMessageClassId(kMsgClassScene, 0x0001);

struct SceneLoadRequest
{
    char name[64];
};

struct SceneLoadResponse
{
    char name[64];
};

    
    }   /* namespace ove */
}   /* namespace cinek */


#endif /* Overview_Messages_Scene_hpp */
