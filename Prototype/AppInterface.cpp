//
//  AppInterface.cpp
//  Overview
//
//  Created by Samir Sinha on 6/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "AppInterface.hpp"

#include "Engine/Entity/EntityFactory.hpp"
#include "Engine/Entity/EntityDatabase.hpp"

#include <cinek/debug.hpp>
#include <cinek/json/jsonstreambuf.hpp>
#include <cinek/filestreambuf.hpp>

namespace cinek {
    namespace ovproto {


////////////////////////////////////////////////////////////////////////////////

JsonDocument AppInterface::sNullDocument;

AppInterface::AppInterface(AppContext& context) :
    _context(&context)
{
}

void AppInterface::createJsonDocumentFromFile
(
    uint32_t id,
    const char* pathname
)
{
    FileStreamBuf streamBuf(pathname);
    
    if (!streamBuf)
    {
        CK_LOG_ERROR("ovproto", "AppInterface.createJsonDocumentFromFile - "
                     "Failed to load %s", pathname);
        return;
    }
    
    RapidJsonStdStreamBuf jsonStreamBuf(streamBuf);
    
    JsonDocument& document = jsonDocument(id);
    document.ParseStream<0>(jsonStreamBuf);
}

void AppInterface::destroyJsonDocument(uint32_t id)
{
    auto it = _context->documentMap->find(id);
    if (it == _context->documentMap->end())
        return;
    
    _context->documentMap->erase(it);
}

//  Obtain an JsonDocument node by name.  If not found, this will return
//  a new document (not recommended, but allowed so controller code can
//  assume that documents exist - even if empty, preventing regular null
//  pointer checks, etc.)
JsonDocument& AppInterface::jsonDocument(uint32_t id)
{
    auto it = _context->documentMap->find(id);
    if (it == _context->documentMap->end())
    {
        auto newDoc = allocate_unique<cinek::JsonDocument>(*_context->allocator);
        it = _context->documentMap->emplace(id, std::move(newDoc)).first;
    }
    CK_ASSERT(it != _context->documentMap->end());
    return *it->second;
}

const JsonDocument& AppInterface::jsonDocument(uint32_t id) const
{
    auto it = _context->documentMap->find(id);
    if (it == _context->documentMap->end())
        return sNullDocument;
    
    return *it->second;
}

overview::EntityObject* AppInterface::createEntity
(
    uint32_t srcTemplatesDocId,
    const char* templateName
)
{
    return overview::createEntity(*_context->entityDb,
        *_context->renderResources,
        *_context->documentMap->find(srcTemplatesDocId)->second,
        templateName,
        _context->messagePublisher,
        _context->createComponentCb);
}


overview::EntityDatabase* AppInterface::entityDb()
{
    return _context->entityDb;
}

overview::EntityObject* AppInterface::entityById(EntityId eid)
{
    return _context->entityDb->entityObjectFromId(eid);
}
    


        
    } /* namespace ovproto */
} /* namespace cinek */