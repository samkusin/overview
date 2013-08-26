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
        _theaterCLI(cli)
    {
        _theaterCLI.loadTileDatabase("dungeontiles");
        _theaterCLI.loadSpriteDatabase("sprites");
    }
    
    Overview::~Overview()
    {
        
    }
    
    void Overview::update()
    {

    }

    
    }   //  namespace prototype
}   // namespace cinekine
