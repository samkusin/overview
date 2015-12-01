//
//  EngineMessages.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/28/15.
//
//

#ifndef Overview_EngineMessages_hpp
#define Overview_EngineMessages_hpp

#include <ckmsg/message.hpp>
#include <functional>

namespace cinek {
    namespace ove {
    
    using MessageClientCb = std::function<void(uint32_t, ckmsg::ClassId, const ckmsg::Payload*)>;
    using MessageServerDel = std::function<void(ckmsg::ServerRequestId, const ckmsg::Payload*)>;
    
    using MessageClient = ckmsg::Client<MessageClientCb>;
    using MessageServer = ckmsg::Server<MessageServerDel>;
    
    struct MessageClientSender
    {
        MessageClient* client;
        ckmsg::Address server;
    };
    
    template<typename T>
    ckmsg::Payload makePayloadFromData(const T& data) {
        static_assert(std::is_trivial<T>(), "Payload object must be POD or trivially constructable");
        return ckmsg::Payload(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
    }
    
    }   /* namespace ove */
}   /* namespace cinek */

#endif /* EngineMessages_hpp */
