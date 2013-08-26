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

namespace cinekine {
    namespace prototype {
        
    class GameView: public ovengine::View
    {
    public:
        GameView(ovengine::Renderer& renderer);
        virtual ~GameView();
        
        virtual void onViewpointSet(uint32_t viewpointId, std::shared_ptr<overview::Viewpoint>& viewpoint);
        virtual void render();
        
    private:
        ovengine::Renderer& _renderer;
        
        std::shared_ptr<overview::Viewpoint> _mainViewpoint;
    };
        
    }
}

#endif
