//
//  Overview.cpp
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Overview.hpp"
#include "Engine/Model/TheaterClient.hpp"

#include "cinek/cpp/ckalloc.hpp"

#include "cinek/overview/stage.hpp"
#include "cinek/overview/viewpoint.hpp"

namespace cinekine {
    namespace ovengine {
    
    ovengine::Director* CreateDirector(TheaterClient& cli)
    {
        Allocator allocator;
        return allocator.newItem<prototype::Overview>(cli);
    }
    
    void DestroyDirector(ovengine::Director* director)
    {
        Allocator allocator;
        allocator.deleteItem(director);
    }
    
    }   // namespace ovengine
}

namespace cinekine {
    namespace prototype {

    Overview::Overview(ovengine::TheaterClient& cli) :
        _theaterCLI(cli),
        _allocator(),
        _stage({ 32, 32, 0, 0 }, _allocator)
    {
        _theaterCLI.loadTileDatabase("dungeontiles");
        _theaterCLI.loadSpriteDatabase("sprites");
        
        cinek_ov_pos pos = { 0, 0, 0 };
        cinek_ov_map_bounds bounds = { 14, 16, 0, 0 };
        
        _mainViewpoint = std::allocate_shared<overview::Viewpoint>(std_allocator<overview::Viewpoint>(_allocator),
                                                                   _stage.getMapPtr(),
                                                                   pos,
                                                                   bounds);
        
        _theaterCLI.setViewpoint(kViewpoint_Main, _mainViewpoint);
    }
    
    Overview::~Overview()
    {
        _theaterCLI.clearViewpoint(kViewpoint_Main);
    }
    
    void Overview::update()
    {

    }

    
    }   //  namespace prototype
}   // namespace cinekine
