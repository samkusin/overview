//
//  GameView.cpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "GameView.hpp"
#include "Engine/Render/Renderer.hpp"
#include "Engine/Debug.hpp"
#include "Engine/Theater.hpp"

#include "cinek/overview/map.hpp"
#include "cinek/rendermodel/tiledatabase.hpp"

/////////////////////////////////////////////////////////////

namespace cinekine {
    namespace ovengine {
        //  must be defined by the implementing application.
        View* CreateView(Renderer& cli)
        {
            Allocator allocator;
            return allocator.newItem<prototype::GameView>(cli);
        }

        //  must be defined by the implementing application - destroys the View created by
        //  CreateView
        void DestroyView(View* view)
        {
            Allocator allocator;
            allocator.deleteItem(view);
        }
    }
}

namespace cinekine {
    namespace prototype {
    
    const int32_t TILE_WIDTH = 64;
    const int32_t TILE_HEIGHT = 32;
    const int32_t TILE_HALFWIDTH = TILE_WIDTH/2;
    const int32_t TILE_HALFHEIGHT = TILE_HEIGHT/2;

    GameView::GameView(ovengine::Renderer& renderer) :
        _renderer(renderer),
        _pen(renderer),
        _worldViewBounds()
    {
        
    }
    
    GameView::~GameView()
    {
        
    }
    
    //  A new viewpoint has been initialized by our controller.  Replace our reference with the new one.
    //
    void GameView::onMapSet(std::shared_ptr<overview::Map> &map,
                            const cinek_ov_pos& pos)
    {
        _map = map;
        if (_map)
        {
            setupWorldSpace(xlatMapToWorldPos(pos));
        }
    }
    
    void GameView::onMapSetPosition(const cinek_ov_pos& pos)
    {
        setupWorldSpace(xlatMapToWorldPos(pos));
    }
    
    //  precalculates values used for rendering the local view
    void GameView::setupWorldSpace(const cinek_ov_pos& worldPos)
    {
        SDL_Rect viewportRect = _pen.getViewport();

        //  calculate view anchor
        _worldViewBounds.min.x = worldPos.x - viewportRect.w / 2;
        _worldViewBounds.min.y = worldPos.y - viewportRect.h / 2;
        _worldViewBounds.min.z = 0;
        _worldViewBounds.max.x = worldPos.x + viewportRect.w / 2;
        _worldViewBounds.max.y = worldPos.y + viewportRect.h / 2;
        _worldViewBounds.max.z = 0;
    }
        
    //  converts map coordinates to our "global" rendering coordinate system
    //  called "world" for the purposes of this view.
    cinek_ov_pos GameView::xlatMapToWorldPos(const cinek_ov_pos& pos)
    {
        cinek_ov_pos worldPos;
        worldPos.x = TILE_WIDTH * (pos.x - pos.y)/2;
        worldPos.y = TILE_HEIGHT * (pos.x + pos.y)/2;
        worldPos.z = pos.z;
        return worldPos;
    }
    
    cinek_ov_pos GameView::xlatWorldToMapPos(const cinek_ov_pos& pos)
    {
        cinek_ov_pos mapPos;
        mapPos.x = pos.x/TILE_WIDTH + pos.y/TILE_HEIGHT;
        mapPos.y = 2 * pos.y/TILE_HEIGHT - mapPos.x;
        mapPos.z = pos.z;
        return mapPos;
    }
    
    //  Main render pipeline
    //
    void GameView::render()
    {
        SDL_Rect viewportRect = _pen.getViewport();
        
        const cinek_ov_map_bounds& mapBounds = _map->getMapBounds();
        const rendermodel::TileDatabase& tileDb = _renderer.getTheater().getTileDatabase();

        //  pick all tiles within the view bounds:
        //
        int32_t worldY = _worldViewBounds.min.y;
        int32_t worldEndY = _worldViewBounds.max.y;
    
        //  left to right
        //  top to bottom
        const overview::Tilemap* tilemap = _map->getTilemapAtZ(0);
        int32_t rowCount = 0;
        const int32_t kScreenViewLeft = (viewportRect.w - _worldViewBounds.max.x +_worldViewBounds.min.x)/2;
        const int32_t kScreenViewTop = (viewportRect.h - _worldViewBounds.max.y +_worldViewBounds.min.y)/2;
       
        while (worldY <= worldEndY)
        {
            int32_t worldX = _worldViewBounds.min.x;
            int32_t worldEndX = _worldViewBounds.max.x;
            if (rowCount & 1)
            {
                worldX -= TILE_WIDTH/2;
                worldEndX += TILE_WIDTH/2;
            }
            
            cinek_ov_pos worldTilePos { (float)worldX, (float)worldY, 0 };
            while (worldX <= worldEndX)
            {
                worldTilePos.x = worldX;
                
                //  determine tile lower left origin as the tile anchor for drawing.
                float tileWorldOX = worldTilePos.x - TILE_WIDTH*0.5f;
                float tileWorldOY = worldTilePos.y + TILE_HEIGHT*0.5f;
                
                int32_t screenOX = tileWorldOX - _worldViewBounds.min.x + kScreenViewLeft;
                int32_t screenOY = tileWorldOY - _worldViewBounds.min.y + kScreenViewTop;
                
                //  find world tile from X,Y
                cinek_ov_pos mapPos = xlatWorldToMapPos(worldTilePos);
                if (mapPos.y >= 0.f && mapPos.y < mapBounds.yUnits &&
                    mapPos.x >= 0.f && mapPos.x < mapBounds.xUnits)
                {
                    cinek_tile tile = tilemap->at((uint32_t)mapPos.x, (uint32_t)mapPos.y);
                    const cinek_bitmap& tileInfo = tileDb.getTileInfo(tile);
                    _pen.setBitmapAtlas(tileInfo.bmpAtlas);
                    _pen.drawBitmap(tileInfo.bmpIndex, screenOX, screenOY);
                }
                worldX += TILE_WIDTH;
            }
            worldY += TILE_HEIGHT/2;
            ++rowCount;
        }
        
        _pen.setFont(ovengine::kFontHandle_Default);
        SDL_Color color;
        color.r = 255;
        color.g = 0;
        color.b = 0;
        color.a = 255;
        _pen.drawText("Welcome to the overview 2D project by Samir Sinha", 20, 160, color);
    }
        
    }
}