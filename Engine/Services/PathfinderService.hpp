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
    PathfinderService(const PathfinderContext& context);
    ~PathfinderService();
    
    //  generates data for the pathfinder from existing scene data using the
    //  current application context
    //
    void generate(Pathfinder::GenerateCb cb);
    
    
    void update(double dt);
    
    //  debug methods
    void renderDebug(RenderService &renderService, const gfx::Camera& camera);
    
private:
    Scene* scene() { return _context.scene; }
    Pathfinder* pathfinder() { return _context.pathfinder; }
    PathfinderDebug* pathfinderDebug() { return _context.pathfinderDebug; }
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* PathfinderService_hpp */
