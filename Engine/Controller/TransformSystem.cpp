//
//  TransformSystem.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/10/16.
//
//

#include "TransformSystem.hpp"
#include "TransformBody.hpp"

#include "Engine/System.inl"

namespace cinek {
    namespace ove {
    
template class System<TransformBody, TransformSystem>;


TransformSystem::TransformSystem(const InitParams& params) :
    System(params.numBodies)
{
}

TransformSystem::~TransformSystem()
{
}


void TransformSystem::activate()
{
}

void TransformSystem::startFrame()
{
}

void TransformSystem::simulate(CKTimeDelta dt)
{
}

void TransformSystem::endFrame()
{
}

void TransformSystem::deactivate()
{
}

    } /* namespace ove */
} /* namespace cinek */