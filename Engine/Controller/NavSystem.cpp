//
//  NavSystem.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/17/16.
//
//

#include "NavSystem.hpp"
#include "NavBody.hpp"

#include "Engine/Path/NavPath.hpp"
#include "Engine/Path/NavPathQuery.hpp"
#include "Engine/Path/Pathfinder.hpp"
#include "Engine/Debug.hpp"

#include <ckm/math.hpp>
#include <algorithm>

#include "Engine/System.inl"

namespace cinek {
    namespace ove {
    
template class System<NavBody, NavSystem>;
    
NavSystem::NavSystem(const InitParams& params) :
    System<NavBody, NavSystem>(params.numBodies),
    _pathfinder(params.pathfinder),
    _query(nullptr),
    _active(true)
{
    _bodies.reserve(params.numBodies);
}

NavSystem::~NavSystem()
{
    _pathfinder->cancelByListener(this);
}

void NavSystem::moveBodyToPosition(Entity entity, ckm::vector3 pos)
{
    const NavBody* body = findBody(entity);
    if (body) {
        _pathfinder->generatePath(this, entity, body->position(), pos);
    }
}

void NavSystem::activate()
{
    _active = true;
}

void NavSystem::deactivate()
{
    _active = false;
}
    
void NavSystem::startFrame()
{
    if (!_active)
        return;
    
    for (auto& body : _bodies) {
        body->pullTransform();
    }
}

void NavSystem::simulate(CKTimeDelta dt)
{
    if (!_active || _bodies.empty())
        return;
    
    if (!_query) {
        _query = _pathfinder->acquireQuery();
    }
    //  Each body travels from its current position to a target along a series
    //  of areas/volumes represented by the NavPath.   Using properties from
    //  NavBody, simulate adjusts the NavBody transform based on the body's
    //  current path.
    //
    for (auto& body : _bodies) {
        const NavPath& path = body->currentPath();
        if (path) {
            if (body->state() == NavBody::State::kPathStart) {
                body->pushTransformPosOrient(body->rotation(), path.startPos());
                body->runPath();
            }
            
            ckm::vector3 velocity;
            ckm::vector3 direction;
            ckm::vector3 angularVelocity;
            
            //  update path progress from the current transform
            ckm::vector3 extents { ckm::scalar(0), ckm::scalar(0.075), ckm::scalar(0) };
            dtPolyRef thisPoly = _query->findNearestWalkable(body->position(), extents);
            
            auto pathState = body->updatePath(thisPoly);
            
            if (body->state() == NavBody::State::kPathRun) {
                //  run path based on current body position and speed
                //  reorient if needed
                velocity = steer(path, body->position(), body->calcAbsoluteSpeed() * dt);
            
                ckm::normalize(direction, velocity);

                angularVelocity = turn(body->rotation(), direction, ckm::kPi * dt);
            }
            else {
                velocity.set(0,0,0);
                angularVelocity.set(0,0,0);
                direction.set(0,0,0);
                
                if (pathState == NavBody::State::kPathBreak) {
                    body->setToIdle();
                }
                else if (pathState == NavBody::State::kPathEnd) {
                    body->setToIdle();
                }
            }
            
            body->pushTransformVelocity(velocity, angularVelocity);
        }
    }
}

void NavSystem::endFrame()
{
    if (!_active)
        return;
}


ckm::vector3 NavSystem::steer
(
    const NavPath& path,
    const ckm::vector3& position,
    ckm::scalar dist
)
const
{
    ckm::vector3 velocity;
    
    constexpr int kNumSteerPoints = 3;
    float pathPoints[kNumSteerPoints * 3];
    unsigned char pathFlags[kNumSteerPoints];
    dtPolyRef pathPolys[kNumSteerPoints];
    
    NavPathQuery::PointList points;
    points.points = pathPoints;
    points.flags = pathFlags;
    points.polys = pathPolys;
    points.size = kNumSteerPoints;
    
    int pointIndex = _query->plotPath(points, path, position, dist);
    
    if (pointIndex >= 0 && pointIndex < kNumSteerPoints) {
        auto point = points.getPoint(pointIndex);
        ckm::vector3 target(point[0], point[1], point[2]);
        ckm::sub(velocity, target, position);
        ckm::normalize(velocity, velocity);
        ckm::scale(velocity, velocity, dist);
    }
    else {
        velocity.set(0, 0, 0);
    }
    
    return velocity;
}

ckm::vector3 NavSystem::turn
(
    const ckm::quat& orient,
    const ckm::vector3& direction,
    ckm::scalar rotation
)
const
{
    ckm::vector3 velocity;
    ckm::vector3 forward;
    ckm::forwardFromQuat(forward, orient);
    
    //  we should fine tune this for a more 2d approach
    //  this approach is a simple cross product to determine the angular
    //  vector, and some simple constraint checking
    
    auto angle = ckm::dot(forward, direction);
    if (angle > ckm::scalar(0.98)) {
        velocity.set(0,0,0);
    }
    else if (angle < ckm::scalar(-0.98)) {
        
        // this is making an assumption that any significant turns are
        // around the body's up vector
        ckm::upFromQuat(velocity, orient);
    }
    else {
        ckm::cross(velocity, forward, direction);
    }
    
    ckm::scale(velocity, velocity, rotation);
 
    return velocity;
}

void NavSystem::onPathfinderPathUpdate
(
    Entity entity,
    NavPath&& path
)
{
    NavBody* body = findBody(entity);
    if (body) {
        OVENGINE_LOG_INFO("NavSystem.onPathfinderPathUpdate - "
                      "Path for entity %" PRIu64 " generated of size %d\n",
                      entity, path.regionCount());
                      
        body->setPath(std::forward<NavPath>(path));
        body->setSpeedScalar(ckm::scalar(1));
    }
    else {
        //  we should've cancelled the path request if the body was being
        //  destroyed.
        OVENGINE_LOG_WARN("NavSystem.onPathfinderPathUpdate - "
                          "No NavBody found for entity %" PRIu64 "\n",
                          entity);
    }
}

void NavSystem::onPathfinderError
(
    Entity entity,
    PathfinderError error
)
{
}
    
    } /* namespace ove */
} /* namespace cinek */