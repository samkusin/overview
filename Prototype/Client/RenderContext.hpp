//
//  RemderContext.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Render_Context_hpp
#define Overview_App_Render_Context_hpp

#include "GameTypes.hpp"
#include "UI/UITypes.hpp"
#include "Engine/Render/RenderTypes.hpp"

namespace cinek {
    namespace ovproto {

    struct RenderObjects
    {
        //  graphics
        overview::RenderResources* renderResources;
        gfx::Rect viewRect;
        NVGcontext* nvg;
    };
    
    class RenderContext
    {
    public:
        RenderContext(): _o(nullptr) {}
        RenderContext(RenderObjects& objs) : _o(&objs) {}
        
    private:
        friend class RenderService;
        
        RenderObjects* _o;
    };

    } /* namespace ovproto */
} /* namespace cinek */

#endif
