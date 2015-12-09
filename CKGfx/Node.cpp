//
//  Node.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/1/15.
//
//

#include "Node.hpp"

#include <bx/fpumath.h>

namespace cinek {
    namespace gfx {
    
//  sibling list is circular from the "prev" node to simplify adding
//  new nodes to the end of the list.
//  

const Node* Node::prevSibling() const
{
    if (!_prevSibling)
        return nullptr;
    
    // this is the first node? - see notes about our sibling list impl
    if (!_prevSibling->_nextSibling)
        return nullptr;
    
    return _prevSibling.resource();
}

Node* Node::prevSibling()
{
    return const_cast<Node*>(static_cast<const Node*>(this)->prevSibling());
}


void generateAABBForNode(AABB& aabb, const Node& node, const Matrix4& parentMtx)
{
    Matrix4 mtx;
    bx::mtxMul(mtx, node.transform(), parentMtx);

    for (const Node* child = node.firstChild();
         child;
         child = child->nextSibling()) {
        
        generateAABBForNode(aabb, *child, mtx);
    }
    
    aabb.merge(transformAABB(node.obb(), mtx));
}


    }   // namespace gfx
}   // namespace cinek
