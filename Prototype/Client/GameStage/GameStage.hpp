//
//  GameStage/GameStage.hpp
//  Overview
//
//  Created by Samir Sinha on 4/26/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Stage_Game_hpp
#define Overview_App_Stage_Game_hpp

#include "GameTypes.hpp"
#include "Client/Stage.hpp"
#include "MessageTypes.hpp"

#include <array>

namespace cinek { namespace gfx {

class Mesh;

} /* namespace gfx */ } /* namespace cinek */

namespace cinek { namespace ovproto {

class GameStage: public Stage
{
public:
    GameStage();
    virtual ~GameStage();
    
    virtual void update(overview::EngineContext& context) override;
    
private:
    Allocator _allocator;
    
    //void render(overview::RenderResources& resources);
};

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_App_Stage_Game_hpp) */
