//
//  GfxTypes.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/26/15.
//
//

#include "GfxTypes.hpp"

#include "Texture.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Animation.hpp"
#include "AnimationController.hpp"
#include "Light.hpp"
#include "Node.hpp"
#include "NodeGraph.hpp"
#include "ModelSet.hpp"

#include <ckm/math.hpp>
#include <ckm/aabb.hpp>

#include <cinek/debug.h>
#include <bx/fpumath.h>

#include <cinek/objectpool.inl>
#include <cinek/managed_handle.inl>



namespace cinek {

    //  necessary template impls for now - the ManagedObjectPool needs to have
    //  the definition of the object its pooling, and the managed handle needs
    //  the ManagedObjectPool implementation to compile and link.
    

    template class ObjectPool<gfx::Texture>;
    template class ManagedObjectPool<gfx::Texture, void>;
    template void ManagedHandle<gfx::Texture, ManagedObjectPool<gfx::Texture, void>>::acquire();
    template void ManagedHandle<gfx::Texture, ManagedObjectPool<gfx::Texture, void>>::release();
    template class ManagedObjectPoolBase<gfx::Texture, ManagedObjectPool<gfx::Texture, void>>;
    
    template class ObjectPool<gfx::Material>;
    template class ManagedObjectPool<gfx::Material, void>;
    template void ManagedHandle<gfx::Material, ManagedObjectPool<gfx::Material, void>>::acquire();
    template void ManagedHandle<gfx::Material, ManagedObjectPool<gfx::Material, void>>::release();
    template class ManagedObjectPoolBase<gfx::Material, ManagedObjectPool<gfx::Material, void>>;
    
    template class ObjectPool<gfx::AnimationSet>;
    template class ManagedObjectPool<gfx::AnimationSet, void>;
    template void ManagedHandle<gfx::AnimationSet, ManagedObjectPool<gfx::AnimationSet, void>>::acquire();
    template void ManagedHandle<gfx::AnimationSet, ManagedObjectPool<gfx::AnimationSet, void>>::release();
    template class ManagedObjectPoolBase<gfx::AnimationSet, ManagedObjectPool<gfx::AnimationSet, void>>;
    
    template class ObjectPool<gfx::AnimationController>;
    template class ManagedObjectPool<gfx::AnimationController, void>;
    template void ManagedHandle<gfx::AnimationController, ManagedObjectPool<gfx::AnimationController, void>>::acquire();
    template void ManagedHandle<gfx::AnimationController, ManagedObjectPool<gfx::AnimationController, void>>::release();
    template class ManagedObjectPoolBase<gfx::AnimationController, ManagedObjectPool<gfx::AnimationController, void>>;
    
    template class ObjectPool<gfx::Mesh>;
    template class ManagedObjectPool<gfx::Mesh, void>;
    template void ManagedHandle<gfx::Mesh, ManagedObjectPool<gfx::Mesh, void>>::acquire();
    template void ManagedHandle<gfx::Mesh, ManagedObjectPool<gfx::Mesh, void>>::release();
    template class ManagedObjectPoolBase<gfx::Mesh, ManagedObjectPool<gfx::Mesh, void>>;
    
    template class ObjectPool<gfx::Light>;
    template class ManagedObjectPool<gfx::Light, void>;
    template void ManagedHandle<gfx::Light, ManagedObjectPool<gfx::Light, void>>::acquire();
    template void ManagedHandle<gfx::Light, ManagedObjectPool<gfx::Light, void>>::release();
    template class ManagedObjectPoolBase<gfx::Light, ManagedObjectPool<gfx::Light, void>>;
    
    /* Use 16-byte alignment for nodes - matrix alignment conforms to SSE 128-bit ops */
    template class ObjectPool<gfx::Node, 16>;
    template class ManagedObjectPool<gfx::Node, gfx::NodeGraph*, 16>;
    template void ManagedHandle<gfx::Node, ManagedObjectPool<gfx::Node, gfx::NodeGraph*, 16>>::acquire();
    template void ManagedHandle<gfx::Node, ManagedObjectPool<gfx::Node, gfx::NodeGraph*, 16>>::release();
    template class ManagedObjectPoolBase<gfx::Node, ManagedObjectPool<gfx::Node, gfx::NodeGraph*, 16>, 16>;

    template class ObjectPool<gfx::ModelSet>;
    template class ManagedObjectPool<gfx::ModelSet, void>;
    template void ManagedHandle<gfx::ModelSet, ManagedObjectPool<gfx::ModelSet, void>>::acquire();
    template void ManagedHandle<gfx::ModelSet, ManagedObjectPool<gfx::ModelSet, void>>::release();
    template class ManagedObjectPoolBase<gfx::ModelSet, ManagedObjectPool<gfx::ModelSet, void>>;


    namespace gfx {
    
    AABB transformAABB(const AABB& aabb, const Matrix4& mtx)
    {
        AABB newAABB;
        
        //  todo - optimize
        std::array<Vector3, 8> vertices;
        aabb.generateVertices(vertices);
        
        Vector3& min = newAABB.min;
        Vector3& max = newAABB.max;
        
        for (auto& vertex : vertices) {
            Vector3 pt;
            bx::vec3MulMtx(pt, vertex, mtx);
            
            if (pt.x < min.x)
                min.x = pt.x;
            if (pt.x > max.x)
                max.x = pt.x;

            if (pt.y < min.y)
                min.y = pt.y;
            if (pt.y > max.y)
                max.y = pt.y;

            if (pt.z < min.z)
                min.z = pt.z;
            if (pt.z > max.z)
                max.z = pt.z;
        }
        
        return newAABB;
    }
    
    
        /*
    template<class _Point>
    template<class _Matrix>
    void AABB<_Point>::rotate(const _Matrix& mtxRot)
    {
        std::array<point_type, 8> box;
        generateVertices(box);
        
        typedef typename _Matrix::col_type column;
        
        for (auto& pt : box)
        {
            column col = column(pt, (typename column::value_type)1);
            col *= mtxRot;
            
            if (pt.x < min.x)
                min.x = pt.x;
            if (pt.x > max.x)
                max.x = pt.x;

            if (pt.y < min.y)
                min.y = pt.y;
            if (pt.y > max.y)
                max.y = pt.y;

            if (pt.z < min.z)
                min.z = pt.z;
            if (pt.z > max.z)
                max.z = pt.z;
        }
    }
    */

    }   //  namespace gfx
    
    

    
}   //  namespace cinek

