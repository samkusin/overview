//
//  PathfinderService.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/9/16.
//
//

#ifndef Overview_PathfinderService_hpp
#define Overview_PathfinderService_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/Nav/Pathfinder.hpp"
#include "CKGfx/GfxTypes.hpp"


namespace cinek {
    namespace ove {
    
struct PathfinderContext
{
    Scene* scene;
    Pathfinder* pathfinder;
    PathfinderDebug* pathfinderDebug;
    
};
    
class PathfinderService
{
    PathfinderContext _context;
    
public:
    PathfinderService(PathfinderContext context);
    ~PathfinderService();
    
    //  generates data for the pathfinder from existing scene data using the
    //  current application context
    //
    void generate(Pathfinder::GenerateCb cb);
    
    //  determines if a region is 'walkable', meaning if that point in isolation
    //  is walkable.  this does not check if an entity can walk *to* the target
    bool isPositionWalkable(ckm::vector3f pos, ckm::vector3f extents);
    
    //  debug methods
    void renderDebug(RenderService &renderService, const gfx::Camera& camera);
    
private:
    Scene* scene() { return _context.scene; }
    Pathfinder* pathfinder() { return _context.pathfinder; }
    const Pathfinder* pathfinder() const { return _context.pathfinder; }
    PathfinderDebug* pathfinderDebug() { return _context.pathfinderDebug; }
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* PathfinderService_hpp */
