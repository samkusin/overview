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

#include "cinek/overview/stage.hpp"

namespace cinekine {
    namespace overview {
        class Viewpoint;
    }
    namespace ovengine {
        class TheaterClient;
    }
    namespace imgui {
        class ClientInterface;
    }
}

namespace cinekine {
    namespace prototype {
    
    class Overview: public ovengine::Director
    {
    public:
        Overview(ovengine::TheaterClient& cli, imgui::ClientInterface& imgui);
        virtual ~Overview();

        virtual void update();
        
    private:
        ovengine::TheaterClient& _theaterCLI;
        imgui::ClientInterface& _imguiCLI;
        Allocator _allocator;

        overview::Stage _stage;
        cinek_ov_pos _viewPos;

        int32_t _scrollPos1;
    };
    
    }
}

#endif
