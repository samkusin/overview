//
//  GameScene.cpp
//  Overview
//
//  Created by Samir Sinha on 12/4/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "GameScene.hpp"

namespace cinekine {
    namespace prototype {
        
    GameScene::GameScene(unique_ptr<ovengine::UIWindow> window) :
        InteractiveScene(std::move(window))
    {
        
    }
    
    void GameScene::update()
    {
        
    }
        
    }   // namespace ovengine
}   // namespace cinekine
