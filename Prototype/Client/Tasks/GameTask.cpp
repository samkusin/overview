//
//  GameTask.cpp
//  Overview
//
//  Created by Samir Sinha on 6/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "GameTask.hpp"

//  used for building a galactic "cell"
#include "Sim/BuildStarmap.hpp"
#include "Engine/BVH/AABBTree.hpp"

#include "Engine/Entity/EntityStore.hpp"
#include "Engine/Entity/Comp/Transform.hpp"
#include "Engine/Entity/Comp/EntityHierarchy.hpp"
#include "Engine/Entity/Comp/Body.hpp"
#include "Custom/Comp/StellarSystem.hpp"

#include <cinek/debug.hpp>
#include <cinek/vector.hpp>
#include <random>


namespace cinek {

using namespace overview;

namespace ovproto {

GameTask::GameTask(AppInterface api) :
    _API(api),
    _bodyEntityId { 0 }
{
}

void GameTask::onBegin()
{
    _API.createJsonDocumentFromFile(kDocumentId_EntityTemplates, "entity.json");
    
    _bodyEntityId = _API.createEntity(
        kDocumentId_EntityTemplates,
        "star"
    );
    
    //  position our new star entity - note, this is a brute force method that
    //  bypasses any notification, etc.  use a proper physics system or add
    //  a utility to manipulate entities that sends out the appropriate
    //  notification
    
    /*
    auto& entityMatrix = entityObj->matrix();
    bx::mtxSRT(entityMatrix, 2.0f, 2.0f, 2.0f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f);
    */
    
    BuildStarmapFunction cellGenFn;
    
    vector<SpectralClass> spectralClasses =
    {
        {   55000, 275.0,  38.0, 1000000,    0.0,  0,  SpectralClass::kWeibull  },
        {   30000,  16.0,   6.6,   30000,   15.0,  5,  SpectralClass::kWeibull  },
        {   10000,   2.1,   1.8,      25,   20.0,  5,  SpectralClass::kUniform  },
        {   7500,    1.4,   1.4,       5,   25.0,  4,  SpectralClass::kUniform  },
        {   6000,   1.04,  1.15,     1.5,   30.0,  3,  SpectralClass::kUniform  },
        {   5200,    0.8,  0.96,     0.6,   35.0,  3,  SpectralClass::kUniform  },
        {   3700,   0.45,   0.7,    0.08,   45.0,  2,  SpectralClass::kUniform  },
        {   2400,   0.08,   0.1,  5.3e-6,   50.0,  1,  SpectralClass::kUniform  }
    };
    /*
    vector<BuildCellFunction::SpectralInput> spectralInputs =
    {
        { 0.0 },
        { 0.00003 },
        { 0.13 },
        { 0.6 },
        { 3.0 },
        { 7.6 },
        { 12.1 },
        { 76.45 }
    };
    */
    constexpr double kSolarMassTotal = 20.0;
    vector<BuildStarmapFunction::SpectralInput> spectralInputs =
    {
        { 0.0 },
        { kSolarMassTotal * 0.0 },
        { kSolarMassTotal * 0.01 },
        { kSolarMassTotal * 0.09 },
        { kSolarMassTotal * 0.25 },
        { kSolarMassTotal * 0.35 },
        { kSolarMassTotal * 0.20 },
        { kSolarMassTotal * 0.10 }
    };
    ckm::vec3 cellMin( -8,-8,-8 );
    ckm::vec3 cellMax( 8, 8, 8 );
    AABB<ckm::vec3> cellBounds(cellMin, cellMax);
    
    auto cell = cellGenFn(10245,
        spectralClasses,
        spectralInputs,
        7,
        cellBounds,
        0.1, 1.5);
    
    

    //  create entities from cell
    //  iterate through systems, creating entities from systems and bodies
    

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
    //Matrix4 rotMat;
    //bx::mtxIdentity(rotMat);
    //bx::mtxRotateXYZ(rotMat, 0.f, bx::piHalf*0.10f*deltaTimeMs/1000.f, 0.f);
    
    /*
    auto object = _API.entityById(_bodyEntityId);
    if (object)
    {
        auto& entityMatrix = object->matrix();
        Matrix4 transform;
        
        bx::mtxMul(transform, entityMatrix, rotMat);
        object->matrix() = transform;
    }
    */
}

void GameTask::onEnd()
{
}

void GameTask::onFail()
{
}


} /* namespace ovproto */ } /* namespace cinek */
