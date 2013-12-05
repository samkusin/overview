//
//  Overview.h
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Overview_hpp
#define Overview_Overview_hpp

#include "Common.hpp"
#include "Engine/Director.hpp"
#include "Engine/SceneController.hpp"

#include "cinek/overview/stage.hpp"

namespace cinekine {
    namespace overview {
        class Viewpoint;
    }
    namespace ovengine {
        class TheaterClient;
        class UIController;
    }
}

namespace cinekine {
    namespace prototype {
    
    class Overview: public ovengine::Director
    {
    public:
        Overview(ovengine::TheaterClient& cli, ovengine::UIClient& uiCLI);
        virtual ~Overview();

        virtual void update();
        
    private:
        ovengine::TheaterClient& _theaterCLI;
        ovengine::UIClient& _UICLI;
        Allocator _allocator;
        ovengine::SceneController _sceneController;

        overview::Stage _stage;
        cinek_ov_pos _viewPos;
    };
    
    }
}

#endif
