//
//  GameView.cpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "GameView.hpp"

#include "Engine/Debug.hpp"
#include "Engine/Model/Stage.hpp"
#include "Engine/Model/TileDatabase.hpp"
#include "Graphics/RendererCLI.hpp"
#include "Graphics/Graphics2D.hpp"
#include "Graphics/FontLibrary.hpp"
#include "Graphics/BitmapLibrary.hpp"


/////////////////////////////////////////////////////////////

namespace cinekine {
    namespace prototype {

    const int32_t TILE_WIDTH = 128;
    const int32_t TILE_HEIGHT = 64;
    const int32_t TILE_HALFWIDTH = TILE_WIDTH/2;
    const int32_t TILE_HALFHEIGHT = TILE_HEIGHT/2;

    GameView::GameView(glx::RendererCLI& renderer,
                       glx::FontLibrary& fontLibrary,
                       glx::BitmapLibrary& bitmapLibrary) :
        _renderer(renderer),
        _bitmapLibrary(bitmapLibrary),
        _graphics(renderer, bitmapLibrary, fontLibrary),
        _screenViewLeft(0), _screenViewTop(0),
        _currentAtlasIndex(kCinekBitmapAtlas_Invalid),
        _currentAtlas(nullptr),
        _tilemap(nullptr)
    {

    }

    GameView::~GameView()
    {

    }

    //  A new viewpoint has been initialized by our controller.  Replace our reference with the new one.
    //
    void GameView::setStage(std::shared_ptr<ovengine::Stage> stage, const glm::vec3& pos)
    {
        _stage = stage;
        if (_stage)
        {
            setupWorldSpace(xlatMapToWorldPos(pos));
        }
    }

    void GameView::setMapPosition(const glm::vec3& pos)
    {
        setupWorldSpace(xlatMapToWorldPos(pos));
    }

    //  precalculates values used for rendering the local view
    void GameView::setupWorldSpace(const glm::vec3& worldPos)
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
    glm::vec3 GameView::xlatMapToWorldPos(const glm::vec3& pos)
    {
        glm::vec3 worldPos;
        worldPos.x = TILE_WIDTH * (pos.x - pos.y)/2;
        worldPos.y = TILE_HEIGHT * (pos.x + pos.y)/2;
        worldPos.z = pos.z;
        return worldPos;
    }

    glm::vec3 GameView::xlatWorldToMapPos(const glm::vec3& pos)
    {
        glm::vec3 mapPos;
        mapPos.x = pos.x/TILE_WIDTH + pos.y/TILE_HEIGHT;
        mapPos.y = 2 * pos.y/TILE_HEIGHT - mapPos.x;
        mapPos.z = pos.z;
        return mapPos;
    }

    //  Main render pipeline
    //
    void GameView::render()
    {
        renderReset();
        
        renderTileMap(0);
        
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
        
    void GameView::renderReset()
    {
        _currentAtlasIndex = kCinekBitmapAtlas_Invalid;
        _currentAtlas = nullptr;
        _mapBounds = _stage->map().bounds();
    }
        
    void GameView::renderTileMap(int tileZ)
    {
        _tilemap = _stage->map().tilemapAtZ(tileZ);
        if (!_tilemap)
            return;

        //  pick all tiles within the view bounds:
        //
        int32_t worldY = _worldViewAlignedBounds.min.y;
        int32_t worldEndY = _worldViewAlignedBounds.max.y;
        
        //  left to right
        //  top to bottom
        int32_t rowCount = 0;
        
        while (worldY <= worldEndY)
        {
            int32_t worldX = _worldViewAlignedBounds.min.x;
            int32_t worldEndX = _worldViewAlignedBounds.max.x;
            if (rowCount & 1)
            {
                worldX -= TILE_HALFWIDTH;
                worldEndX += TILE_HALFWIDTH;
            }
            
            glm::vec3 worldTilePos { (float)worldX, (float)worldY, 0 };
            while (worldX <= worldEndX)
            {
                worldTilePos.x = worldX;
                glm::vec3 mapPos = xlatWorldToMapPos(worldTilePos);
                if (mapPos.y >= 0.f && mapPos.y < _mapBounds.yUnits &&
                    mapPos.x >= 0.f && mapPos.x < _mapBounds.xUnits)
                {
                    auto tile = _tilemap->at((uint32_t)mapPos.y, (uint32_t)mapPos.x);
                    renderTile(tile, worldTilePos, 0);
                    renderTile(tile, worldTilePos, 1);
                }
                worldX += TILE_WIDTH;
            }
            worldY += TILE_HALFHEIGHT;
            ++rowCount;
        }
    }
        
    void GameView::renderTile(const ovengine::Tile& tile, const glm::vec3& worldPos, int layer)
    {
        //  find world tile from X,Y
        glm::vec3 mapPos = xlatWorldToMapPos(worldPos);
        if (mapPos.y >= 0.f && mapPos.y < _mapBounds.yUnits &&
            mapPos.x >= 0.f && mapPos.x < _mapBounds.xUnits)
        {
            //  our 2d draw anchor is the lower left of our isotile.
            float tileLocalOX = worldPos.x - TILE_HALFWIDTH;
            float tileLocalOY = worldPos.y + TILE_HEIGHT;
            int32_t screenOX = tileLocalOX - _worldViewBounds.min.x + _screenViewLeft;
            int32_t screenOY = tileLocalOY - _worldViewBounds.min.y + _screenViewTop;
        
            const auto& tileInfo = _stage->tileDatabase().tileInfo(tile.layer[layer]);
            if (_currentAtlasIndex != tileInfo.bitmap.bmpAtlas)
            {
                _currentAtlas = _bitmapLibrary.getAtlas(tileInfo.bitmap.bmpAtlas);
                _currentAtlasIndex = tileInfo.bitmap.bmpAtlas;
            }
            
            if (_currentAtlas)
            {
                const glx::BitmapInfo* bitmapInfo = _currentAtlas->getBitmapFromIndex(tileInfo.bitmap.bmpIndex);
                if (bitmapInfo)
                {
                    glx::RGBAColor color(255,255,255,(uint8_t)(1.0*255));
                    glx::Rect srcRect(bitmapInfo->x, bitmapInfo->y,
                                      bitmapInfo->x+bitmapInfo->w, bitmapInfo->y+bitmapInfo->h);
                    glx::Rect destRect = glx::Rect::rectFromDimensions(screenOX + bitmapInfo->offX,
                                                             screenOY - bitmapInfo->srcH + bitmapInfo->offY,
                                                             bitmapInfo->offW,
                                                             bitmapInfo->offH);
                    _renderer.drawTextureRect(_currentAtlas->getTexture(), srcRect, destRect, color);
                }
            }
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