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
        
        //  returns the resident query filter object
        const dtQueryFilter& filter() const { return _filter; }
        
        //  set's up node filters used during the next query
        void setupFilters(uint16_t includeNodeMask);
        
        //  is the point 'walkable' - i.e. on the walk mesh
        bool isWalkable(ckm::vector3f pos, ckm::vector3f extents) const;
        
        struct PointList
        {
            float *points;
            unsigned char *flags;
            dtPolyRef *polys;
            int size;
            
            float* getPoint(int index);
        };
        
        int plotPath(PointList& outPointList,
                     const NavPath& path,
                     const ckm::vector3f& pos,
                     ckm::scalar dist) const;

        
        //  used for fine-tuning the query (i.e. a direct interface to the
        //  low-level query system.
        dtNavMeshQuery& interface() { return *_query; }
        const dtNavMeshQuery& interface() const { return *_query; }
        
    private:
        detour_nav_query_unique_ptr _query;
        dtQueryFilter _filter;
    };
        
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_NavQuery_hpp */
