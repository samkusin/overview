//
//  Comp/Body.hpp
//  Overview
//
//  Created by Samir Sinha on 6/23/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Component_Body_hpp
#define Overview_Entity_Component_Body_hpp

namespace cinek { namespace overview {

namespace component
{
    struct Body
    {
        COMPONENT_DEFINITION(Body);
        
        float radius;
    };
}

} /* namespace overview */ } /* namespace cinek */


#endif
