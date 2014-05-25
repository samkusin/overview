//
//  GameView.hpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_GameView_hpp
#define Overview_GameView_hpp

#include "Engine/View.hpp"
#include "Engine/Model/Map.hpp"
#include "Engine/Model/Sprite.hpp"
#include "Graphics/Graphics2D.hpp"

#include "cinek/allocator.hpp"


namespace cinekine {
    namespace ovengine {
        class Stage;
    }
    namespace glx {
        class RendererCLI;
        class FontLibrary;
        class BitmapLibrary;
    }
}

namespace cinekine {
    namespace prototype {

    class GameView: public ovengine::View
    {
    public:
        GameView(glx::RendererCLI& renderer,
                 glx::FontLibrary& fontLibrary,
                 glx::BitmapLibrary& bitmapLibrary);
        virtual ~GameView();

        void setStage(std::shared_ptr<ovengine::Stage> stage, const cinek_ov_pos& pos);
        void setMapPosition(const cinek_ov_pos& pos);

        virtual void render();

        virtual void onMouseButtonDown(MouseButton button, int32_t x, int32_t y);
        virtual void onMouseButtonUp(MouseButton button, int32_t x, int32_t y);
        virtual void onMouseMove(MouseRegion region, int32_t x, int32_t y);

    private:
        //  precalculates values used for rendering the local view
        void setupWorldSpace(const cinek_ov_pos& worldPos);
        //  utilities
        cinek_ov_pos xlatMapToWorldPos(const cinek_ov_pos& pos);
        cinek_ov_pos xlatWorldToMapPos(const cinek_ov_pos& pos);
        
        void renderReset();
        void renderTileMap(int tileZ);
        void renderTile(const ovengine::Tile& tile, const cinek_ov_pos& worldPos, int layer);
        void renderSprite(const ovengine::Sprite& sprite);

    private:
        std::shared_ptr<ovengine::Stage> _stage;
        glx::RendererCLI& _renderer;
        glx::BitmapLibrary& _bitmapLibrary;
        glx::Graphics2D _graphics;

        cinek_ov_bounds _worldViewBounds;
        cinek_ov_bounds _worldViewAlignedBounds;
        int32_t _screenViewLeft, _screenViewTop;

        cinek_bitmap_atlas _currentAtlasIndex;
        std::shared_ptr<glx::BitmapAtlas> _currentAtlas;
        ovengine::MapBounds _mapBounds;
        ovengine::Tilemap* _tilemap;
    };

    }
}

#endif
