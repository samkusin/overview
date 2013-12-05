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
        class FontLibrary;
        class BitmapLibrary;
    }
}

namespace cinekine {
    namespace ovengine {
    
    class Theater;

    /**
     * @struct ViewComponents
     * @brief  External components used by the View
     */
    struct ViewComponents
    {
        glx::FontLibrary* fontLibrary;
        glx::BitmapLibrary* bitmapLibrary;
    };
    
    //  The View renders a Viewpoint drives execution of the simulation based on 
    //  an external application controller.  Currently the Engine only initializes
    //  and destroys the Director object.
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

        /** Mouse button constants */
        enum MouseButton
        {
            kMouseButtonLeft,       /**< Left mouse button */
            kMouseButtonRight       /**< Right mouse button */
        };
        /**
         * Handles mouse down events within the view
         * @param button Button index
         * @param x      X coordinate within view
         * @param y      Y coordinate within view
         */
        virtual void onMouseButtonDown(MouseButton button, int32_t x, int32_t y) = 0;
        /**
         * Handles mouse up events within the view
         * @param button Button index
         * @param x      X coordinate within view
         * @param y      Y coordinatw within view
         */
        virtual void onMouseButtonUp(MouseButton button, int32_t x, int32_t y) = 0;

        /** The area where the mouse move event occurred */
        enum MouseRegion
        {
            kMouseRegionOver,       /**< Mouse motion within the view */
            kMouseRegionEnter,      /**< Mouse entered view */
            kMouseRegionExit        /**< Mouse exited view */
        };
        /**
         * Mouse movement event
         * @param region The mouse region where the motion occurred
         * @param x      X coordinate within view
         * @param y      Y coordinate within view
         */
        virtual void onMouseMove(MouseRegion region, int32_t x, int32_t y) = 0;
    };
    
    //  must be defined by the implementing application.
    View* CreateView(Theater& theater, glx::Renderer& cli, const ViewComponents& viewComponents);
    //  must be defined by the implementing application - destroys the View created by
    //  CreateView
    void DestroyView(View* view);
        
    }   // namespace ovengine
}   // namespace cinekine


#endif
