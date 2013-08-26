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
#include <functional>

namespace cinekine {
    namespace overview {
        class Viewpoint;
    }
}

namespace cinekine {    
    namespace ovengine {
    
    class SpriteDatabaseLoader;
    class TileDatabaseLoader;
    class TheaterClient;

    //  The Theater drives execution of the Application.
    class Theater
    {
        CK_CLASS_NON_COPYABLE(Theater);

    public:
        Theater(const Allocator& allocator);
        ~Theater();
        
        TheaterClient& getClient();
        SpriteDatabaseLoader& getSpriteDatabaseLoader();
        TileDatabaseLoader& getTileDatabaseLoader();
        
        //  The Theater is attempting to publish a viewpoint with a given ID to the renderer.
        void onViewpointSet(std::function<void(uint32_t, std::shared_ptr<overview::Viewpoint>& viewpoint)> setCb);

    private:
        Allocator _allocator;
        class Impl;
        unique_ptr<Impl> _impl;
    };
        
        
    }   // namespace ovengine
}   // namespace cinekine

#endif
