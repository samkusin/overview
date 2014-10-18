//
//  GameTemplates.hpp
//  Overview
//
//  Created by Samir Sinha on 10/16/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Model_GameTemplates_hpp
#define Overview_Model_GameTemplates_hpp

#include "Game/EntityTemplateCollection.hpp"

#include "Engine/Model/TileLibrary.hpp"
#include "Engine/Model/SpriteLibrary.hpp"
#include "Core/JsonUtilities.hpp"

#include "cinek/allocator.hpp"

namespace cinekine {
    namespace ovengine {
        class TileGridMap;
        class RoomGraph;
    }
    namespace glx {
        class BitmapLibrary;
    }
} 
namespace cinekine { namespace ovengine {

    class GameTemplates
    {
    public:
        struct InitParams
        {
            const char* gameDefinitionPath;
            uint32_t tileSlotLimit;
            uint32_t spriteLimit;
        };
        
        GameTemplates(const InitParams& params, const Allocator& allocator);
        ~GameTemplates();
        
        void loadTileCollection(const char* pathname,
                                glx::BitmapLibrary& bitmapLibrary);
        void loadSpriteCollection(const char* pathname,
                                  glx::BitmapLibrary& bitmapLibrary);
        void loadEntityTemplateCollection(const char* pathname);
        void loadTileGridMap(unique_ptr<TileGridMap>&& tileGridMap);
        void loadRoomGraph(unique_ptr<RoomGraph>&& roomGraph);
        
        const JsonDocument& gameDefinition() const {
            return _gameDefinition;
        }
        
        const TileLibrary& tileLibrary() const {
            return _tileLibrary;
        }
        const SpriteLibrary& spriteLibrary() const {
            return _spriteLibrary;
        }
        const EntityTemplateCollection& entityTemplateCollection() const {
            return _entityTemplColl;
        }
        const TileGridMap* tileGridMap() const {
            return _tileGridMap.get();
        }
        const RoomGraph* roomGraph() const {
            return _roomGraph.get();
        }
        
        Allocator& allocator() {
            return _allocator;
        }
        
    private:
        Allocator _allocator;
        JsonDocument _gameDefinition;
        TileLibrary _tileLibrary;
        SpriteLibrary _spriteLibrary;
        EntityTemplateCollection _entityTemplColl;
        
        unique_ptr<TileGridMap> _tileGridMap;
        unique_ptr<RoomGraph> _roomGraph;
    };
    
} /* namespace ovengine */ } /* namespace cinekine */

#endif /* Overview_Model_GameTemplates_hpp */
