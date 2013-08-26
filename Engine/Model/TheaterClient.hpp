//
//  TheaterClient.hpp
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_TheaterClient_hpp
#define Overview_TheaterClient_hpp

#include <memory>

namespace cinekine {
    namespace overview {
        class Viewpoint;
        class Stage;
    }
}

namespace cinekine {
    namespace ovengine {
    
    //  The Client Interface (i.e to the Director object)
    class TheaterClient
    {
    public:
        virtual ~TheaterClient() {}
        
        /**
         *  Loads data into the current SpriteDatabase.
         *  @param spriteDbName The name of the DB (this translates to a JSON source file of
         *                      the same name.
         */
        virtual void loadSpriteDatabase(const char* spriteDbName) = 0;
        /**
         *  Loads a tile database (including bitmaps) into the global tile database.
         *  @param tileDbName   The name of the DB (this translates to a JSON source file of
         *                      the same name.
         */
        virtual void loadTileDatabase(const char* tileDbName) = 0;
        /**
         *  Sets the target view's Viewpoint. The ID passed in is passed on to the Renderer,
         *  allowing Views to access multiple Viewpoints.
         *  @param viewpointId  The Viewpoint ID (a constant shared between the client and the
         *                      renderer.
         *  @param viewpoint    The Viewpoint shared_ptr object to set.
         */
        virtual void setViewpoint(uint32_t viewpointId,
                                  std::shared_ptr<overview::Viewpoint>& viewpoint) = 0;
        /**
         *  Clears the target View's Viewpoint.
         *  @param viewpointId  The Viewpoint ID (a constant shared between the client and the
         *                      renderer.
         */
        virtual void clearViewpoint(uint32_t viewpointId) = 0;
    };
        
    }   // namespace ovengine
}   // namespace cinekine

#endif
