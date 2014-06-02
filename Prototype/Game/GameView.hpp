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
#include "Graphics/Graphics2D.hpp"
#include "Graphics/RenderTypes.hpp"

#include "cinek/allocator.hpp"

#include <array>


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

        void setStage(std::shared_ptr<ovengine::Stage> stage, const glm::vec3& pos);
        void setMapPosition(const glm::vec3& pos);

        virtual void render();

        virtual void onMouseButtonDown(MouseButton button, int32_t x, int32_t y);
        virtual void onMouseButtonUp(MouseButton button, int32_t x, int32_t y);
        virtual void onMouseMove(MouseRegion region, int32_t x, int32_t y);

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
        std::shared_ptr<ovengine::Stage> _stage;
        glx::RendererCLI& _renderer;
        glx::BitmapLibrary& _bitmapLibrary;
        glx::Graphics2D _graphics;
        
        struct ViewBounds
        {
            glm::vec3 min;
            glm::vec3 max;
        };

        ViewBounds _worldViewBounds;
        ViewBounds _worldViewAlignedBounds;
        int32_t _screenViewLeft, _screenViewTop;

        ovengine::MapBounds _mapBounds;
        ovengine::Tilemap* _tilemap;
        
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
