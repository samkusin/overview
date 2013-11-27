//
//  GameView.hpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_GameView_hpp
#define Overview_GameView_hpp

#include "Common.hpp"
#include "Engine/View.hpp"
#include "Graphics/Graphics2D.hpp"

#include "cinek/cpp/ckalloc.hpp"

namespace cinekine {
    namespace overview {
        class Viewpoint;
    }
    namespace ovengine {
        class Theater;
    }
}

namespace cinekine {
    namespace prototype {
        
    class GameView: public ovengine::View
    {
    public:
        GameView(ovengine::Theater& theater, glx::Renderer& renderer,
                 const ovengine::ViewComponents& components);
        virtual ~GameView();
        
        virtual void onMapSet(std::shared_ptr<overview::Map>& Map,
                              const cinek_ov_pos& pos);
        virtual void onMapSetPosition(const cinek_ov_pos& pos);
    
        virtual void render();
        
    private:
        //  precalculates values used for rendering the local view
        void setupWorldSpace(const cinek_ov_pos& worldPos);
        //  utilities
        cinek_ov_pos xlatMapToWorldPos(const cinek_ov_pos& pos);
        cinek_ov_pos xlatWorldToMapPos(const cinek_ov_pos& pos);
        
    private:
        ovengine::Theater& _theater;
        glx::Renderer& _renderer;
        glx::Graphics2D _graphics;
        
        std::shared_ptr<overview::Map> _map;
        cinek_ov_bounds _worldViewBounds;
        cinek_ov_map_bounds _mapViewBounds;
    };
        
    }
}

#endif
