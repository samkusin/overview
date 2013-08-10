//
//  Game.hpp

//  Overview
//
//  Created by Samir Sinha on 7/13/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Game_hpp
#define Overview_Game_hpp

namespace cinekine {
    namespace ovengine {

class Game
{
public:
    virtual ~Game() {}
    
    virtual void update() = 0;
};


    }   // namespace ovengine
}   // namespace cinekine


/* Output game, inputs memory, renderAsset, archive */
cinekine::ovengine::Game* OVEngine_CreateGame();

#endif
