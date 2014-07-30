//
//  IsoScene.cpp
//  Overview
//
//  Created by Samir Sinha on 7/26/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/Render/IsoScene.hpp"
#include "Engine/Model/TileLibrary.hpp"
#include "Graphics/RendererCLI.hpp"
#include "Graphics/Rect.hpp"

namespace cinekine { namespace ovengine {
    
    IsoScene::IsoScene(const glm::ivec2& viewDimensions,
                       const glm::ivec2& tileDimensions,
                       const ovengine::TileGridMap& tileGridMap,
                       const ovengine::TileLibrary& tileLibrary,
                       const ovengine::SpriteLibrary& spriteLibrary,
                       const Allocator& allocator) :
        _tileGridMap(tileGridMap),
        _tileLibrary(tileLibrary),
        _spriteLibrary(spriteLibrary),
        _tileDim(tileDimensions),
        _viewDim(viewDimensions),
        _allocator(allocator),
        _isoNodeGraph(1024, allocator)
    {
        auto& overlayGrid = _tileGridMap.overlay();
        _centerPos = glm::vec3(overlayGrid.columnCount() * 0.5f,
                               overlayGrid.rowCount() * 0.5f,
                               0.f);
        _isoWorldBounds.min = glm::vec3(0,0,0);
        _isoWorldBounds.max = glm::vec3(overlayGrid.columnCount(),
                                    overlayGrid.rowCount(),
                                    _tileGridMap.overlayToFloorRatio()*1.f);
        setupViewBounds(isoToViewPos(_centerPos));
    }
        
    void IsoScene::update(const glm::vec3& pos)
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
        int32_t viewY = _viewAlignedBounds.min.y;
        int32_t viewEndY = _viewAlignedBounds.max.y;
        
        //  left to right
        //  top to bottom
        int32_t rowCount = 0;
        
        while (viewY <= viewEndY)
        {
            int32_t viewX = _viewAlignedBounds.min.x;
            int32_t viewEndX = _viewAlignedBounds.max.x;
            if (rowCount & 1)
            {
                viewX -= _tileDim.x/2;
                viewEndX += _tileDim.x/2;
            }
            
            glm::vec3 viewTilePos(viewX, viewY, 0);
            while (viewX <= viewEndX)
            {
                viewTilePos.x = viewX;
                glm::vec3 isoTilePos = viewToIsoPos(viewTilePos);
                if (isoTilePos.x >= _isoWorldBounds.min.x && isoTilePos.x < _isoWorldBounds.max.x &&
                    isoTilePos.y >= _isoWorldBounds.min.y && isoTilePos.y < _isoWorldBounds.max.y &&
                    isoTilePos.z >= _isoWorldBounds.min.z && isoTilePos.z < _isoWorldBounds.max.z)
                {
                    attachTileToGraph(viewTilePos - _viewBounds.min, isoTilePos);
                }
                viewX += _tileDim.x;
            }
            viewY += _tileDim.y/2;
            ++rowCount;
        }
        
        // sort nodes.  once complete, the graph will be ready for traversal
        //
        _isoNodeGraph.sort();
    }
    
    void IsoScene::attachTileToGraph(const glm::vec3& viewPos,
                                     const glm::vec3& isoPos)
    {
        const uint32_t kOverlayTilePerFloor = _tileGridMap.overlayToFloorRatio();
        const uint32_t overlayY = (uint32_t)isoPos.y;
        const uint32_t overlayX = (uint32_t)isoPos.x;
        const uint32_t floorY = overlayY / kOverlayTilePerFloor;
        const uint32_t floorX = overlayX / kOverlayTilePerFloor;
        AABB<glm::vec3> isoBox;
        
        //  floor tile
        if (!(overlayX % kOverlayTilePerFloor) && !(overlayY % kOverlayTilePerFloor))
        {
            auto tileId = _tileGridMap.floor().at(floorY, floorX);
            if (tileId)
            {
                auto& tileInfo = tileFromId(tileId);

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
            auto& tileInfo = tileFromId(tileId);
            
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

    glm::vec3 IsoScene::isoToViewPos(const glm::vec3& isoPos) const
    {
        glm::vec3 viewPos(_tileDim.x * (isoPos.x - isoPos.y)/2,
                          _tileDim.y * (isoPos.x + isoPos.y)/2,
                          isoPos.z);
        return viewPos;
    }
    
    glm::vec3 IsoScene::viewToIsoPos(const glm::vec3& viewPos) const
    {
        glm::vec3 isoPos;
        isoPos.x = viewPos.x/_tileDim.x + viewPos.y/_tileDim.y;
        isoPos.y = 2 * viewPos.y/_tileDim.y - isoPos.x;
        isoPos.z = viewPos.z;
        return isoPos;
    }
    
    const Tile& IsoScene::tileFromId(TileId id) const
    {
        return _tileLibrary.tileFromCollectionAtIndex(slotFromTileId(id),
                                                      indexFromTileId(id));
    }
    
    //  precalculates values used for rendering the local view
    void IsoScene::setupViewBounds(const glm::vec3& viewPos)
    {
        //  calculate view anchor
        //  this is the world bounds for our view,
        _viewBounds.min.x = viewPos.x - _viewDim.x / 2;
        _viewBounds.min.y = viewPos.y - _viewDim.y / 2;
        _viewBounds.min.z = 0;
        _viewBounds.max.x = viewPos.x + _viewDim.x / 2;
        _viewBounds.max.y = viewPos.y + _viewDim.y / 2;
        _viewBounds.max.z = 0;
        
        //  - now adjust to include some extra space around the viewport, which
        //  is necessary when some tiles are larger than our standard tile size
        const uint32_t kOverlayTilePerFloor = _tileGridMap.overlayToFloorRatio();
        _viewBounds.min.x -= _tileDim.x*2*kOverlayTilePerFloor;
        _viewBounds.min.y -= _tileDim.y*2*kOverlayTilePerFloor;
        _viewBounds.max.x += _tileDim.x*2*kOverlayTilePerFloor;
        _viewBounds.max.y += _tileDim.y*2*kOverlayTilePerFloor;
        
        //  our bounds aligned to tile dimensions - this is used as the bounding rect
        //  for drawing our isoworld
        _viewAlignedBounds.min.x = _tileDim.x * floorf(_viewBounds.min.x/_tileDim.x);
        _viewAlignedBounds.min.y = _tileDim.y * floorf(_viewBounds.min.y/_tileDim.y);
        _viewAlignedBounds.min.z = 0;
        _viewAlignedBounds.max.x = _tileDim.x * floorf(_viewBounds.max.x/_tileDim.x);
        _viewAlignedBounds.max.y = _tileDim.y * floorf(_viewBounds.max.y/_tileDim.y);
        _viewAlignedBounds.max.z = 0;
        
        _screenOffset.x = (_viewDim.x - _viewBounds.max.x + _viewBounds.min.x);
        _screenOffset.y = (_viewDim.y - _viewBounds.max.y + _viewBounds.min.y);
        _screenOffset /= 2;
    }

} /* namespace ovengine */ } /* namespace cinekine */
