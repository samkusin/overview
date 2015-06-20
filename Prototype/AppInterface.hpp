//
//  AppInterface.hpp
//  Overview
//
//  Created by Samir Sinha on 6/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_AppInterface_hpp
#define Overview_App_AppInterface_hpp

#include "GameTypes.hpp"

#include <cinek/json/json.hpp>

namespace cinek {
    namespace ovproto {
        

class AppInterface
{
public:
    AppInterface(AppContext& context);
    
    //  loads a document from a JSON source file
    void createJsonDocumentFromFile(uint32_t id, const char* pathname);
    //  destroys a JsonDocument
    void destroyJsonDocument(uint32_t id);
    //  Obtain an JsonDocument node by name.  If not found, this will return
    //  a new document (not recommended, but allowed so controller code can
    //  assume that documents exist - even if empty, preventing regular null
    //  pointer checks, etc.)
    JsonDocument& jsonDocument(uint32_t id);
    //  Returns a document by name - if not found, returns a null, immutable
    //  document.
    const JsonDocument& jsonDocument(uint32_t id) const;
    
    //  creates an entity and broadcasts the event to subscribers
    overview::Entity createEntity(uint32_t srcTemplatesDocId,
            const char* templateName);
    
    overview::EntityDatabase* entityDb();
    
private:
    AppContext* _context;
    
    static JsonDocument sNullDocument;
};
    
    } /* namespace overview */
} /* namespace cinek */

#endif /* defined(Overview_App_AppInterface_hpp) */
