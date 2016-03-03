//
//  NavBodyTransform.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/12/16.
//
//

#ifndef Overview_Controller_NavBodyTransform_hpp
#define Overview_Controller_NavBodyTransform_hpp

#include "ControllerTypes.hpp"

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
     *  @param linear    The linear velocity (translate)
     *  @param angular   The angular velocity (orientation)
     */
    virtual void setNavBodyTransform(ckm::vector3f translate, ckm::matrix3f basis) = 0;
    /**
     *  Sets transform information modified by the Driver system.
     *
     *  @param linear    The linear velocity (translate)
     *  @param angular   The angular velocity (orientation)
     */
    virtual void setNavBodyVelocity(ckm::vector3f linear, ckm::vector3f angular) = 0;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_Controller_NavBodyTransform_hpp */