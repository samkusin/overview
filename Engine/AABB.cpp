//
//  AABB.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "AABB.hpp"
#include "EngineTypes.hpp"

namespace cinek { namespace overview {

    template<>
    void AABB<Vector3>::clear()
    {
        min.comp[0] = min.comp[1] = min.comp[2] = 0.f;
        max.comp[0] = max.comp[1] = max.comp[2] = 0.f;
    }

    template<>
    bool AABB<Vector3>::inside(const AABB<Vector3>& box) const
    {
        return (min.comp[0] >= box.min.comp[0] && max.comp[0] <= box.max.comp[0] &&
                min.comp[1] >= box.min.comp[1] && max.comp[1] <= box.max.comp[1] &&
                min.comp[2] >= box.min.comp[2] && max.comp[2] <= box.max.comp[2]);
    }

    template<>
    bool AABB<Vector3>::contains(const point_type& pt) const
    {
        return (pt.comp[0] >= min.comp[0] && pt.comp[0] <= max.comp[0] &&
                pt.comp[1] >= min.comp[1] && pt.comp[1] <= max.comp[1] &&
                pt.comp[2] >= min.comp[2] && pt.comp[2] <= max.comp[2]);
    }

    template<>
    bool AABB<Vector3>::outside(const AABB<Vector3>& box) const
    {
        return (min.comp[0] > box.max.comp[0] || box.min.comp[0] > max.comp[0] ||
                min.comp[1] > box.max.comp[1] || box.min.comp[1] > max.comp[1] ||
                min.comp[2] > box.max.comp[2] || box.min.comp[2] > max.comp[2]);
    }
    
    template<>
    void AABB<Vector3>::generateVertices(std::array<point_type, 8>& verts)
    {
        //  -y vertices, clockwise from -x,-z
        verts[0].comp[0] = min.comp[0];     // v0 (-x,-y,-z)
        verts[0].comp[1] = min.comp[1];
        verts[0].comp[2] = min.comp[2];
        verts[1].comp[0] = min.comp[0];     // v1 (-x,-y,+z)
        verts[1].comp[1] = min.comp[1];
        verts[1].comp[2] = max.comp[2];
        verts[2].comp[0] = max.comp[0];     // v2 (+x,-y,+z)
        verts[2].comp[1] = min.comp[1];
        verts[2].comp[2] = max.comp[2];
        verts[3].comp[0] = max.comp[0];     // v3 (+x,-y,-z)
        verts[3].comp[1] = min.comp[1];
        verts[3].comp[2] = min.comp[2];
        
        //  +y vertices
        verts[4].comp[0] = min.comp[0];     // v4 (-x,+y,-z)
        verts[4].comp[1] = max.comp[1];
        verts[4].comp[2] = min.comp[2];
        verts[5].comp[0] = min.comp[0];     // v5 (-x,+y,+z)
        verts[5].comp[1] = max.comp[1];
        verts[5].comp[2] = max.comp[2];
        verts[6].comp[0] = max.comp[0];     // v6 (+x,+y,+z)
        verts[6].comp[1] = max.comp[1];
        verts[6].comp[2] = max.comp[2];
        verts[7].comp[0] = max.comp[0];     // v7 (+x,+y,-z)
        verts[7].comp[1] = max.comp[1];
        verts[7].comp[2] = min.comp[2];
    }

    template<>
    template<class _Matrix>
    void AABB<Vector3>::rotate(const _Matrix& mtxRot)
    {
        std::array<point_type, 8> box;
        generateVertices(box);
        
        for (auto& pt : box)
        {
            Vector4 pt4;
            pt4.comp[0] = pt.comp[0];
            pt4.comp[1] = pt.comp[1];
            pt4.comp[2] = pt.comp[2];
            pt4.comp[3] = 1.f;
            bx::vec4MulMtx(pt4, pt4, mtxRot);
            
            if (pt.comp[0] < min.comp[0])
                min.comp[0] = pt.comp[0];
            if (pt.comp[0] > max.comp[0])
                max.comp[0] = pt.comp[0];

            if (pt.comp[1] < min.comp[1])
                min.comp[1] = pt.comp[1];
            if (pt.comp[1] > max.comp[1])
                max.comp[1] = pt.comp[1];

            if (pt.comp[2] < min.comp[2])
                min.comp[2] = pt.comp[2];
            if (pt.comp[2] > max.comp[2])
                max.comp[2] = pt.comp[2];
        }
    }
    
    template<>
    AABB<Vector3>& AABB<Vector3>::merge(const AABB<Vector3>& aabb)
    {
        if (aabb.min.comp[0] < min.comp[0])
            min.comp[0] = aabb.min.comp[0];
        if (aabb.min.comp[1] < min.comp[1])
            min.comp[1] = aabb.min.comp[1];
        if (aabb.min.comp[2] < min.comp[2])
            min.comp[2] = aabb.min.comp[2];
        if (aabb.max.comp[0] > max.comp[0])
            max.comp[0] = aabb.max.comp[0];
        if (aabb.max.comp[1] > max.comp[1])
            max.comp[1] = aabb.max.comp[1];
        if (aabb.max.comp[2] > max.comp[2])
            max.comp[2] = aabb.max.comp[2];
        
        return *this;
    }

} /* namespace overview */ } /* namespace cinek */
