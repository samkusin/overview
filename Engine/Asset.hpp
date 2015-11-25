//
//  Asset.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/24/15.
//
//

#ifndef Overview_Asset_hpp
#define Overview_Asset_hpp

#include "EngineTypes.hpp"

#include <cinek/allocator.hpp>
#include <cinek/buffer.hpp>

namespace cinek {
    namespace ove {
  
class Asset
{
    CK_CLASS_NON_COPYABLE(Asset);
    
public:
    enum class Type
    {
        kNone,
        kJSONText,
        kImage,
        kBinary
    };
    
    Asset();
    Asset(Type t, uint32_t sz);
    ~Asset();
    Type type() const { return _type; }
    uint32_t size() const { return _size; }
    const uint8_t* data() const { return _data; }
    
    operator bool() const { return _type != Type::kNone; }
    
private:
    friend class AssetLoader;
    uint8_t* data() { return _data; }

private:
    Type _type;
    uint32_t _size;
    uint8_t* _data;
};
     
    }   /* namespace ove */
}   /* namespace cinek */


#endif /* Overview_Asset_hpp */
