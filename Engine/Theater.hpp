//
//  Theater.hpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Theater_hpp
#define Overview_Theater_hpp

#include "cinek/cpp/ckalloc.hpp"
#include "cinek/overview/maptypes.h"
#include <functional>

namespace cinekine {
    namespace rendermodel {
        class TileDatabase;
        class SpriteDatabase;
    }
    namespace overview {
        class Map;
    }
}

namespace cinekine {    
    namespace ovengine {
    
    class SpriteDatabaseLoader;
    class TileDatabaseLoader;
    class TheaterCLI;

    //  The Theater drives execution of the Application.
    class Theater
    {
        CK_CLASS_NON_COPYABLE(Theater);

    public:
        Theater(const Allocator& allocator);
        ~Theater();
        
        TheaterCLI& getCLI();
        
    private:
        Allocator _allocator;
        class Impl;
        unique_ptr<Impl> _impl;
    };
        
        
    }   // namespace ovengine
}   // namespace cinekine

#endif
