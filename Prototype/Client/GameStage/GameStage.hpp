//
//  GameStage/GameStage.hpp
//  Overview
//
//  Created by Samir Sinha on 4/26/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Stage_Game_hpp
#define Overview_App_Stage_Game_hpp

#include "Client/Stage.hpp"

namespace cinek { namespace ovproto {

class GameStage: public overview::Stage
{
public:
    GameStage();
    virtual ~GameStage();
    
    virtual overview::MessageBuffer update(
        overview::MessageBuffer msgStream,
        const overview::RenderResources& renderResources
    );
};

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_App_Stage_Game_hpp) */
