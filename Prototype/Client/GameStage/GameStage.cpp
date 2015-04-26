//
//  GameStage.cpp
//  Overview
//
//  Created by Samir Sinha on 4/26/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "GameStage.hpp"


namespace cinek { namespace ovproto {

GameStage::GameStage()
{
}

GameStage::~GameStage()
{
}
    
overview::MessageBuffer GameStage::update
(
    overview::MessageBuffer msgBuffer,
    const overview::RenderResources& renderResources
)
{

    return std::move(msgBuffer);
}

} /* namespace ovproto */ } /* namespace cinek */