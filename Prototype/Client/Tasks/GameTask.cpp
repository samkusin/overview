//
//  GameTask.cpp
//  Overview
//
//  Created by Samir Sinha on 6/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "GameTask.hpp"
#include "EntityObject.hpp"

namespace cinek { namespace ovproto {

GameTask::GameTask(AppInterface api) :
    _API(api)
{
}

void GameTask::onBegin()
{
    _API.createJsonDocumentFromFile(kDocumentId_EntityTemplates, "entity.json");
    
    overview::EntityObject* entityObj = _API.createEntity(
        kDocumentId_EntityTemplates,
        "star"
    );
    
    //  position our new star entity - note, this is a brute force method that
    //  bypasses any notification, etc.  use a proper physics system or add
    //  a utility to manipulate entities that sends out the appropriate
    //  notification
    
    auto entityTransform = &entityObj->transform();
    bx::mtxTranslate(entityTransform->matrix, 0.f, -0.5f, 0.f);
    
    entityObj = _API.createEntity(
        kDocumentId_EntityTemplates,
        "star"
    );
    
    entityTransform = &entityObj->transform();
    bx::mtxTranslate(entityTransform->matrix, 0.f, 1.f, 1.f);
}

void GameTask::onUpdate(uint32_t deltaTimeMs)
{
    
}

void GameTask::onEnd()
{
}

void GameTask::onFail()
{
}


} /* namespace ovproto */ } /* namespace cinek */
