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
#include "Node.hpp"
#include "NodeGraph.hpp"

#include <cinek/debug.h>

#include <cinek/objectpool.inl>
#include <cinek/managed_handle.inl>

#include <bx/fpumath.h>

namespace cinek {

    //  necessary template impls for now - the ManagedObjectPool needs to have
    //  the definition of the object its pooling, and the managed handle needs
    //  the ManagedObjectPool implementation to compile and link.
    

    template class ManagedObjectPool<gfx::Texture, void>;
    template void ManagedHandle<gfx::Texture, ManagedObjectPool<gfx::Texture, void>>::acquire();
    template void ManagedHandle<gfx::Texture, ManagedObjectPool<gfx::Texture, void>>::release();
    template class ManagedObjectPoolBase<gfx::Texture, ManagedObjectPool<gfx::Texture, void>>;
    
    template class ManagedObjectPool<gfx::Material, void>;
    template void ManagedHandle<gfx::Material, ManagedObjectPool<gfx::Material, void>>::acquire();
    template void ManagedHandle<gfx::Material, ManagedObjectPool<gfx::Material, void>>::release();
    template class ManagedObjectPoolBase<gfx::Material, ManagedObjectPool<gfx::Material, void>>;
    
    template class ManagedObjectPool<gfx::Mesh, void>;
    template void ManagedHandle<gfx::Mesh, ManagedObjectPool<gfx::Mesh, void>>::acquire();
    template void ManagedHandle<gfx::Mesh, ManagedObjectPool<gfx::Mesh, void>>::release();
    template class ManagedObjectPoolBase<gfx::Mesh, ManagedObjectPool<gfx::Mesh, void>>;
    
    template class ManagedObjectPool<gfx::Node, gfx::NodeGraph*>;
    template void ManagedHandle<gfx::Node, ManagedObjectPool<gfx::Node, gfx::NodeGraph*>>::acquire();
    template void ManagedHandle<gfx::Node, ManagedObjectPool<gfx::Node, gfx::NodeGraph*>>::release();
    template class ManagedObjectPoolBase<gfx::Node, ManagedObjectPool<gfx::Node, gfx::NodeGraph*>>;

    namespace gfx {
    
    
    Vector3 operator-(const Vector3& v0, const Vector3& v1)
    {
        Vector3 ret;
        bx::vec3Sub(ret, v0, v1);
        return ret;
    }
    
    Vector3& operator-=(Vector3& v0, const Vector3& v1)
    {
        bx::vec3Sub(v0, v0, v1);
        return v0;
    }
    
    Vector3 operator+(const Vector3& v0, const Vector3& v1)
    {
        Vector3 ret;
        bx::vec3Add(ret, v0, v1);
        return ret;
    }
    
    Vector3& operator+=(Vector3& v0, const Vector3& v1)
    {
        bx::vec3Add(v0, v0, v1);
        return v0;
    }
    
    Vector3 operator*(const Vector3& v0, float scalar)
    {
        Vector3 ret;
        bx::vec3Mul(ret, v0, scalar);
        return ret;
    }
    
    }   //  namespace gfx
    
}   //  namespace cinek


#include <ckm/aabb.hpp>
#include <ckm/math.hpp>
#define CKM_MATH_IMPLEMENTATION
#include <ckm/math.inl>

#include <ckm/geometry.hpp>
#include <ckm/geometry.inl>

namespace ckm {

using namespace cinek::gfx;

template<> Vector3 cross(Vector3 const& v0, Vector3 const& v1) {
    Vector3 v;
    bx::vec3Cross(v, v0, v1);
    return v;
}

template<> Vector3::value_type dot(Vector3 const& v0, Vector3 const& v1) {
    return bx::vec3Dot(v0, v1);
}

template<> Matrix4 inverse(Matrix4 const& m) {
    Matrix4 ret;
    
    bx::mtxInverse(ret, m);
    return ret;
}

template<> Vector3 normalize(Vector3 const& v) {
    Vector3 ret;
    bx::vec3Norm(ret, v);
    return ret;
}

template<> Vector3::value_type vectorLength(Vector3 const& v) {
    return bx::vec3Length(v);
}

template struct Plane3<Vector3>;
template class Frustrum<Vector3>;
template struct AABB<Vector3>;


}   // namespace ckm
