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
        
    GameScene::GameScene(ovengine::WindowComponentClient& windowClient) :
        _windowClient(windowClient),
        _window(_windowClient.createWindow("static/ui/main.rml"))
    {
        
    }
    
    void GameScene::update()
    {
        
    }
        
    }   // namespace ovengine
}   // namespace cinekine
