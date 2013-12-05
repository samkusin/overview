//
//  GameScene.hpp
//  Overview
//
//  Created by Samir Sinha on 12/4/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Scenes_GameScene_hpp
#define Overview_Scenes_GameScene_hpp

#include "../InteractiveScene.hpp"
#include "Engine/UIWindow.hpp"

namespace cinekine {
    namespace prototype {
    
    class GameScene : public InteractiveScene
    {
    public:
        GameScene(unique_ptr<ovengine::UIWindow> window);
        
        virtual void update();
    };
        
    }   // namespace ovengine
}   // namespace cinekine


#endif /* defined(Overview_Scenes_GameScene_hpp) */
