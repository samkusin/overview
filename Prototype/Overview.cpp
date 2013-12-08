//
//  Overview.cpp
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Overview.hpp"

#include "cinek/cpp/ckalloc.hpp"

#include "SceneController.hpp"
#include "Scenes/GameScene.hpp"

#include <memory>


namespace cinekine {
    namespace ovengine {
    
    ovengine::Director* CreateDirector(TheaterCLI& theaterCLI,
                                       WindowComponentCLI& windowCLI,
                                       glx::RendererCLI& rendererCLI)
    {
        Allocator allocator;
        return allocator.newItem<prototype::Overview>(theaterCLI, windowCLI, rendererCLI);
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

    Overview::Overview(ovengine::TheaterCLI& theaterCLI,
                       ovengine::WindowComponentCLI& uiCLI,
                       glx::RendererCLI& rendererCLI) :
        _allocator(),
        _sceneController(theaterCLI, uiCLI, rendererCLI, _allocator)
    {
        _sceneController.add( "game",
            [this]() -> std::shared_ptr<Scene>
            {
                return std::allocate_shared<GameScene, std_allocator<GameScene>,
                                            SceneController&>
                        (
                            std_allocator<GameScene>(_allocator),
                            _sceneController
                        );
            });
        
        _sceneController.next("game");
    }
    
    Overview::~Overview()
    {
    }
    
    void Overview::update()
    {
        _sceneController.update(); 
    }

    
    }   //  namespace prototype
}   // namespace cinekine
