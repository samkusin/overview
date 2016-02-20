//
//  NavPathQuery.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/19/16.
//
//

#ifndef Overview_NavQuery_hpp
#define Overview_NavQuery_hpp

#include "Engine/Nav/PathTypes.hpp"
#include "Engine/Contrib/Recast/DetourNavMeshQuery.h"

namespace cinek {
    namespace ove {
    
    class NavPathQuery
    {
    public:
        NavPathQuery(const NavMesh* mesh, int numNodes);
        
        //  the total number of nodes created by this query
        int nodeLimit() const;
        
        //  set's up node filters used during the next query
        void setupFilters(uint16_t includeNodeMask);
        
        //  is the point 'walkable' - i.e. on the walk mesh
        bool isWalkable(ckm::vector3f pos, ckm::vector3f extents) const;
                
    private:
        detour_nav_query_unique_ptr _query;
        dtQueryFilter _filter;
    };
        
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_NavQuery_hpp */
