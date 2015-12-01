//
//  AssetManifest.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/27/15.
//
//

#include "AssetManifest.hpp"
#include "Debug.hpp"

namespace cinek {
    namespace ove {
    
AssetManifest::AssetManifest(std::string name, std::vector<uint8_t>&& source) :
    _name(std::move(name)),
    _source(std::move(source)),
    _document()
{
    if (_document.ParseInsitu<0>((char*)_source.data()).HasParseError()) {
        OVENGINE_LOG_ERROR("AssetManifest - failed to parse (%d) at %zu\n",
                           _document.GetParseError(),
                           _document.GetErrorOffset());
    }
}

const JsonValue& AssetManifest::root(const char* name) const {
    if (name) {
        auto it = _document.FindMember(name);
        CK_ASSERT(it != _document.MemberEnd());
        if (it != _document.MemberEnd()) {
            return it->value;
        }
    }
    return _document;
}
    
    }   /* namespace ove */
}   /* namespace cinek */