//
//  Stage.hpp
//  Overview
//
//  Created by Samir Sinha on 4/15/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Stage_hpp
#define Overview_App_Stage_hpp

#include "MessageStream.hpp"
#include "Render/RenderResources.hpp"

namespace cinek { namespace overview {

class Stage
{
public:
    virtual ~Stage() {}
    
    virtual MessageBuffer update(MessageBuffer msgStream,
                                 const RenderResources& renderResources) = 0;
};


} /* namespace overview */ } /* namespace cinek */


#endif /* ifdef Overview_App_Stage_hpp */
