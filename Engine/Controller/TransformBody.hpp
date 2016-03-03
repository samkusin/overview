//
//  TransformBody.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/1/16.
//
//

#ifndef Overview_Controller_TransformBody_hpp
#define Overview_Controller_TransformBody_hpp

#include "ControllerTypes.hpp"

#include <string>

namespace cinek {
    namespace ove {

/**
 *  @class  TransformBody
 *  @brief  Component for the AnimBody system defining transform animations for
 *          an entity.
 *
 *  @detail Defines transform-based animations for an entity over time.  Every
 *          animation can transform position and rotation multiple times over
 *          a time interval. 
 *
 */
 
 class TransformBody
 {
 private:
    CKTime _startTime;
    TransformIndex _transformIndex;
    TransformSetHandle _transformSet;
 };
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_Controller_TransformBody_hpp */
