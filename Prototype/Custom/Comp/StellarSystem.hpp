//
//  StellarSystem.hpp
//  Overview
//
//  Created by Samir Sinha on 6/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Custom_Comp_StellarSystem_hpp
#define Overview_App_Custom_Comp_StellarSystem_hpp

#include "CustomComponentTypes.hpp"

namespace cinek { namespace ovproto {

class StellarSystemUtility;

struct StellarSystemComponent
{
    COMPONENT_DEFINITION(StellarSystemComponent);
    
    void init(uint32_t seed, ckm::scalar radius);
    
    uint32_t seed() const { return _seed; }
    ckm::scalar radiusInLYR() const { return _radius; }
    int indexToTreeNode() const { return _indexToTreeNode; }
    
private:
    friend class ::cinek::ovproto::StellarSystemUtility;
    
    //  seed used to generate planets for this system
    uint32_t _seed;
    //  filled in by the StellarSystemUtility
    int _indexToTreeNode;
    //  radius of the system
    ckm::scalar _radius;
};

} /* namespace overview */ } /* namespace cinek */

#endif
