//
//  NavBodyTransform.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/12/16.
//
//

#ifndef Overview_NavBodyTransform_hpp
#define Overview_NavBodyTransform_hpp

#include "PathTypes.hpp"

#include <ckm/mathtypes.hpp>

namespace cinek {
    namespace ove {
  
/**
 *  @class  NavBodyTransform
 *  @brief  An interface between the Nav System and an external System
 */
class NavBodyTransform
{
public:
    virtual ~NavBodyTransform() {}
    
    /**
     *  Retrieves transform information from an external provider
     *
     *  @param translate    The position on the object
     *  @param basis        Provides orientation for the object
     */
    virtual void getNavBodyTransform(ckm::vector3f& translate, ckm::matrix3f& basis) = 0;
    /**
     *  Sets transform information modified by the Driver system.
     *
     *  @param translate    The driver adjusted position
     *  @param basis        The driver adjusted basis matrix (orientation)
     */
    virtual void setNavBodyTransform(ckm::vector3f translate, ckm::matrix3f basis) = 0;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_NavBodyTransform_hpp */
