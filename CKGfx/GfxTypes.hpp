/**
 *  @file   GfxTypes.hpp
 *  @author Samir Sinha
 *  @date   9/25/2015
 *  @brief  Common header for the rendering framework
 */

#ifndef CK_Graphics_GfxTypes_hpp
#define CK_Graphics_GfxTypes_hpp

#include <cinek/objectpool.hpp>
#include <cinek/types.hpp>
#include <cinek/debug.h>
#include <ckm/mathtypes.hpp>

namespace cinek {
    namespace gfx {

class Context;
class Texture;
class Mesh;
struct Material;
struct Node;
struct Camera;
struct Animation;
struct AnimationSet;
struct Light;
struct NodeElementCounts;
class NodeGraph;
class NodeRenderer;
class AnimationController;
class ModelSet;

using NodeId = uint64_t;

using MeshPool = ManagedObjectPool<Mesh, void>;
using TexturePool = ManagedObjectPool<Texture, void>;
using MaterialPool = ManagedObjectPool<Material, void>;
using AnimationSetPool = ManagedObjectPool<AnimationSet, void>;
using AnimationControllerPool = ManagedObjectPool<AnimationController, void>;
using LightPool = ManagedObjectPool<Light, void>;
using NodePool = ManagedObjectPool<Node, NodeGraph*, 16>;
using ModelSetPool = ManagedObjectPool<ModelSet, void>;

using MeshHandle = ManagedHandle<Mesh, MeshPool>;
using TextureHandle = ManagedHandle<Texture, TexturePool>;
using MaterialHandle = ManagedHandle<Material, MaterialPool>;
using AnimationSetHandle = ManagedHandle<AnimationSet, AnimationSetPool>;
using AnimationControllerHandle = ManagedHandle<AnimationController, AnimationControllerPool>;
using LightHandle = ManagedHandle<Light, LightPool>;
using NodeHandle = ManagedHandle<Node, NodePool>;
using ModelSetHandle = ManagedHandle<ModelSet, ModelSetPool>;

class ShaderLibrary;

using ShaderProgramId = uint32_t;

inline ShaderProgramId makeShaderProgramId(
    uint16_t vertexType,
    uint16_t shaderType
)
{
    return ((ShaderProgramId)vertexType<<16) | shaderType;
}

struct Rect
{
    int x, y;
    int w, h;
};


/// A 4x4 uniform
struct Matrix4
{
    static const Matrix4 kIdentity;
    
    typedef float value_type;
    value_type comp[16];
    operator float*() { return comp; }
    operator const float*() const { return comp; }
};

/// A 4x1 uniform
struct Vector4
{
    typedef float value_type;
    
    static const Vector4 kUnitX;
    static const Vector4 kUnitY;
    static const Vector4 kUnitZ;
    static const Vector4 kUnitW;
    static const Vector4 kZero;
    
    union
    {
        struct { value_type x, y, z, w; };
        struct { value_type r, g, b, a; };
        value_type comp[4];
    };
    
    Vector4() {}
    Vector4(value_type x, value_type y, value_type z, value_type w) :
        comp { x, y, z, w }
    {
    }
    
    operator float*() { return comp; }
    operator const float*() const { return comp; }
    Vector4& from(value_type x, value_type y, value_type z, value_type w) {
        comp[0] = x;
        comp[1] = y;
        comp[2] = z;
        comp[3] = w;
        return *this;
    }
    Vector4& fromABGR(uint32_t abgr) {
        comp[3] = (abgr >> 24)/255.0f;
        comp[2] = ((abgr >> 16) & 0xff)/255.0f;
        comp[1] = ((abgr >> 8) & 0xff)/255.0f;
        comp[0] = (abgr & 0xff)/255.0f;
        return *this;
    }
    uint32_t toABGR() const {
        return ((uint32_t)(comp[3]*255.0f) << 24) |
               ((uint32_t)(comp[2]*255.0f) << 16) |
               ((uint32_t)(comp[1]*255.0f) << 8) |
               ((uint32_t)(comp[0]*255.0f));
    }
};

static_assert(offsetof(Vector4, comp[1]) == offsetof(Vector4, y) &&
              offsetof(Vector4, comp[1]) == offsetof(Vector4, g),
              "ensure array and struct values are at the same offsets");

/// A 3x1 uniform
struct Vector3
{
    typedef float value_type;
    
    static const Vector3 kUnitX;
    static const Vector3 kUnitY;
    static const Vector3 kUnitZ;
    static const Vector3 kZero;
    
    union
    {
        struct { value_type x, y, z; };
        struct { value_type r, g, b; };
        value_type comp[3];
    };
    operator float*() { return comp; }
    operator const float*() const { return comp; }
    
    Vector3() {}
    Vector3(value_type x, value_type y, value_type z) :
        comp { x, y, z }
    {
    }
    
    Vector3& from(value_type x, value_type y, value_type z) {
        comp[0] = x;
        comp[1] = y;
        comp[2] = z;
        return *this;
    }
    Vector3& fromBGR(uint32_t bgr) {
        comp[2] = ((bgr >> 16) & 0xff)/255.0f;
        comp[1] = ((bgr >> 8) & 0xff)/255.0f;
        comp[0] = (bgr & 0xff)/255.0f;
        return *this;
    }
    uint32_t toBGR() const {
        return ((uint32_t)(comp[2]*255.0f) << 16) |
               ((uint32_t)(comp[1]*255.0f) << 8) |
               ((uint32_t)(comp[0]*255.0f));
    }
    
    Vector3& operator+=(const Vector3& v);
    Vector3& operator-=(const Vector3& v);
    Vector3& operator*=(float scalar);
};

static_assert(offsetof(Vector3, comp[1]) == offsetof(Vector3, y) &&
              offsetof(Vector3, comp[1]) == offsetof(Vector3, g),
              "ensure array and struct values are at the same offsets");

struct Vector2
{
    typedef float value_type;
    union
    {
        struct { value_type x, y; };
        struct { value_type u, v; };
        value_type comp[2];
    };
    operator float*() { return comp; }
    operator const float*() const { return comp; }
    
    Vector2() {}
    Vector2(value_type x, value_type y) :
        comp { x, y }
    {
    }
    
    Vector2& from(value_type x, value_type y) {
        comp[0] = x;
        comp[1] = y;
        return *this;
    }
};

static_assert(offsetof(Vector2, comp[1]) == offsetof(Vector2, y),
              "ensure array and struct values are at the same offsets");


using Color4 = Vector4;
using Color3 = Vector3;

Vector3 operator-(const Vector3& v0, const Vector3& v1);
Vector3 operator+(const Vector3& v0, const Vector3& v1);
Vector3 operator*(const Vector3& v0, float scalar);

using Frustrum = ckm::Frustrum<Vector3>;
using AABB = ckm::AABB<Vector3>;

AABB transformAABB(const AABB& aabb, const Matrix4& mtx);

struct NodeJsonLoader;

class RenderTarget;
class MultiTextureRenderTarget;

    }   // namepace gfx
}   // namespace cinek

#endif
