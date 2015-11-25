//
//  Asset.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/24/15.
//
//

#include "Asset.hpp"

#include <cinek/allocator.hpp>
#include <ckjson/jsontypes.hpp>

namespace cinek {
    namespace ove {
    
Asset::Asset() :
    _type(Type::kNone),
    _size(0),
    _data(nullptr)
{
}

Asset::Asset(Type type, uint32_t sz) :
    _type(type),
    _size(0),
    _data(nullptr)
{
    Allocator allocator;
    _data = reinterpret_cast<uint8_t*>(allocator.alloc(sz));
    if (_data) {
        _size = sz;
    }
    else {
        _type = Type::kNone;
    }
}

Asset::~Asset()
{
    Allocator allocator;
    if (_data) {
        allocator.free(_data);
        _data = nullptr;
        _size = 0;
        _type = Type::kNone;
    }
}
     
    }   /* namespace ove */
}   /* namespace cinek */

