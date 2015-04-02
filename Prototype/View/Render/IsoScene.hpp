//
//  IsoScene.hpp
//  Overview
//
//  Created by Samir Sinha on 7/24/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_IsoScene_hpp
#define Overview_IsoScene_hpp

#include "View/Render/IsoNodeGraph.hpp"

#include "Engine/Model/TileGridMap.hpp"
#include "Engine/Model/AABB.hpp"

#include "cinek/allocator.hpp"

namespace cinek {
    namespace overview {
        class Stage;
        class IsoNode;
    } /* namespace overview */
} /* namespace cinek */

namespace cinek { namespace overview {

    class TileGridMap;
    class TileLibrary;
    class SpriteCollection;

    /// @class   IsoScene
    /// @ingroup IsoScene
    /// @brief   A scene generator used for building Isometric render graphs.
    class IsoScene
    {
        CK_CLASS_NON_COPYABLE(IsoScene);

    public:
        IsoScene(const glm::ivec2& viewDimensions,
                 const glm::ivec2& tileDimensions,
                 const overview::Stage& stage,
                 const Allocator& allocator);

        void update(uint32_t ticks, const Point& pos);

        void visit(const std::function<void(const IsoNode*)>& fn);

        glm::ivec2 isoToScreenPos(const Point& pt) const;
        Point screenToIsoPos(const glm::ivec2& pt, float z) const;
        
        //  Using the current graph, pick the "shallowest" node (smallest depth)
        //  where the node's AABB contains the specified screen point.  Flags
        //  are used for filtering.
        const IsoNode* pickNode(const glm::ivec2& screenPt,
                                uint32_t contexts, uint32_t categories) const;
                            

    private:
        void setupViewBounds(const Point& viewPos);
        Point isoToViewPos(const Point& isoPos) const;
        Point viewToIsoPos(const Point& viewPos) const;

        void attachTileToGraph(const Point& viewPos, const Point& isoPos);
        
        int determineSideOfLineIsPoint(const glm::ivec2& pt,
            const glm::ivec2& p0, const glm::ivec2& p1) const;

    private:
        const overview::Stage& _stage;
        const overview::TileGridMap& _tileGridMap;
        const Point _tileDim;
        Point _viewDim;

        Allocator _allocator;

        Point _centerPos;
        AABB<Point> _isoStageBounds;
        AABB<Point> _viewBounds;
        AABB<Point> _viewAlignedBounds;
        glm::ivec2 _screenOffset;

        IsoNodeGraph _isoNodeGraph;
    };

} /* namespace overview */ } /* namespace cinek */

#endif
