//
//  GameView.cpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "GameView.hpp"
#include "Graphics/RendererCLI.hpp"
#include "Engine/Debug.hpp"
#include "Engine/TheaterCLI.hpp"
#include "Engine/Map.hpp"
#include "Graphics/Graphics2D.hpp"
#include "Graphics/FontLibrary.hpp"
#include "Graphics/BitmapLibrary.hpp"

#include "cinek/rendermodel/tiledatabase.hpp"

/////////////////////////////////////////////////////////////

namespace cinekine {
    namespace prototype {

    const int32_t TILE_WIDTH = 128;
    const int32_t TILE_HEIGHT = 64;
    const int32_t TILE_HALFWIDTH = TILE_WIDTH/2;
    const int32_t TILE_HALFHEIGHT = TILE_HEIGHT/2;

    GameView::GameView(ovengine::TheaterCLI& theater,
                       glx::RendererCLI& renderer,
                       glx::FontLibrary& fontLibrary,
                       glx::BitmapLibrary& bitmapLibrary) :
        _theater(theater),
        _renderer(renderer),
        _graphics(renderer, bitmapLibrary, fontLibrary),
        _screenViewLeft(0), _screenViewTop(0)
    {

    }

    GameView::~GameView()
    {

    }

    //  A new viewpoint has been initialized by our controller.  Replace our reference with the new one.
    //
    void GameView::setMap(std::shared_ptr<ovengine::Map> &map, const cinek_ov_pos& pos)
    {
        _map = map;
        if (_map)
        {
            setupWorldSpace(xlatMapToWorldPos(pos));
        }
    }

    void GameView::setMapPosition(const cinek_ov_pos& pos)
    {
        setupWorldSpace(xlatMapToWorldPos(pos));
    }

    //  precalculates values used for rendering the local view
    void GameView::setupWorldSpace(const cinek_ov_pos& worldPos)
    {
        glx::Rect viewportRect = _renderer.getViewport();

        //  calculate view anchor
        //  this is the world bounds for our view,
        _worldViewBounds.min.x = worldPos.x - viewportRect.width() / 2;
        _worldViewBounds.min.y = worldPos.y - viewportRect.height() / 2;
        _worldViewBounds.min.z = 0;
        _worldViewBounds.max.x = worldPos.x + viewportRect.width() / 2;
        _worldViewBounds.max.y = worldPos.y + viewportRect.height() / 2;
        _worldViewBounds.max.z = 0;
        
        //  - now adjust to include some extra space around the viewport, which
        //  is necessary when some tiles are larger than our standard tile size
        _worldViewBounds.min.x -= TILE_WIDTH*2;
        _worldViewBounds.min.y -= TILE_HEIGHT*2;
        _worldViewBounds.max.x += TILE_WIDTH*2;
        _worldViewBounds.max.y += TILE_HEIGHT*2;
        
        //  our bounds aligned to tile dimensions - this is used as the bounding rect
        //  for drawing our isoworld
        _worldViewAlignedBounds.min.x = TILE_WIDTH * floorf(_worldViewBounds.min.x/TILE_WIDTH);
        _worldViewAlignedBounds.min.y = TILE_HEIGHT * floorf(_worldViewBounds.min.y/TILE_HEIGHT);
        _worldViewAlignedBounds.min.z = 0;
        _worldViewAlignedBounds.max.x = TILE_WIDTH * floorf(_worldViewBounds.max.x/TILE_WIDTH);
        _worldViewAlignedBounds.max.y = TILE_HEIGHT * floorf(_worldViewBounds.max.y/TILE_HEIGHT);
        _worldViewAlignedBounds.max.z = 0;
        
        _screenViewLeft = (viewportRect.width() - _worldViewBounds.max.x +_worldViewBounds.min.x)/2;
        _screenViewTop = (viewportRect.height() - _worldViewBounds.max.y +_worldViewBounds.min.y)/2;
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
        renderTileMapLayer(0, 0);
        renderTileMapLayer(0, 1);
        
        glx::Style style;
        style.textColor = glx::RGBAColor(255,0,255,255);
        style.textFont = glx::kFontHandle_Default;
        style.fillMethod = glx::kFillMethod_SolidFill;
        style.fillColor = glx::RGBAColor(0,0,255,255);
        _graphics.drawText("Welcome to the overview 2D project by Samir Sinha.",
                            20, 160, style);
        /*
        _graphics.drawRect(glx::Rect(100,100,200,200), style);

        glm::ivec2 polyVerts[5];
        polyVerts[0].x = 300;
        polyVerts[0].y = 100;
        polyVerts[1].x = 375;
        polyVerts[1].y = 175;
        polyVerts[2].x = 340;
        polyVerts[2].y = 250;
        polyVerts[3].x = 260;
        polyVerts[3].y = 250;
        polyVerts[4].x = 225;
        polyVerts[4].y = 175;
        style.fillColor = glx::RGBAColor(255,0,0,255);
        _graphics.drawPolygon(polyVerts, 5, style);
        */
    }
        
    void GameView::renderTileMapLayer(int tileZ, int layer)
    {
        const cinek_ov_map_bounds& mapBounds = _map->bounds();
        const rendermodel::TileDatabase& tileDb = _theater.tileDatabase();
        
        //  pick all tiles within the view bounds:
        //
        int32_t worldY = _worldViewAlignedBounds.min.y;
        int32_t worldEndY = _worldViewAlignedBounds.max.y;
        
        //  left to right
        //  top to bottom
        const auto* tilemap = _map->tilemapAtZ(tileZ);
        int32_t rowCount = 0;
        
        
        cinek_bitmap_atlas currentAtlas = kCinekBitmapAtlas_Invalid;
        
        while (worldY <= worldEndY)
        {
            int32_t worldX = _worldViewAlignedBounds.min.x;
            int32_t worldEndX = _worldViewAlignedBounds.max.x;
            if (rowCount & 1)
            {
                worldX -= TILE_HALFWIDTH;
                worldEndX += TILE_HALFWIDTH;
            }
            
            cinek_ov_pos worldTilePos { (float)worldX, (float)worldY, 0 };
            while (worldX <= worldEndX)
            {
                worldTilePos.x = worldX;
                
                //  our 2d draw anchor is the lower left of our isotile.
                float tileLocalOX = worldTilePos.x - TILE_HALFWIDTH;
                float tileLocalOY = worldTilePos.y + TILE_HEIGHT;
                
                int32_t screenOX = tileLocalOX - _worldViewBounds.min.x + _screenViewLeft;
                int32_t screenOY = tileLocalOY - _worldViewBounds.min.y + _screenViewTop;
                
                //  find world tile from X,Y
                cinek_ov_pos mapPos = xlatWorldToMapPos(worldTilePos);
                if (mapPos.y >= 0.f && mapPos.y < mapBounds.yUnits &&
                    mapPos.x >= 0.f && mapPos.x < mapBounds.xUnits)
                {
                    auto tile = tilemap->at((uint32_t)mapPos.y, (uint32_t)mapPos.x);
                    const auto& floorTileInfo = tileDb.tileInfo(tile.layer[layer]);
                    if (currentAtlas != floorTileInfo.bitmap.bmpAtlas)
                    {
                        _graphics.setBitmapAtlas(floorTileInfo.bitmap.bmpAtlas);
                        currentAtlas = floorTileInfo.bitmap.bmpAtlas;
                    }
                    _graphics.drawBitmapFromAtlas(floorTileInfo.bitmap.bmpIndex, screenOX, screenOY, 1.0f);
                }
                worldX += TILE_WIDTH;
            }
            worldY += TILE_HALFHEIGHT;
            ++rowCount;
        }
    }

    void GameView::onMouseButtonDown(MouseButton button, int32_t x, int32_t y)
    {

    }

    void GameView::onMouseButtonUp(MouseButton button, int32_t x, int32_t y)
    {

    }

    void GameView::onMouseMove(MouseRegion region, int32_t x, int32_t y)
    {
    }

    }
}