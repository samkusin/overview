//
//  GameScene.hpp
//  Overview
//
//  Created by Samir Sinha on 12/4/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Scenes_GameScene_hpp
#define Overview_Scenes_GameScene_hpp

#include "Scene.hpp"
#include "Engine/Component/Window/WindowComponentClient.hpp"

namespace cinekine {
    namespace prototype {
    
    class GameScene : public Scene
    {
    public:
        GameScene(ovengine::WindowComponentClient& windowClient);
        
        virtual void update();

    private:
        ovengine::WindowComponentClient& _windowClient;
        ovengine::WindowPtr _window;
    };
        
    }   // namespace ovengine
}   // namespace cinekine


#endif /* defined(Overview_Scenes_GameScene_hpp) */
