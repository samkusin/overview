//
//  TheaterClient.hpp
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_TheaterClient_hpp
#define Overview_TheaterClient_hpp

namespace cinekine {
    namespace ovengine {
    
    //  The Client Interface (i.e to the Director object)
    class TheaterClient
    {
    public:
        virtual ~TheaterClient() {}
        
        //  loads a sprite database (including bitmaps) into the global sprite database.
        virtual void loadSpriteDatabase(const char* spriteDbName) = 0;
        
        //  loads a tile database (including bitmaps) into the global tile database.
        virtual void loadTileDatabase(const char* tileDbName) = 0;
    };
        
    }   // namespace ovengine
}   // namespace cinekine

#endif
