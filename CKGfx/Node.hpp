//
//  Node.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/29/15.
//
//

#ifndef CK_Graphics_Node_hpp
#define CK_Graphics_Node_hpp

#include "GfxTypes.hpp"
#include "AABB.hpp"

namespace cinek {
    namespace gfx {

struct MeshElement
{
    MeshHandle mesh;
    MaterialHandle material;
    MeshElement* next;
    
    MeshElement& copy(const MeshElement& src) {
        mesh = src.mesh;
        material = src.material;
        return *this;
    }
};

struct ArmatureElement
{
    AnimationSetHandle animSet;
    ArmatureElement& copy(const ArmatureElement& src) {
        animSet = src.animSet;
        return *this;
    }
};



//  Nodes contain lists of their Elements and child Nodes
//      These lists are provided typically by the Node's Owner.
//      For example, the Model Object provides such lists to the Nodes it owns.
//      Since Nodes exist only within the context of their Owner, there's no
//      concern about shared ownership - if the Owner dies, its Nodes also die.

struct Node
{
    enum
    {
        kNodeFlagMask               = 0x00ffffff,
        kElementTypeMask            = 0xff000000
    };
    
    enum ElementType
    {
        kElementTypeNone,
        kElementTypeMesh,
        kElementTypeArmature,
        kElementTypeCustom
    };
    
    Node() : _flags(0) {}
    Node(ElementType e, uint32_t f=0) : Node() {
        _flags |= (e << 24);
        _flags |= (f & ~kElementTypeMask);
    }
    
    bool checkFlags(uint32_t flags) const {
        return (flags & (_flags & kNodeFlagMask)) != 0;
    }
    ElementType elementType() const {
        return (ElementType)((_flags & kElementTypeMask) >> 24);
    }
    
    const Matrix4& transform() const { return _mtx; }
    Matrix4& transform() { return _mtx; }
    void setTransform(const Matrix4& mtx) { _mtx = mtx; }
    
    const ckm::AABB<Vector3>& obb() const { return _obb; }
    ckm::AABB<Vector3>& obb() { return _obb; }
    void setOBB(const ckm::AABB<Vector3>& obb) { _obb = obb; }
    
    const MeshElement* mesh() const;
    MeshElement* mesh();
    
    const ArmatureElement* armature() const;
    ArmatureElement* armature();
    
    Node* parent() { return _parent.resource(); }
    const Node* parent() const { return _parent.resource(); }
    NodeHandle parentHandle() const { return _parent; }
    
    Node* firstChild() { return _firstChild.resource(); }
    const Node* firstChild() const { return _firstChild.resource(); }
    NodeHandle firstChildHandle() const { return _firstChild; }
    
    Node* nextSibling() { return _nextSibling.resource(); }
    const Node* nextSibling() const { return _nextSibling.resource(); }
    NodeHandle nextSiblingHandle() const { return _nextSibling; }
    
    Node* prevSibling();
    const Node* prevSibling() const;
    NodeHandle prevSiblingHandle() const { return _prevSibling; }

private:
    friend class NodeGraph;
 
    Matrix4 _mtx;
    ckm::AABB<Vector3> _obb;
 
    //  parent node
    NodeHandle _parent;
    //  Use flags to determine how to handle the node data.
    uint32_t _flags;
    
    //  Link to the first child of the node.
    NodeHandle _firstChild;
    //  Link to our next node sibling
    NodeHandle _nextSibling;
    NodeHandle _prevSibling;
    
    //  Context depends on the element type.  TODO - Make this a variant?
    union
    {
        MeshElement* mesh;
        ArmatureElement* armature;
        void* custom;
    }
    _element;
};

struct NodeElementCounts
{
    uint32_t nodeCount;
    uint32_t meshElementCount;
    uint32_t armatureCount;
};

////////////////////////////////////////////////////////////////////////////////

inline const MeshElement* Node::mesh() const {
    return elementType() == kElementTypeMesh ? _element.mesh : nullptr;
}
inline MeshElement* Node::mesh() {
    return const_cast<MeshElement*>(
        static_cast<const Node*>(this)->mesh()
    );
}
inline const ArmatureElement* Node::armature() const {
    return elementType() == kElementTypeArmature ? _element.armature : nullptr;
}
inline ArmatureElement* Node::armature() {
    return const_cast<ArmatureElement*>(
        static_cast<const Node*>(this)->armature()
    );
}

    }   //  namespace gfx
}   //  namespace cinek

#endif
