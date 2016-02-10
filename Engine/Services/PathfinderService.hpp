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
#include "Engine/Path/Pathfinder.hpp"


namespace cinek {
    namespace ove {
    
struct PathfinderContext
{
    Scene* scene;
    Pathfinder* pathfinder;
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
    
private:
    Scene* scene() { return _context.scene; }
    Pathfinder* pathfinder() { return _context.pathfinder; }
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* PathfinderService_hpp */
