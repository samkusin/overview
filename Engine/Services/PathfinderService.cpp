//
//  PathfinderService.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/9/16.
//
//

#include "PathfinderService.hpp"
#include "Engine/Debug.hpp"
#include "Engine/Services/RenderService.hpp"

#include "Engine/Nav/PathfinderDebug.hpp"

namespace cinek {
    namespace ove {
    
PathfinderService::PathfinderService(PathfinderContext context) :
    _context(context)
{
}

PathfinderService::~PathfinderService()
{
}

void PathfinderService::generate(Pathfinder::GenerateCb callback)
{
    CK_ASSERT_RETURN(pathfinder() != nullptr);
    CK_ASSERT_RETURN(scene() != nullptr);
    pathfinder()->generateFromScene(*scene(), std::move(callback));
}

void PathfinderService::renderDebug
(
    RenderService &renderService,
    const gfx::Camera& camera
)
{
    const RenderContext& renderContext = renderService.renderContext();
    pathfinderDebug()->setup(renderContext.programs, renderContext.uniforms, &camera, nullptr);
    pathfinder()->updateDebug(*pathfinderDebug());
}

bool PathfinderService::isPositionWalkable
(
    ckm::vector3f pos,
    ckm::vector3f extents
)
{
    return pathfinder()->isLocationWalkable(pos, extents);
}

    
    } /* namespace ove */
} /* namespace cinek */
