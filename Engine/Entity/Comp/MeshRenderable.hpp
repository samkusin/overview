//
//  MeshRenderable.hpp
//  Overview
//
//  Created by Samir Sinha on 5/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Component_MeshRenderable_hpp
#define Overview_Entity_Component_MeshRenderable_hpp

#include "Engine/Entity/EntityTypes.hpp"
#include "CKGfx/VertexTypes.hpp"

namespace cinek { namespace overview {

namespace component
{
    struct MeshRenderable
    {
        COMPONENT_DEFINITION(MeshRenderable);

        cinek::gfx::MeshHandle::pod_type meshHandle;
        cinek::gfx::TextureHandle::pod_type texHandle;
    };
}

} /* namespace overview */ } /* namespace cinek */



#endif
