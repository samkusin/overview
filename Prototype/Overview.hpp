//
//  Overview.h
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Overview_hpp
#define Overview_Overview_hpp

#include "Engine/Director.hpp"

namespace cinekine {
    namespace ovengine {
        class TheaterClient;
    }
}

namespace cinekine {
    namespace prototype {
    
    class Overview: public ovengine::Director
    {
    public:
        Overview(ovengine::TheaterClient& cli);
        virtual ~Overview();
        
        virtual void update();
        
    private:
        ovengine::TheaterClient& _theaterCLI;
    };
    
    }
}

#endif
