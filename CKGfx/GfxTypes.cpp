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
    

    template class ManagedObjectPool<gfx::Texture, void>;
    template void ManagedHandle<gfx::Texture, ManagedObjectPool<gfx::Texture, void>>::acquire();
    template void ManagedHandle<gfx::Texture, ManagedObjectPool<gfx::Texture, void>>::release();
    template class ManagedObjectPoolBase<gfx::Texture, ManagedObjectPool<gfx::Texture, void>>;
    
    template class ManagedObjectPool<gfx::Material, void>;
    template void ManagedHandle<gfx::Material, ManagedObjectPool<gfx::Material, void>>::acquire();
    template void ManagedHandle<gfx::Material, ManagedObjectPool<gfx::Material, void>>::release();
    template class ManagedObjectPoolBase<gfx::Material, ManagedObjectPool<gfx::Material, void>>;
    
    template class ManagedObjectPool<gfx::AnimationSet, void>;
    template void ManagedHandle<gfx::AnimationSet, ManagedObjectPool<gfx::AnimationSet, void>>::acquire();
    template void ManagedHandle<gfx::AnimationSet, ManagedObjectPool<gfx::AnimationSet, void>>::release();
    template class ManagedObjectPoolBase<gfx::AnimationSet, ManagedObjectPool<gfx::AnimationSet, void>>;
    
    template class ManagedObjectPool<gfx::AnimationController, void>;
    template void ManagedHandle<gfx::AnimationController, ManagedObjectPool<gfx::AnimationController, void>>::acquire();
    template void ManagedHandle<gfx::AnimationController, ManagedObjectPool<gfx::AnimationController, void>>::release();
    template class ManagedObjectPoolBase<gfx::AnimationController, ManagedObjectPool<gfx::AnimationController, void>>;
    
    template class ManagedObjectPool<gfx::Mesh, void>;
    template void ManagedHandle<gfx::Mesh, ManagedObjectPool<gfx::Mesh, void>>::acquire();
    template void ManagedHandle<gfx::Mesh, ManagedObjectPool<gfx::Mesh, void>>::release();
    template class ManagedObjectPoolBase<gfx::Mesh, ManagedObjectPool<gfx::Mesh, void>>;
    
    template class ManagedObjectPool<gfx::Light, void>;
    template void ManagedHandle<gfx::Light, ManagedObjectPool<gfx::Light, void>>::acquire();
    template void ManagedHandle<gfx::Light, ManagedObjectPool<gfx::Light, void>>::release();
    template class ManagedObjectPoolBase<gfx::Light, ManagedObjectPool<gfx::Light, void>>;
    
    template class ManagedObjectPool<gfx::Node, gfx::NodeGraph*>;
    template void ManagedHandle<gfx::Node, ManagedObjectPool<gfx::Node, gfx::NodeGraph*>>::acquire();
    template void ManagedHandle<gfx::Node, ManagedObjectPool<gfx::Node, gfx::NodeGraph*>>::release();
    template class ManagedObjectPoolBase<gfx::Node, ManagedObjectPool<gfx::Node, gfx::NodeGraph*>>;

    template class ManagedObjectPool<gfx::NodeGraph, void>;
    template void ManagedHandle<gfx::NodeGraph, ManagedObjectPool<gfx::NodeGraph, void>>::acquire();
    template void ManagedHandle<gfx::NodeGraph, ManagedObjectPool<gfx::NodeGraph, void>>::release();
    template class ManagedObjectPoolBase<gfx::NodeGraph, ManagedObjectPool<gfx::NodeGraph, void>>;


    namespace gfx {
    
    const Matrix4 Matrix4::kIdentity = { 1.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 1.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 1.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 1.0f };
    
    const Vector4 Vector4::kUnitX = { 1.0f, 0.0f, 0.0f, 0.0f };
    const Vector4 Vector4::kUnitY = { 0.0f, 1.0f, 0.0f, 0.0f };
    const Vector4 Vector4::kUnitZ = { 0.0f, 0.0f, 1.0f, 0.0f };
    const Vector4 Vector4::kUnitW = { 0.0f, 0.0f, 0.0f, 1.0f };
    const Vector4 Vector4::kZero = { 0.0f, 0.0f, 0.0f, 0.0f };
    
    
    const Vector3 Vector3::kUnitX = { 1.0f, 0.0f, 0.0f };
    const Vector3 Vector3::kUnitY = { 0.0f, 1.0f, 0.0f };
    const Vector3 Vector3::kUnitZ = { 0.0f, 0.0f, 1.0f };
    const Vector3 Vector3::kZero = { 0.0f, 0.0f, 0.0f };
    
    Vector3 operator-(const Vector3& v0, const Vector3& v1)
    {
        Vector3 ret;
        bx::vec3Sub(ret, v0, v1);
        return ret;
    }
    
    Vector3& Vector3::operator-=(const Vector3& v0)
    {
        bx::vec3Sub(comp, comp, v0);
        return *this;
    }
    
    Vector3 operator+(const Vector3& v0, const Vector3& v1)
    {
        Vector3 ret;
        bx::vec3Add(ret, v0, v1);
        return ret;
    }
    
    Vector3& Vector3::operator+=(const Vector3& v0)
    {
        bx::vec3Add(comp, comp, v0);
        return *this;
    }
    
    Vector3 operator*(const Vector3& v0, float scalar)
    {
        Vector3 ret;
        bx::vec3Mul(ret, v0, scalar);
        return ret;
    }
    
    Vector3& Vector3::operator*=(float scalar)
    {
        bx::vec3Mul(comp, comp, scalar);
        return *this;
    }

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

template<> Vector4 zero<Vector4>() {
    return Vector4(0,0,0,0);
}

template<> Vector3 zero<Vector3>() {
    return Vector3(0,0,0);
}

template<> Vector2 zero<Vector2>() {
    return Vector2(0,0);
}

template struct Plane3<Vector3>;
template class Frustrum<Vector3>;
template struct AABB<Vector3>;

}   // namespace ckm
