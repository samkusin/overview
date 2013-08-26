//
//  GameView.cpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "GameView.hpp"
#include "Engine/Render/Renderer.hpp"

/////////////////////////////////////////////////////////////

namespace cinekine {
    namespace ovengine {
        //  must be defined by the implementing application.
        View* CreateView(Renderer& cli)
        {
            Allocator allocator;
            return allocator.newItem<prototype::GameView>(cli);
        }

        //  must be defined by the implementing application - destroys the View created by
        //  CreateView
        void DestroyView(View* view)
        {
            Allocator allocator;
            allocator.deleteItem(view);
        }
    }
}

namespace cinekine {
    namespace prototype {

    GameView::GameView(ovengine::Renderer& renderer) :
        _renderer(renderer)
    {
        
    }
    
    GameView::~GameView()
    {
        
    }
    
    //  A new viewpoint has been initialized by our controller.  Replace our reference with the new one.
    //
    void GameView::onViewpointSet(uint32_t viewpointId, std::shared_ptr<overview::Viewpoint> &viewpoint)
    {
        if (viewpointId == kViewpoint_Main)
        {
            _mainViewpoint = viewpoint;
        }
    }
     
    //  Main render pipeline
    //
    void GameView::render()
    {
    
    }
        
    }
}