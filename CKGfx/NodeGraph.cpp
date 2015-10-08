//
//  Model.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/28/15.
//
//

#include "NodeGraph.hpp"

#include <cinek/debug.h>

namespace cinek {
    namespace gfx {
 
NodeGraph::NodeGraph(const NodeElementCounts& params) :
    _transformElementsPool(params.transformCount),
    _meshElementPool(params.meshElementCount),
    _nodes(params.nodeCount)
{
    _nodes.setDelegate(this);
}

NodeGraph::NodeGraph(NodeGraph&& other) noexcept :
    _transformElementsPool(std::move(other._transformElementsPool)),
    _meshElementPool(std::move(other._meshElementPool)),
    _nodes(std::move(other._nodes)),
    _root(std::move(other._root))
{
    _nodes.setDelegate(this);
}

NodeGraph& NodeGraph::operator=(NodeGraph&& other) noexcept
{
    _transformElementsPool = std::move(other._transformElementsPool);
    _meshElementPool = std::move(other._meshElementPool);
    _nodes = std::move(other._nodes);
    _root = std::move(other._root);
    
    _nodes.setDelegate(this);
    
    return *this;
}

NodeGraph::~NodeGraph()
{
}

NodeHandle NodeGraph::createMeshNode(uint32_t elementCnt)
{
    NodeHandle handle;
    CK_ASSERT_RETURN_VALUE(elementCnt>0, nullptr);

    Node node(Node::kElementTypeMesh);

    handle = _nodes.add(std::move(node));
    if (handle) {
        handle->_element.mesh = _meshElementPool.construct();
        MeshElement* e = handle->_element.mesh;
        for (uint32_t i = 1; i < elementCnt && e; ++i) {
            e->next = _meshElementPool.construct();
            e = e->next;
        }
        CK_ASSERT(e);   // ran out of elements?
        if (e) {
            e->next = nullptr;
        }
    }
    return handle;
}

NodeHandle NodeGraph::createTransformNode()
{
    NodeHandle handle;
    Node node(Node::kElementTypeTransform);
    
    handle = _nodes.add(std::move(node));
    if (handle) {
        handle->_element.transform = _transformElementsPool.construct();
    }
    return handle;
}

void NodeGraph::onReleaseManagedObject(cinek::gfx::Node &node)
{
    switch (node.elementType())
    {
    case Node::kElementTypeMesh:
        {
            while (node._element.mesh) {
                _meshElementPool.destruct(node._element.mesh);
                node._element.mesh = node._element.mesh->next;
            }
        }
        break;
    case Node::kElementTypeTransform:
        {
            _transformElementsPool.destruct(node._element.transform);
            node._element.transform = nullptr;
        }
        break;
        
    default:
        break;
    };
}

void NodeGraph::setRoot(NodeHandle node)
{
    _root = node;
}

NodeHandle NodeGraph::addChildNodeToNode(NodeHandle child, NodeHandle node)
{
    CK_ASSERT(node);
    
    child->_parent = node;
 
    NodeHandle childHead = child->_parent->_firstChild;
    
    if (childHead) {
        NodeHandle childTail = childHead->_prevSibling;
        childHead->_prevSibling = child;
        childTail->_nextSibling = child;
        child->_prevSibling = childTail;
    }
    else {
        node->_firstChild = child;
        child->_prevSibling = child;
    }
    child->_nextSibling = nullptr;

    return child;
}

NodeHandle NodeGraph::removeNode(NodeHandle child)
{
    //  detach from next sibling or fixup firstChild links child was the tail
    //  remember: head prev points to tail, but not vice-versa
    if (child->_nextSibling) {
        child->_nextSibling->_prevSibling = child->_prevSibling;
    }
    else if (child->_parent) {
        child->_parent->_firstChild->_prevSibling = child->_prevSibling;
    }
    //  head prev = tail, so we only need to fixup prev sibling if its not
    //  a tail node
    if (child->_prevSibling->_nextSibling) {
        child->_prevSibling->_nextSibling = child->_nextSibling;
    }
    else if (child->_parent) {
        child->_parent->_firstChild = child->_nextSibling;
    }
    
    if (child == _root) {
        _root = child->_nextSibling;
    }
    
    child->_parent = nullptr;
    child->_prevSibling = nullptr;
    child->_nextSibling = nullptr;
    
    return child;
}


NodeHandle NodeGraph::clone(NodeHandle source)
{
    NodeHandle cloned;
    
    //  select node type to clone
    switch (source->elementType()) {
    case Node::kElementTypeTransform:
        cloned = createTransformNode();
        cloned->transform()->copy(*source->transform());
        break;
    case Node::kElementTypeMesh: {
            int cnt = 0;
            const MeshElement* src = source->mesh();
            while (src) {
                src = src->next;
                ++cnt;
            }
            cloned = createMeshNode(cnt);
            src = source->mesh();
            MeshElement* dest = cloned->mesh();
            while (src && dest) {
                dest->copy(*src);
                dest = dest->next;
                src = src->next;
            }
            CK_ASSERT(!src && !dest);
        }
        break;
    default:
        //  this node type is not supported?
        CK_ASSERT(false);
        return nullptr;
    }
    
    for (NodeHandle child = source->firstChildHandle();
         child;
         child = child->nextSiblingHandle()) {
        NodeHandle clonedChild = clone(child);
        addChildNodeToNode(clonedChild, cloned);
    }
    
    return cloned;
}




    }   // namespace gfx
}   // namespace cinek