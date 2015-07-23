//
//  AppContext.hpp
//  Overview
//
//  Created by Samir Sinha on 6/29/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Context_hpp
#define Overview_App_Context_hpp

#include "GameTypes.hpp"
#include "Client/UI/UITypes.hpp"

#include "Engine/Render/RenderTypes.hpp"

#include <cinek/json/jsontypes.hpp>
#include <cinek/map.hpp>
#include <cinek/string.hpp>

namespace cinek { namespace ovproto {

    //  JsonDocument doesn't allow copy or move - so we must persist by pointer
    using AppDocumentMap = unordered_map<uint32_t, unique_ptr<cinek::JsonDocument>>;
    
    struct AppContext
    {
        //  objects
        Allocator* allocator;
        overview::MessagePublisher* messagePublisher;
        overview::EntityStore* entityStore;
        overview::ViewStack* viewStack;
        AppDocumentMap* documentMap;
        
        overview::RenderResources* renderResources;
        
        //  viewport
        gfx::Rect viewRect;
        NVGcontext* nvg;
        
        //  callbacks
        overview::CustomComponentCreateFn createComponentCb;
    };
    
    //  Document IDs
    enum
    {
        kDocumentId_EntityTemplates
    };

} /* namespace ovproto */ } /* namespace cinek */
\
#endif
