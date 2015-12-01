//
//  AssetManifest.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/27/15.
//
//

#ifndef Overview_AssetManifest_hpp
#define Overview_AssetManifest_hpp

#include "EngineTypes.hpp"

#include <cinek/allocator.hpp>
#include <ckjson/json.hpp>

#include <vector>
#include <string>

namespace cinek {
    namespace ove {
    
class AssetManifest
{
public:
    AssetManifest(std::string name, std::vector<uint8_t>&& source);
    
    const JsonValue& root(const char* name=nullptr) const;
    
    const std::string name() const { return _name; }
    
private:
    std::string _name;
    std::vector<uint8_t> _source;
    JsonDocument _document;
};
    
    }   /* namespace ove */
}   /* namespace cinek */

#endif /* Overview_AssetManifest_hpp */
