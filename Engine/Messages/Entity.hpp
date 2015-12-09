//
//  Messages/Entity.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/28/15.
//
//

#ifndef Overview_EntityMessages_hpp
#define Overview_EntityMessages_hpp

#include "Core.hpp"

namespace cinek {
    namespace ove {
   
const ckmsg::ClassId kMsgEntityLoadDefinitions = MakeMessageClassId(kMsgClassEntity, 0x0001);

struct EntityLoadDefinitionsRequest
{
    char name[64];
};

struct EntityLoadDefinitionsResponse
{
    char name[64];
};

    
    }   /* namespace ove */
}   /* namespace cinek */

#endif /* Overview_EntityMessages_hpp */
