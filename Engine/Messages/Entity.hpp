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
#include "Engine/EngineTypes.hpp"

namespace cinek {
    namespace ove {
   
const ckmsg::ClassId kMsgEntityLoadDefinitions = 0x01000000;

struct EntityLoadDefinitionsRequest
{
    char name[32];
};

struct EntityLoadDefinitionsResponse
{
    char name[32];
};

    
    }   /* namespace ove */
}   /* namespace cinek */

#endif /* Overview_EntityMessages_hpp */
