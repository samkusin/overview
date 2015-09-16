//
//  Renderable.hpp
//  Overview
//
//  Created by Samir Sinha on 5/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Component_Renderable_hpp
#define Overview_Entity_Component_Renderable_hpp

#include "Engine/Entity/EntityTypes.hpp"
#include "CKGfx/VertexTypes.hpp"

namespace cinek { namespace overview {

struct RenderableComponent
{
    COMPONENT_DEFINITION(RenderableComponent);

    gfx::Matrix4 worldSRT;
};

} /* namespace overview */ } /* namespace cinek */



#endif
