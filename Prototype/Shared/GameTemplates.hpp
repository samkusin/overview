//
//  GameTemplates.hpp
//  Overview
//
//  Created by Samir Sinha on 10/16/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Model_GameTemplates_hpp
#define Overview_Model_GameTemplates_hpp

#include "Engine/Model/EntityTemplateCollection.hpp"

#include "Engine/Model/TileLibrary.hpp"
#include "Engine/Model/SpriteCollection.hpp"
#include "Core/JsonUtilities.hpp"

#include "cinek/allocator.hpp"

namespace cinek {
    namespace glx {
        class BitmapLibrary;
    }
}
namespace cinek { namespace overview {

    class GameTemplates
    {
    public:
        struct InitParams
        {
            const char* gameDefinitionPath = "";
            uint32_t tileSlotLimit = 32;
            uint32_t spriteLimit = 32;
        };

        GameTemplates(const InitParams& params, const Allocator& allocator);
        ~GameTemplates();
        
        void loadTileCollection(const char* pathname,
                                glx::BitmapLibrary& bitmapLibrary);
        void loadSpriteCollection(const char* pathname,
                                  glx::BitmapLibrary& bitmapLibrary);
        void loadEntityTemplateCollection(const char* pathname);

        const JsonDocument& gameDefinition() const {
            return _gameDefinition;
        }
        const TileLibrary& tileLibrary() const {
            return _tileLibrary;
        }
        const SpriteCollection& spriteCollection() const {
            return _spriteCollection;
        }
        const EntityTemplateCollection& entityTemplateCollection() const {
            return _entityTemplColl;
        }

        Allocator& allocator() {
            return _allocator;
        }
        const Allocator& allocator() const {
            return _allocator;
        }

    private:
        Allocator _allocator;
        JsonDocument _gameDefinition;
        TileLibrary _tileLibrary;
        SpriteCollection _spriteCollection;
        EntityTemplateCollection _entityTemplColl;
    };

} /* namespace overview */ } /* namespace cinek */

#endif /* Overview_Model_GameTemplates_hpp */
