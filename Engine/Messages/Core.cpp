//
//  EngineMessages.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/28/15.
//
//

#include "Core.hpp"

#include <ckmsg/client.inl>
#include <ckmsg/server.inl>

template class ckmsg::Client<cinek::ove::MessageClientCb>;
template class ckmsg::Server<cinek::ove::MessageServerDel>;

namespace cinek {
    namespace ove {
    }   /* namespace ove */
}   /* namespace cinek */