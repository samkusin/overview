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

    private:
        void setupViewBounds(const Point& viewPos);
        glm::vec3 isoToViewPos(const Point& isoPos) const;
        glm::vec3 viewToIsoPos(const Point& viewPos) const;

        void attachTileToGraph(const Point& viewPos, const Point& isoPos);

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
