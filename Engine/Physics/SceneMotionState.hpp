//
//  SceneMotionState.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/6/15.
//
//

#ifndef Overview_SceneMotionState_hpp
#define Overview_SceneMotionState_hpp

#include "CKGfx/GfxTypes.hpp"

#include <bullet/LinearMath/btMotionState.h>

namespace cinek {
    namespace ove {

class SceneMotionState : public btMotionState
{
public:
    SceneMotionState(gfx::NodeHandle gfxNode);
    
    virtual void getWorldTransform(btTransform& worldTrans ) const;

    virtual void setWorldTransform(const btTransform& worldTrans);
    
private:
    gfx::NodeHandle _gfxNode;
};

    } /* namesapce ove */
} /* namespace cinek */

#endif /* SceneMotionState_hpp */
