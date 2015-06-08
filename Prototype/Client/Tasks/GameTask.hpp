//
//  GameTask.h
//  Overview
//
//  Created by Samir Sinha on 6/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Game_Task_hpp
#define Overview_App_Game_Task_hpp

#include "AppInterface.hpp"

#include <cinek/task.hpp>

namespace cinek { namespace ovproto {

class GameTask : public Task
{
public:
    GameTask(AppInterface api);

protected:
    void onBegin() override;
    void onUpdate(uint32_t deltaTimeMs) override;
    void onEnd() override;
    void onFail() override;
    
private:
    AppInterface _API;
    
    EntityId _bodyEntityId;
};


} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_App_Game_Task_hpp) */
