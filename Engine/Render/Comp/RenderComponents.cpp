//
//  RenderComponents.cpp
//  Overview
//
//  Created by Samir Sinha on 8/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "Engine/Entity/EntityTypes.hpp"
#include "Renderable.hpp"
#include "MeshRenderable.hpp"

namespace cinek { namespace overview {

namespace component
{
    COMPONENT_TYPE_IMPL(Renderable, kRenderable, {});
    COMPONENT_TYPE_IMPL(MeshRenderable, kMeshRenderable, {});
}

} /* namespace overview */ } /* namespace cinek */
