//
//  TransformEntity.hpp
//  Overview
//
//  Created by Samir Sinha on 7/9/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Transform_Function_hpp
#define Overview_Entity_Transform_Function_hpp

#include "EntityTypes.hpp"
#include "EntityDataTable.hpp"
#include "Comp/Transform.hpp"

namespace cinek { namespace overview {

namespace component
{
    struct TransformEntity
    {
        Transform* operator()(Entity entity, Table<Transform> transforms);
        
        void all(Table<Transform> transforms);
        
    private:
        Table<Transform> _transforms;
        
        void runTransform(Transform& transform, const ckm::mat4& parentSRT);
    };
}

} /* namespace overview */ } /* namespace cinek */

#endif
