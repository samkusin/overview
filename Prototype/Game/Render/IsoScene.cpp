//
//  IsoScene.cpp
//  Overview
//
//  Created by Samir Sinha on 7/26/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/Render/IsoScene.hpp"
#include "Engine/Model/Stage.hpp"
#include "Graphics/RendererCLI.hpp"
#include "Graphics/Rect.hpp"

namespace cinekine { namespace ovengine {
    
    IsoScene::IsoScene(const glm::ivec2& viewDimensions,
                       const glm::ivec2& tileDimensions,
                       const ovengine::Stage& stage,
                       const Allocator& allocator) :
        _stage(stage),
        _tileGridMap(_stage.tileGridMap()),
        _tileDim(tileDimensions.x, tileDimensions.y, tileDimensions.y),
        _viewDim(viewDimensions.x, viewDimensions.y, 0.f),
        _allocator(allocator),
        _isoNodeGraph(2048, allocator)
    {
        auto& overlayGrid = _tileGridMap.overlay();
        //  determine z view and iso coords based on a pre-fixed +- z tile offset
        //  at some point we'd want to make this configurable, but for now pre-fixed
        //  works for development purposes
        //
        float zTiles = _tileGridMap.overlayToFloorRatio() * 1.f;
        _isoStageBounds.min = Point(0,0, -zTiles*0.5f);
        _isoStageBounds.max = Point(overlayGrid.columnCount(),
                                    overlayGrid.rowCount(),
                                    zTiles*0.5f);
        _viewDim.z = zTiles * _tileDim.z;

        _centerPos = _isoStageBounds.center();
        setupViewBounds(isoToViewPos(_centerPos));
    }
        
    void IsoScene::update(uint32_t ticks, const Point& pos)
    {
        _isoNodeGraph.reset();
        
        //  update our view bounds within the tile grid
        if (pos != _centerPos)
        {
            _centerPos = pos;
            setupViewBounds(isoToViewPos(_centerPos));
        }
        
        //  pick all tiles within the view bounds
        //      this is done to minimize the number of tiles and objects added to
        //      our scene graph
        float viewY = _viewAlignedBounds.min.y;
        float viewEndY = _viewAlignedBounds.max.y;
        
        //  left to right
        //  top to bottom
        int32_t rowCount = 0;
        
        while (viewY <= viewEndY)
        {
            float viewX = _viewAlignedBounds.min.x;
            float viewEndX = _viewAlignedBounds.max.x;
            if (rowCount & 1)
            {
                viewX -= _tileDim.x/2;
                viewEndX += _tileDim.x/2;
            }
            
            Point viewTilePos(viewX, viewY, 0);
            while (viewX <= viewEndX)
            {
                viewTilePos.x = viewX;
                auto isoTilePos = viewToIsoPos(viewTilePos);
                if (isoTilePos.x >= _isoStageBounds.min.x && isoTilePos.x < _isoStageBounds.max.x &&
                    isoTilePos.y >= _isoStageBounds.min.y && isoTilePos.y < _isoStageBounds.max.y &&
                    isoTilePos.z >= _isoStageBounds.min.z && isoTilePos.z < _isoStageBounds.max.z)
                {
                    attachTileToGraph(viewTilePos - _viewBounds.min, isoTilePos);
                }
                viewX += _tileDim.x;
            }
            viewY += _tileDim.y/2;
            ++rowCount;
        }
        
        // Place sprites
        // Calculate a bounding isometric bounds based on our view bounds.
        //
        //
        Point viewIsoMinXY = _viewAlignedBounds.min;
        Point viewIsoMaxXY = _viewAlignedBounds.max;
        Point viewIsoMinXMaxY = Point(viewIsoMinXY.x, viewIsoMaxXY.y, 0.f);
        Point viewIsoMaxXMinY = Point(viewIsoMaxXY.x, viewIsoMinXY.y, 0.f);
        
        viewIsoMinXY = viewToIsoPos(viewIsoMinXY);
        viewIsoMaxXY = viewToIsoPos(viewIsoMaxXY);
        viewIsoMinXMaxY = viewToIsoPos(viewIsoMinXMaxY);
        viewIsoMaxXMinY = viewToIsoPos(viewIsoMaxXMinY);
        viewIsoMinXY.y = viewIsoMaxXMinY.y;
        viewIsoMaxXY.y = viewIsoMinXMaxY.y;
        
        AABB<Point> isoBounds(viewIsoMinXY, viewIsoMaxXY);
        
        struct
        {
            IsoScene* scene;
            uint32_t ticks;
        }
        context = { this, ticks };
        
        _stage.selectInstanceLists(isoBounds,
                                   [&context](const ovengine::SpriteInstanceList& instances)
                                   {
                                       IsoScene* scene = context.scene;
                                       for (auto& instance : instances)
                                       {
                                           auto& isoPos = instance.position();
                                           auto viewPos = scene->isoToViewPos(isoPos);
                                           if (scene->_viewAlignedBounds.contains(viewPos))
                                           {
                                               viewPos -= scene->_viewBounds.min;
                                               glm::ivec2 viewAnchor = scene->_screenOffset - instance.anchor();
                                               viewAnchor.x += viewPos.x;
                                               viewAnchor.y += viewPos.y + scene->_tileDim.y;
                                               
                                               AABB<Point> isoBox =  instance.aabb() + isoPos;
                                               scene->_isoNodeGraph.obtainNode(instance.bitmapFromTime(context.ticks),
                                                                               viewAnchor,
                                                                               isoBox);
                                           }
                                       }
                                   });
        
        // sort nodes.  once complete, the graph will be ready for traversal
        //
        _isoNodeGraph.sort();
    }
    
    void IsoScene::attachTileToGraph(const Point& viewPos,
                                     const Point& isoPos)
    {
        const uint32_t kOverlayTilePerFloor = _tileGridMap.overlayToFloorRatio();
        const uint32_t overlayY = (uint32_t)isoPos.y;
        const uint32_t overlayX = (uint32_t)isoPos.x;
        const uint32_t floorY = overlayY / kOverlayTilePerFloor;
        const uint32_t floorX = overlayX / kOverlayTilePerFloor;
        AABB<Point> isoBox;
        
        //  floor tile
        if (!(overlayX % kOverlayTilePerFloor) && !(overlayY % kOverlayTilePerFloor))
        {
            auto tileId = _tileGridMap.floor().at(floorY, floorX);
            if (tileId)
            {
                auto& tileInfo = _stage.tileInfo(tileId);

                isoBox =  tileInfo.aabb + isoPos;
                
                glm::ivec2 viewAnchor = _screenOffset;
                viewAnchor.x += viewPos.x - tileInfo.anchor.x;
                viewAnchor.y += viewPos.y + (_tileDim.y * kOverlayTilePerFloor) - tileInfo.anchor.y;
                
                _isoNodeGraph.obtainNode(tileInfo.bitmap, viewAnchor, isoBox);
            }
        }
        
        //  overlay tile
        auto tileId = _tileGridMap.overlay().at(overlayY, overlayX);
        if (tileId)
        {
            auto& tileInfo = _stage.tileInfo(tileId);
            
            isoBox = tileInfo.aabb + isoPos;
            
            glm::ivec2 viewAnchor = _screenOffset;
            viewAnchor.x += viewPos.x - tileInfo.anchor.x;
            viewAnchor.y += viewPos.y + _tileDim.y - tileInfo.anchor.y;

            _isoNodeGraph.obtainNode(tileInfo.bitmap, viewAnchor, isoBox);
        }
    }
    
    void IsoScene::visit(std::function<void(const IsoNode*)> fn)
    {
        auto& nodes = _isoNodeGraph.nodes();
        for (auto& node : nodes)
        {
            fn(node);
        }
    }

    glm::vec3 IsoScene::isoToViewPos(const Point& isoPos) const
    {
        Point viewPos(_tileDim.x * (isoPos.x - isoPos.y)/2,
                      _tileDim.y * (isoPos.x + isoPos.y)/2,
                      isoPos.z * _tileDim.z);
        return viewPos;
    }
    
    glm::vec3 IsoScene::viewToIsoPos(const Point& viewPos) const
    {
        Point isoPos;
        isoPos.x = viewPos.x/_tileDim.x + viewPos.y/_tileDim.y;
        isoPos.y = 2 * viewPos.y/_tileDim.y - isoPos.x;
        isoPos.z = viewPos.z/_tileDim.z;
        return isoPos;
    }
    
    //  precalculates values used for rendering the local view
    void IsoScene::setupViewBounds(const Point& viewPos)
    {
        //  calculate view anchor
        //  this is the stage bounds for our view,
        _viewBounds.min = viewPos - _viewDim*0.5f;
        _viewBounds.max = viewPos + _viewDim*0.5f;
        
        //  - now adjust to include some extra space around the viewport, which
        //  is necessary when some tiles are larger than our standard tile size
        const uint32_t kOverlayTilePerFloor = _tileGridMap.overlayToFloorRatio();
        _viewBounds.min -= _tileDim * (2.f*kOverlayTilePerFloor);
        _viewBounds.max += _tileDim * (2.f*kOverlayTilePerFloor);
        
        //  our bounds aligned to tile dimensions - this is used as the bounding rect
        //  for drawing our isostage
        _viewAlignedBounds.min.x = _tileDim.x * floorf(_viewBounds.min.x/_tileDim.x);
        _viewAlignedBounds.min.y = _tileDim.y * floorf(_viewBounds.min.y/_tileDim.y);
        _viewAlignedBounds.min.z = _tileDim.z * floorf(_viewBounds.min.z/_tileDim.z);
        _viewAlignedBounds.max.x = _tileDim.x * floorf(_viewBounds.max.x/_tileDim.x);
        _viewAlignedBounds.max.y = _tileDim.y * floorf(_viewBounds.max.y/_tileDim.y);
        _viewAlignedBounds.max.z = _tileDim.z * floorf(_viewBounds.max.z/_tileDim.z);

        _screenOffset.x = (_viewDim.x - _viewBounds.max.x + _viewBounds.min.x);
        _screenOffset.y = (_viewDim.y - _viewBounds.max.y + _viewBounds.min.y);
        _screenOffset /= 2;
    }

} /* namespace ovengine */ } /* namespace cinekine */
