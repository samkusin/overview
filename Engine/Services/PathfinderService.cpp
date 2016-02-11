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

#include "Engine/Path/PathfinderDebug.hpp"

namespace cinek {
    namespace ove {
    
PathfinderService::PathfinderService(const PathfinderContext& context) :
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

    
    } /* namespace ove */
} /* namespace cinek */
