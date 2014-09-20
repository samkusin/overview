//
//  IsoScene.hpp
//  Overview
//
//  Created by Samir Sinha on 7/24/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_IsoScene_hpp
#define Overview_IsoScene_hpp

#include "Game/Render/IsoNodeGraph.hpp"

#include "Engine/Model/TileGridMap.hpp"
#include "Engine/Model/AABB.hpp"

#include "cinek/allocator.hpp"

namespace cinekine {
    namespace ovengine {
        class World;
    } /* namespace ovengine */
} /* namespace cinekine */

namespace cinekine { namespace ovengine {

    class TileGridMap;
    class TileLibrary;
    class SpriteLibrary;
    
    /// @class   IsoScene
    /// @ingroup IsoScene
    /// @brief   A scene generator used for building Isometric render graphs.
    class IsoScene
    {
        CK_CLASS_NON_COPYABLE(IsoScene);

    public:
        IsoScene(const glm::ivec2& viewDimensions,
                 const glm::ivec2& tileDimensions,
                 const ovengine::World& world,
                 const Allocator& allocator);
        
        void update(uint32_t ticks, const Point& pos);
        
        void visit(std::function<void(const IsoNode*)> fn);
                
    private:
        void setupViewBounds(const Point& viewPos);
        glm::vec3 isoToViewPos(const Point& isoPos) const;
        glm::vec3 viewToIsoPos(const Point& viewPos) const;
        
        void attachTileToGraph(const Point& viewPos, const Point& isoPos);
        
    private:
        const ovengine::World& _world;
        const ovengine::TileGridMap& _tileGridMap;
        const Point _tileDim;
        Point _viewDim;

        Allocator _allocator;
        
        Point _centerPos;
        AABB<Point> _isoWorldBounds;
        AABB<Point> _viewBounds;
        AABB<Point> _viewAlignedBounds;
        glm::ivec2 _screenOffset;
        
        IsoNodeGraph _isoNodeGraph;
    };
    
} /* namespace ovengine */ } /* namespace cinekine */

#endif
