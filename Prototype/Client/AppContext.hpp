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
#include "Engine/Entity/EntityTypes.hpp"

#include <cinek/json/jsontypes.hpp>
#include <cinek/map.hpp>
#include <cinek/string.hpp>

namespace cinek { namespace ovproto {

    //  JsonDocument doesn't allow copy or move - so we must persist by pointer
    using AppDocumentMap = unordered_map<uint32_t, unique_ptr<cinek::JsonDocument>>;
    
    struct AppObjects
    {
        //  game state
        overview::EntityStore* entityStore;
        
        Allocator* allocator;
        cinek::JsonDocument* entityTemplates;
        AppDocumentMap* documentMap;
        
        //  callbacks
        overview::CustomComponentCreateFn createComponentCb;
        overview::CustomComponentDestroyFn destroyComponentCb;
    };
    
    class AppContext
    {
    public:
        AppContext(): _o(nullptr) {}
        AppContext(AppObjects& objs) : _o(&objs) {}
        
    private:
        friend class AppInterface;
        friend class RenderService;
        friend class EntityService;
        
        AppObjects* _o;
    };
    
    //  Document IDs
    enum
    {
        kDocumentId_EntityTemplates
    };

} /* namespace ovproto */ } /* namespace cinek */
\
#endif
