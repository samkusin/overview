/**
 *  @file   GfxTypes.hpp
 *  @author Samir Sinha
 *  @date   9/25/2015
 *  @brief  Common header for the rendering framework
 */

#ifndef CK_Graphics_GfxTypes_hpp
#define CK_Graphics_GfxTypes_hpp

#if defined(_MSC_VER)
#pragma warning( disable : 4577)
#endif

#include <cinek/objectpool.hpp>
#include <cinek/types.hpp>
#include <cinek/debug.h>
#include <ckm/math.hpp>

#include <array>

typedef struct NVGcontext NVGcontext;

namespace cinek {
    namespace gfx {
    
namespace MeshBuilder {
    struct BuilderState;
}

enum class PrimitiveType
{
    kUndefined,
    kTriangles,
    kLines
};

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

using Matrix4 = ::ckm::matrix4_type<float>;
using Vector4 = ::ckm::vector4_type<float>;
using Vector3 = ::ckm::vector3_type<float>;
using Vector2 = ::ckm::vector2_type<float>;
using Color4 = Vector4;
using Color3 = Vector3;

inline Color4 fromABGR(uint32_t abgr) {
    Vector4 v;
    v.comp[3] = (abgr >> 24)/255.0f;
    v.comp[2] = ((abgr >> 16) & 0xff)/255.0f;
    v.comp[1] = ((abgr >> 8) & 0xff)/255.0f;
    v.comp[0] = (abgr & 0xff)/255.0f;
    return v;
}

inline uint32_t toABGR(const Color4& v) {
    return ((uint32_t)(v.comp[3]*255.0f) << 24) |
           ((uint32_t)(v.comp[2]*255.0f) << 16) |
           ((uint32_t)(v.comp[1]*255.0f) << 8) |
           ((uint32_t)(v.comp[0]*255.0f));
}

inline Color3 fromBGR(uint32_t bgr) {
    Vector3 v;
    v.comp[2] = ((bgr >> 16) & 0xff)/255.0f;
    v.comp[1] = ((bgr >> 8) & 0xff)/255.0f;
    v.comp[0] = (bgr & 0xff)/255.0f;
    return v;
}

inline uint32_t toBGR(const Color3& v)  {
    return ((uint32_t)(v.comp[2]*255.0f) << 16) |
           ((uint32_t)(v.comp[1]*255.0f) << 8) |
           ((uint32_t)(v.comp[0]*255.0f));
}

using Frustrum = ckm::frustrum<Vector3>;
using AABB = ckm::AABB<Vector3>;

AABB transformAABB(const AABB& aabb, const Matrix4& mtx);

struct NodeJsonLoader;

class RenderTarget;
class MultiTextureRenderTarget;

    }   // namepace gfx
}   // namespace cinek

#endif
