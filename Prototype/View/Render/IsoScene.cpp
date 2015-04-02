//
//  IsoScene.cpp
//  Overview
//
//  Created by Samir Sinha on 7/26/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "View/Render/IsoScene.hpp"
#include "View/Stage.hpp"
#include "Graphics/RendererCLI.hpp"
#include "Graphics/Rect.hpp"
#include "Graphics/BitmapLibrary.hpp"

namespace cinek { namespace overview {

    IsoScene::IsoScene(const glm::ivec2& viewDimensions,
                       const glm::ivec2& tileDimensions,
                       const overview::Stage& stage,
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
                                   [&context](const overview::SpriteInstanceList& instances)
                                   {
                                       IsoScene* scene = context.scene;
                                       for (auto& instance : instances)
                                       {
                                           auto& isoPos = instance.position();
                                           auto viewPos = scene->isoToViewPos(isoPos);
                                           if (scene->_viewAlignedBounds.contains(viewPos))
                                           {
                                               viewPos.x -= scene->_viewBounds.min.x;
                                               viewPos.y -= scene->_viewBounds.min.y;
                                               glm::ivec2 viewAnchor = scene->_screenOffset;
                                               viewAnchor.x += viewPos.x;
                                               viewAnchor.y += viewPos.y + scene->_tileDim.y;
                                               viewAnchor.y -= viewPos.z;
                                               viewAnchor -= instance.anchor();
                                               AABB<Point> isoBox =  instance.aabb() + isoPos;
                                               scene->_isoNodeGraph.obtainNode(instance.bitmapFromTime(context.ticks),
                                                                               viewAnchor,
                                                                               isoBox,
                                                                               IsoNode::kObject,
                                                                               instance.modelId());
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
                //  aabb assumes an anchor to the X,Y max of the tile
                //  (lower/right on a 2d grid)
                isoBox = tileInfo.aabb + isoPos +
                         Point(kOverlayTilePerFloor,kOverlayTilePerFloor,0);

                glm::ivec2 viewAnchor = _screenOffset;
                viewAnchor.x += viewPos.x - tileInfo.anchor.x;
                viewAnchor.y += viewPos.y + (_tileDim.y * kOverlayTilePerFloor) - tileInfo.anchor.y;

                _isoNodeGraph.obtainNode(tileInfo.bitmap, viewAnchor, isoBox,
                                         IsoNode::kTile, tileId);
            }
        }

        //  overlay tile
        auto tileId = _tileGridMap.overlay().at(overlayY, overlayX);
        if (tileId)
        {
            auto& tileInfo = _stage.tileInfo(tileId);
            //  aabb assumes an anchor to the X,Y max of the tile
            //  (lower/right on a 2d grid)
            isoBox = tileInfo.aabb + isoPos + Point(1,1,0); //

            glm::ivec2 viewAnchor = _screenOffset;
            viewAnchor.x += viewPos.x - tileInfo.anchor.x;
            viewAnchor.y += viewPos.y + _tileDim.y - tileInfo.anchor.y;

            _isoNodeGraph.obtainNode(tileInfo.bitmap, viewAnchor, isoBox,
                                     IsoNode::kTile, tileId);
        }
    }

    void IsoScene::visit(const std::function<void(const IsoNode*)>& fn)
    {
        auto& nodes = _isoNodeGraph.nodes();
        for (auto& node : nodes)
        {
            fn(node);
        }
    }
    
    glm::ivec2 IsoScene::isoToScreenPos(const Point& pt) const
    {
        auto viewPos = isoToViewPos(pt);
        viewPos.x -= _viewBounds.min.x;
        viewPos.y -= _viewBounds.min.y;
        
        glm::ivec2 viewAnchor = _screenOffset;
        viewAnchor.x += viewPos.x;
        viewAnchor.y += viewPos.y;
        viewAnchor.y -= viewPos.z;
        
        return viewAnchor;
    }
    
    Point IsoScene::screenToIsoPos(const glm::ivec2& pt, float z) const
    {
        Point viewPos(pt.x - _screenOffset.x,
            (pt.y - _screenOffset.y) + (z * _tileDim.z),
            z * _tileDim.z);
        viewPos.x += _viewBounds.min.x;
        viewPos.y += _viewBounds.min.y;
        return viewToIsoPos(viewPos);
    }

    Point IsoScene::isoToViewPos(const Point& isoPos) const
    {
        Point viewPos(_tileDim.x * (isoPos.x - isoPos.y)/2,
                      _tileDim.y * (isoPos.x + isoPos.y)/2,
                      isoPos.z * _tileDim.z);
        return viewPos;
    }

    Point IsoScene::viewToIsoPos(const Point& viewPos) const
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
    
    
            
    const IsoNode* IsoScene::pickNode(const glm::ivec2& screenPt,
                                      uint32_t contexts, uint32_t categories) const
    {
        //  Using the current graph, pick the "shallowest" node (smallest depth)
        //  where the node's AABB contains the specified screen point.  Flags
        //  are used for filtering.
        
        //  Note: Since we're testing whether a 2d point resides within an isometric
        //  box, we'll treat the isobox as a hexagon.  The vertices of this hexagon
        //  come from the AABB, translated to screen coordinates.
        
        //  frontmost nodes (screen-wise) are at the end of the list.
        
        auto& nodes = _isoNodeGraph.nodes();
        for (auto it = nodes.rbegin(); it != nodes.rend(); ++it)
        {
            auto node = *it;
            if (!(contexts & node->context()))
                continue;
            if (node->context() == IsoNode::kTile && categories)
            {
                auto& tileInfo = _stage.tileInfo(node->modelId());
                if (!(tileInfo.categories & categories))
                    continue;
            }
            auto& box = node->box();
            
            glm::ivec2 pts[6];  // clockwise from topmost point
            
            //  For each node, check whether screenPt is below the topmost box point
            //  and above the bottommost box point.
            //  use points 0 and 3 are our top and bottom-most points.
            pts[3] = isoToScreenPos(Point(box.max.x, box.max.y, box.min.z));
            if (screenPt.y > pts[3].y)
                break;      // done?
            
            pts[0] = isoToScreenPos(Point(box.min.x, box.min.y, box.max.z));
            if (screenPt.y < pts[0].y)
                continue;   // next node?
            
            //  use points 1 and 4 to filter out points to the right and left
            pts[1] = isoToScreenPos(Point(box.max.x, box.min.y, box.max.z));
            if (screenPt.x > pts[1].x)
                continue;
            pts[4] = isoToScreenPos(Point(box.min.x, box.max.y, box.min.z));
            if (screenPt.x < pts[4].x)
                continue;
            //  construct points 2 and 5, then perform our comprehesive edge check
            pts[2] = isoToScreenPos(Point(box.max.x, box.min.y, box.min.z));
            pts[5] = isoToScreenPos(Point(box.min.x, box.max.y, box.max.z));
            
            if (determineSideOfLineIsPoint(screenPt, pts[0], pts[1]) > 0)
                continue;
            if (determineSideOfLineIsPoint(screenPt, pts[1], pts[2]) > 0)
                continue;
            if (determineSideOfLineIsPoint(screenPt, pts[2], pts[3]) > 0)
                continue;
            if (determineSideOfLineIsPoint(screenPt, pts[3], pts[4]) > 0)
                continue;
            if (determineSideOfLineIsPoint(screenPt, pts[4], pts[5]) > 0)
                continue;
            if (determineSideOfLineIsPoint(screenPt, pts[5], pts[0]) > 0)
                continue;
         
            // hit
            return node;
        }
        
        return nullptr;
    }


    int IsoScene::determineSideOfLineIsPoint(const glm::ivec2& pt,
            const glm::ivec2& p0, const glm::ivec2& p1) const
    {
        //d=(x−x1)(y2−y1)−(y−y1)(x2−x1)
        return (pt.x-p0.x)*(p1.y-p0.y) - (pt.y-p0.y)*(p1.x-p0.x);
    }

} /* namespace overview */ } /* namespace cinek */
