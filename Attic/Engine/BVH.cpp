//
//  RenderScene.cpp
//  Overview
//
//  Created by Samir Sinha on 5/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "RenderScene.hpp"
#include "RenderResources.hpp"

#include "Engine/AABB.hpp"
#include "Engine/Entity/Comp/Transform.hpp"
#include "Engine/Entity/Comp/Renderable.hpp"
#include "Engine/Debug.hpp"

#include <cinek/vector.hpp>

namespace cinek { namespace overview {

struct WorldObject
{
    EntityId eid;                   ///< Points to the entity for this object
    component::Transform transform; ///< The object's current transform matrix
    Vector3 halfDim;                ///< Local half-bound dimensions of object
    uint32_t treeNodeIdx;           ///< Index to a tree node (i.e. BVH)
    
    Vector3 position() const {
        return Matrix4TranslateComponent(transform.matrix);
    }
};

struct BVHPartitionPlane
{
    enum Axis
    {
        kX,
        kY,
        kZ
    };
    
    static Axis next(Axis axis)
    {
        if (axis == kX)
            return kY;
        else if (axis == kY)
            return kZ;
        else
            return kX;
    }
};

//  Let's give credit to Bullet and other physics engines that use BVH
//  for collision and raycasting.  Most implementations end up referencing
//  Bullet or educational resources.

//  BVHWorld and BVHNode
//
//  Specialized for trees that are infrequently rebalanced (i.e. static)
//  Balancing starts with a single AABB and all objects, digging down until
//  all objects are placed within BVH leaves, where each leaf's object count
//  does not exceed a certain threshold count.
//
//  The BVHNode uses indices into arrays
//
struct BVHNode
{
    enum
    {
        kFlag_Leaf      = 0x80000000
    };

    int32_t parent = -1;
    uint32_t flags = 0;
    
    union
    {
        struct
        {
            int32_t left;
            int32_t right;
        }
        children;
        
        struct
        {
            int32_t start;     // index to start of objects on object list
            int32_t count;     // count of objects on object list
        }
        objects;
    };
    
    AABB<Vector3> aabb;
};

class BVHStaticWorld
{
    CK_CLASS_NON_COPYABLE(BVHStaticWorld);
    
public:
    BVHStaticWorld(int32_t objectCount,
        int32_t objPerNode,
        const Allocator& allocator=Allocator());

    //  adds a group of objects.  this method will regenerate the internal graph
    void addObjects(vector<WorldObject>&& objects);
    
private:
    int32_t partition(int32_t parentNodeIdx,
        const std::pair<int32_t, int32_t>& objRange,
        BVHPartitionPlane::Axis axis);

    vector<WorldObject> _objects;   // ordered by EntityId for binary search
    vector<BVHNode> _nodes;         // the node graph where [0] is our root
    vector<int32_t> _objIndices;    // indices to objects grouped for each node
    int32_t _objPerNode;            // object count per node
};

struct BVHObjectComparator
{
    BVHPartitionPlane::Axis axis;
    vector<WorldObject>* objects;
    
    bool operator()(int32_t objIdx0, int32_t objIdx1)
    {
        return (*objects)[objIdx0].position()[axis] < (*objects)[objIdx1].position()[axis];
    }
};

//  returns the index of the created node.
//  algo adapted from 3dmuve.com
//
int32_t BVHStaticWorld::partition
(
    int32_t parentNodeIdx,
    const std::pair<int32_t, int32_t>& objRange,
    BVHPartitionPlane::Axis axis
)
{
    //  create a new node - it will either be a leaf, or a branch.
    int32_t nodeIndex = (int32_t)_nodes.size();
    _nodes.emplace_back();
    if (nodeIndex == (int32_t)_nodes.size())
        return -1;
    
    auto& node = _nodes.back();
    node.parent = parentNodeIdx;
    
    auto objCount = (objRange.second - objRange.first)+1;
    
    if (objCount > 0)
    {
        // define initial aabb - it will be expanded below from the leaf objs,
        // or from the descendant nodes
        const auto& object = _objects[_objIndices[objRange.first]];
        bx::vec3Neg(node.aabb.min, object.halfDim);
        node.aabb.max = object.halfDim;
        
        const Matrix4& objMatrix = object.transform.matrix;
        node.aabb.rotate(objMatrix);
    }
    
    if (objCount < _objPerNode)
    {
        node.flags |= BVHNode::kFlag_Leaf;
        node.objects.start = objRange.first;
        node.objects.count = objCount;
        
        //  could be an empty leaf
        if (objCount > 0)
        {
            //  define aabb emcompassing all objects
        }
    }
    else
    {
        //  sort indices in place within our object range along an axis
        BVHObjectComparator comp;
        comp.axis = axis;
        comp.objects = &_objects;
        std::sort(_objIndices.begin()+objRange.first,
                  _objIndices.begin()+objCount,
                  comp);
        
        //  partition into two child nodes using the mid object as a pivot.
        //  this algo could be improved to take distance into account.
        auto centerIdx = objCount/2;
        
        node.aabb
                  
    }
    return nodeIndex;
}



/*

class RenderScene
{
public:
    RenderScene(size_t entityLimit, const Allocator& allocator=Allocator());
    
    //  Rebalances the internal representation of objects for optimal rendering.
    //  This might be an intensive operation and should not be called per frame.
    void rebalance();
    
    enum class EntityType
    {
        kStatic,
        kDynamic
    };
    
    //  Add an entity to the scene.  Type determines how the entity is rendered
    //  within the scene.
    void addEntity(EntityId eid, EntityType type,
        const component::Transform* transform,
        const component::Renderable* renderable);
    
    //  Removes an entity from the scene
    void removeEntity(EntityId eid);
    
    //  Update entity transform or renderable
    void updateEntity(EntityId eid,
        const component::Transform* transform,
        const component::Renderable* renderable);
    
    //  Renders a single frame
    void render(RenderResources& resources);
    
private:

};
*/

} /* namespace ovproto */ } /* namespace cinek */