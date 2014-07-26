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
#include "Engine/Model/Stage.hpp"
#include "Engine/Model/Sprite.hpp"
#include "Engine/Model/TileLibrary.hpp"
#include "Engine/Model/SpriteLibrary.hpp"
#include "Graphics/Graphics2D.hpp"
#include "Graphics/RenderTypes.hpp"
#include "Graphics/BitmapLibrary.hpp"
#include "Graphics/FontLibrary.hpp"
#include "Core/JsonUtilities.hpp"

#include "cinek/allocator.hpp"

#include <array>


namespace cinekine {
    namespace prototype {
        class ApplicationController;
    }
}

namespace cinekine {
    namespace prototype {

    class GameView: public ovengine::View
    {
    public:
        GameView(ApplicationController& application,
                 const Allocator& allocator);
        virtual ~GameView();
        
        virtual void update();

        virtual void render();

        virtual void onMouseButtonDown(MouseButton button, int32_t x, int32_t y);
        virtual void onMouseButtonUp(MouseButton button, int32_t x, int32_t y);
        virtual void onMouseMove(MouseRegion region, int32_t x, int32_t y);
        virtual void onKeyDown(SDL_Keycode keycode, uint16_t keymod);
        virtual void onKeyUp(SDL_Keycode keycode, uint16_t keymod);
        
    private:
        void loadTileCollection(const char* filename);
        void loadSpriteCollection(const char* filename);

    private:
        //  precalculates values used for rendering the local view
        void setupWorldSpace(const glm::vec3& worldPos);
        //  utilities
        glm::vec3 xlatMapToWorldPos(const glm::vec3& pos);
        glm::vec3 xlatWorldToMapPos(const glm::vec3& pos);
        
        void renderReset();
        void renderTileMap(int tileZ);
        void renderTile(const glm::vec3& worldPos, const glm::vec3& mapPos);
        void renderQueueBitmap(const cinek_bitmap& bitmap, const glm::vec3& worldPos);
        void renderQueueFlush();
        void renderBitmap(const glx::Texture& texture, const glx::BitmapInfo& bitmap,
                          int32_t x, int32_t y);
        
    private:
        ApplicationController& _application;
        Allocator _allocator;
        glx::BitmapLibrary _bitmapLibrary;
        glx::FontLibrary _fontLibrary;
        glx::Graphics2D _graphics;
        
        ovengine::TileLibrary _tileLibrary;
        ovengine::SpriteLibrary _spriteLibrary;
        
        JsonDocument _gameDocument;
        
        std::shared_ptr<ovengine::Stage> _stage;
        
        glm::vec3 _viewPos;
        
        struct ViewBounds
        {
            glm::vec3 min;
            glm::vec3 max;
        };

        ViewBounds _worldViewBounds;
        ViewBounds _worldViewAlignedBounds;
        int32_t _screenViewLeft, _screenViewTop;

        ovengine::MapBounds _mapBounds;
        const ovengine::TileGridMap* _tilemap;
        
        //  used for rendering
        struct RenderItem
        {
            glm::vec3 pos;
            const glx::BitmapAtlas* bmpAtlas;
            const glx::BitmapInfo* bmpInfo;
        };
        
        std::array<RenderItem, 16> _renderItems;
        std::array<RenderItem*, 16> _renderItemsSorted;
        uint32_t _renderItemsCount;
    };

    }
}

#endif
