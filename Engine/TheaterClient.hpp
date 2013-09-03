//
//  TheaterClient.hpp
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_TheaterClient_hpp
#define Overview_TheaterClient_hpp

#include "cinek/overview/maptypes.h"

#include <memory>

namespace cinekine {
    namespace overview {
        class Viewpoint;
        class Map;
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
         *  Sets the target view's Map.
         *  @param map      The Map shared_ptr object to set.
         *  @param viewPos  The initial view position on the map.
         */
        virtual void setViewMap(std::shared_ptr<overview::Map> map,
                                const cinek_ov_pos& viewPos) = 0;
        /**
         *  Publishes an update to the current view position on the map.
         *  @param pos  The position in map cordinates.
         */
        virtual void setViewPos(const cinek_ov_pos& pos) = 0;
        /**
         *  Clears the target View's Map reference.
         */
        virtual void clearViewMap() = 0;
    };
        
    }   // namespace ovengine
}   // namespace cinekine

#endif
