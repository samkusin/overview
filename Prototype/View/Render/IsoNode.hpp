//
//  IsoNode.hpp
//  Overview
//
//  Created by Samir Sinha on 7/26/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_IsoNode_hpp
#define Overview_IsoNode_hpp

#include "Engine/Model/AABB.hpp"

#include <utility>

namespace cinek { namespace overview {

    class IsoNodeGraph;
    struct Tile;

    /// @class   IsoNode
    /// @ingroup IsoScene
    /// @brief   A render node within an IsoScene
    ///
    /// The IsoNode is a single entity within an IsoScene render graph.
    ///
    class IsoNode
    {
    public:
        enum Type
        {
            kNull,
            kBitmap
        };
        
    public:
        IsoNode(const cinek_bitmap& bitmap,
                const glm::ivec2& viewPos,
                const AABB<glm::vec3>& box);

        bool behind(const IsoNode& b) const;
        void clearBehindList();
        void addIndexToBehindList(uint32_t index);
        bool visited() const { return _visited; }
        uint32_t depth() const { return _depth; }

        const std::pair<uint32_t, uint32_t> behindRange() const;

        const glm::ivec2& viewPos() const { return _viewPos; }
        Type type() const { return _renderType; }
        cinek_bitmap bitmap() const { return _bitmap; }

    private:
        friend class IsoNodeGraph;
        glm::ivec2 _viewPos;
        AABB<glm::vec3> _box;          // In IsoScene view coordinates

        std::pair<uint32_t, uint32_t> _behindRange;

        Type _renderType;
        cinek_bitmap _bitmap;

        uint32_t _depth;
        bool _visited;
    };

    ////////////////////////////////////////////////////////////////////

    inline bool IsoNode::behind(const IsoNode& b) const
    {
        return (_box.min.x < b._box.max.x &&
                _box.min.y < b._box.max.y &&
                _box.min.z < b._box.max.z);
    }

    inline void IsoNode::addIndexToBehindList(uint32_t index)
    {
        if (_behindRange.first == _behindRange.second)
            _behindRange.first = index;
        _behindRange.second = index+1;
    }

    inline const std::pair<uint32_t, uint32_t> IsoNode::behindRange() const
    {
        return _behindRange;
    }

} /* namespace overview */ } /* namespace cinek */


#endif
