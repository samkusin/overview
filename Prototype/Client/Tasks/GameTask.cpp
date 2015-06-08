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
    _API(api),
    _bodyEntityId(0)
{
}

void GameTask::onBegin()
{
    _API.createJsonDocumentFromFile(kDocumentId_EntityTemplates, "entity.json");
    
    overview::EntityObject* entityObj = _API.createEntity(
        kDocumentId_EntityTemplates,
        "star"
    );
    _bodyEntityId = entityObj->id();
    
    //  position our new star entity - note, this is a brute force method that
    //  bypasses any notification, etc.  use a proper physics system or add
    //  a utility to manipulate entities that sends out the appropriate
    //  notification
    
    
    auto entityTransform = &entityObj->transform();
    bx::mtxSRT(entityTransform->matrix, 2.0f, 2.0f, 2.0f, 0.f, 0.5*bx::pi, -0.25*bx::pi,
        0.f, 0.f, 0.f);


    //bx::mtxScale(mtx, 2.0f, 2.0f, 2.0f);
    //bx::mtxMul(entityTransform->matrix, entityTransform->matrix, mtx);
    //bx::mtxTranslate(mtx, 0.0f, 0.0f, 0.0f);
    //bx::mtxMul(entityTransform->matrix, entityTransform->matrix, mtx);
    
    /*
    entityObj = _API.createEntity(
        kDocumentId_EntityTemplates,
        "star"
    );
    
    entityTransform = &entityObj->transform();
    bx::mtxRotateXYZ(entityTransform->matrix, 0.0f, 0.0f, bx::pi);
    bx::mtxTranslate(mtx, 2.f, 2.f, 0.f);
    bx::mtxMul(entityTransform->matrix, entityTransform->matrix, mtx);
    */
}

void GameTask::onUpdate(uint32_t deltaTimeMs)
{
    overview::Matrix4 rotMat;
    //bx::mtxIdentity(rotMat);
    bx::mtxRotateXYZ(rotMat, -bx::piHalf*0.25f*deltaTimeMs/1000.f, bx::piHalf*0.25f*deltaTimeMs/1000.f, 0.f);
    
    auto object = _API.entityById(_bodyEntityId);
    if (object)
    {
        auto entityTransform = &object->transform();
        overview::Matrix4 transform;
        
        bx::mtxMul(transform, entityTransform->matrix, rotMat);
        entityTransform->matrix = transform;
    }
}

void GameTask::onEnd()
{
}

void GameTask::onFail()
{
}


} /* namespace ovproto */ } /* namespace cinek */
