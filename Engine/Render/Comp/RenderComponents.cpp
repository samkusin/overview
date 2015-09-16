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

    COMPONENT_TYPE_IMPL(RenderableComponent, kRenderableComponent, {});
    COMPONENT_TYPE_IMPL(MeshRenderableComponent, kMeshRenderableComponent, {});

} /* namespace overview */ } /* namespace cinek */
