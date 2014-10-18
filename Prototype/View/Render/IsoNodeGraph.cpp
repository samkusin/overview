//
//  IsoNodeGraph.cpp
//  Overview
//
//  Created by Samir Sinha on 7/27/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "View/Render/IsoNodeGraph.hpp"


namespace cinekine { namespace ovengine {
    
    IsoNodeGraph::IsoNodeGraph(size_t nodeLimit, const Allocator& allocator) :
        _nodePool(nodeLimit, allocator),
        _nodes(allocator),
        _nodesBehind(allocator),
        _sortDepth(0)
    {
        //  preallocate memory for our nodes. our behind vector has a much larger size,
        //  though its very unlikely to ever reach that limit, even if we reach our
        //  IsoNode limit
        _nodes.reserve(nodeLimit);
        _nodesBehind.reserve(nodeLimit*nodeLimit/2);
    }
    
    IsoNode* IsoNodeGraph::obtainNode(const cinek_bitmap& bitmap,
                                      const glm::ivec2& viewPos, const AABB<glm::vec3>& box)
    {
        auto node = _nodePool.allocateAndConstruct(bitmap, viewPos, box);
        _nodes.push_back(node);
        return node;
    }
    
    uint32_t IsoNodeGraph::pushNodeBehind(IsoNode* node)
    {
        _nodesBehind.push_back(node);
        return (uint32_t)(_nodesBehind.size()-1);
    }
    
    void IsoNodeGraph::reset()
    {
        _nodesBehind.clear();
        _nodes.clear();
        _nodePool.destructAll();
    }
    
    void IsoNodeGraph::sort()
    {
        //  build behind list for every node, forming a graph of IsoNode objects
        for (auto aIdx = 0; aIdx < _nodes.size(); ++aIdx)
        {
            auto& nodeA = _nodes[aIdx];
            nodeA->clearBehindList();
            for (auto bIdx = 0; bIdx < _nodes.size(); ++bIdx)
            {
                if (aIdx != bIdx)
                {
                    auto& nodeB = _nodes[bIdx];
                    if (nodeB->behind(*nodeA))
                    {
                        nodeA->addIndexToBehindList(pushNodeBehind(nodeB));
                    }
                }
            }
            nodeA->_visited = false;
        }
        
        _sortDepth = 0;
        for (auto& node : _nodes)
        {
            fixupNode(node);
        }
        
        //  sort our node list by depth
        std::sort(_nodes.begin(), _nodes.end(),
                  [](const IsoNode* l, const IsoNode* r) -> bool
                  {
                      return l->depth() < r->depth();
                  });
        
        
    }
    
    void IsoNodeGraph::fixupNode(IsoNode* node)
    {
        if (node->visited())
            return;
        
        node->_visited = true;
        
        auto& behindRange = node->behindRange();
        for (auto i = behindRange.first; i < behindRange.second; ++i)
        {
            auto& behindNode = _nodesBehind[i];
            if (!behindNode)
            {
                break;
            }
            else
            {
                fixupNode(behindNode);
                _nodesBehind[i] = nullptr;
            }
        }
        
        node->_depth = _sortDepth++;
    }
    
} /* namespace ovengine */ } /* namespace cinekine */

