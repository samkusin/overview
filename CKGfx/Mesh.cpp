//
//  Mesh.cpp
//  Overview
//
//  Created by Samir Sinha on 4/27/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "Mesh.hpp"

namespace cinek {
    namespace gfx {

Mesh::Mesh
(
    VertexTypes::Format format,
    uint32_t elementCount,
    const Allocator& allocator
) :
    _format(format),
    _nodes(allocator)
{
    _nodes.reserve(elementCount);
}

Mesh::~Mesh()
{
    for (auto& node : _nodes)
    {
        bgfx::destroyIndexBuffer(node.element.idxBufH);
        bgfx::destroyVertexBuffer(node.element.vertBufH);
    }
}

int32_t Mesh::addElement
(
    int32_t parentIndex,
    const Matrix4& transform,
    const bgfx::Memory* vertexData,
    const bgfx::Memory* indexData
)
{
    if (parentIndex < 0 || parentIndex >= _nodes.size())
        return -1;

    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        vertexData,
        VertexTypes::declaration(_format)
    );
    if (!bgfx::isValid(vbh))
        return -1;
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(indexData);
    if (!bgfx::isValid(ibh))
    {
        bgfx::destroyVertexBuffer(vbh);
        return -1;
    }
    
    _nodes.emplace_back();
    auto& node = _nodes.back();
    node.element.parentIdx = parentIndex;
    node.element.transform = transform;
    node.element.vertBufH = vbh;
    node.element.idxBufH = ibh;
    node.element.firstChildIdx = -1;
    node.element.nextSiblingIdx = -1;
    
    //  inject our node's index into our parent or sibling to fixup its location
    //  within the element tree
    int32_t thisIndex = (int32_t)_nodes.size() - 1;
    
    auto& parent = _nodes[parentIndex];
    int32_t siblingIndex = parent.element.firstChildIdx;
    if (siblingIndex < 0)
    {
        parent.element.firstChildIdx = thisIndex;
    }
    else
    {
        while (_nodes[siblingIndex].element.nextSiblingIdx >= 0)
            siblingIndex = _nodes[siblingIndex].element.nextSiblingIdx;
        _nodes[siblingIndex].element.nextSiblingIdx = thisIndex;
    }
    
    return thisIndex;
}
    
    }   // namespace gfx
}   // namespace cinek