//
//  GameView.hpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_GameView_hpp
#define Overview_GameView_hpp

#include "Engine/View.hpp"
#include "Graphics/Graphics2D.hpp"

#include "cinek/cpp/ckalloc.hpp"
#include "cinek/overview/maptypes.h"

namespace cinekine {
    namespace ovengine {
        class TheaterCLI;
    }
    namespace glx {
        class RendererCLI;
        class FontLibrary;
        class BitmapLibrary;
    }
    namespace overview {
        class Map;
    }
}

namespace cinekine {
    namespace prototype {
        
    class GameView: public ovengine::View
    {
    public:
        GameView(ovengine::TheaterCLI& theater,
                    glx::RendererCLI& renderer,
                    glx::FontLibrary& fontLibrary,
                    glx::BitmapLibrary& bitmapLibrary);
        virtual ~GameView();
        
        void setMap(std::shared_ptr<overview::Map>& Map, const cinek_ov_pos& pos);
        void setMapPosition(const cinek_ov_pos& pos);
    
        virtual void render();

        virtual void onMouseButtonDown(MouseButton button, int32_t x, int32_t y);
        virtual void onMouseButtonUp(MouseButton button, int32_t x, int32_t y);
        virtual void onMouseMove(MouseRegion region, int32_t x, int32_t y);
        
    private:
        //  precalculates values used for rendering the local view
        void setupWorldSpace(const cinek_ov_pos& worldPos);
        //  utilities
        cinek_ov_pos xlatMapToWorldPos(const cinek_ov_pos& pos);
        cinek_ov_pos xlatWorldToMapPos(const cinek_ov_pos& pos);
        
    private:
        ovengine::TheaterCLI& _theater;
        glx::RendererCLI& _renderer;
        glx::Graphics2D _graphics;
        
        std::shared_ptr<overview::Map> _map;
        cinek_ov_bounds _worldViewBounds;
        cinek_ov_map_bounds _mapViewBounds;
    };
        
    }
}

#endif
