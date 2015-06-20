//
//  SimulationTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 11/22/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Types_hpp
#define Overview_Game_Types_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/MessageTypes.hpp"
#include "Engine/Entity/EntityTypes.hpp"
#include "Engine/Render/RenderTypes.hpp"
#include "Engine/AABB.hpp"

#include <cinek/json/jsontypes.hpp>
#include <cinek/map.hpp>
#include <cinek/string.hpp>

namespace cinek {
    namespace ovproto {
    
    //  bring certain Engine types over into our namespace
    using Entity = overview::Entity;
    using Vector2 = overview::Vector2;
    using Vector3 = overview::Vector3;
    using Matrix4 = overview::Matrix4;
    template<typename Point> using AABB = overview::AABB<Point>;
    
    //  JsonDocument doesn't allow copy or move - so we must persist by pointer
    using AppDocumentMap = unordered_map<uint32_t, unique_ptr<cinek::JsonDocument>>;
    
    struct AppContext
    {
        //  objects
        Allocator* allocator;
        overview::MessagePublisher* messagePublisher;
        overview::EntityDatabase* entityDb;
        overview::RenderResources* renderResources;
        AppDocumentMap* documentMap;
        
        //  callbacks
        overview::CustomComponentCreateFn createComponentCb;
    };
    
    
    class AppInterface;
    
    //  Document IDs
    enum
    {
        kDocumentId_EntityTemplates
    };
    
    } /* namespace overview */
} /* namespace cinek */

#endif
