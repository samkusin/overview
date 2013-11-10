//
//  View.hpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_View_hpp
#define Overview_View_hpp

#include "cinek/overview/maptypes.h"

#include <memory>

namespace cinekine {
    namespace overview {
        class Map;
    }
    namespace glx {
        class Renderer;
    }
}

namespace cinekine {
    namespace ovengine {
    
    class Theater;
    
    //  The View renders a Viewpoint drives execution of the simulation based on an external application controller
    //  Currently the Engine only initializes and destroys the Director object
    class View
    {
    public:
        virtual ~View() {}

        //  When a new viewpoint is set by the Director.
        virtual void onMapSet(std::shared_ptr<overview::Map>& map,
                              const cinek_ov_pos& pos) = 0;
        //  When a controller updates the current viewpoint center
        virtual void onMapSetPosition(const cinek_ov_pos& pos) = 0;

        //  Executes rendering code specific to the View implementation.
        virtual void render() = 0;
    };
    
    //  must be defined by the implementing application.
    View* CreateView(Theater& theater, glx::Renderer& cli);
    //  must be defined by the implementing application - destroys the View created by
    //  CreateView
    void DestroyView(View* view);
        
    }   // namespace ovengine
}   // namespace cinekine


#endif
