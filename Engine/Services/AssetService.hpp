//
//  AssetService.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/22/15.
//
//

#ifndef Overview_AssetService_hpp
#define Overview_AssetService_hpp

#include "Engine/EngineTypes.hpp"

#include <functional>
#include <string>
#include <memory>

namespace cinek {
    class TaskScheduler;
}

namespace cinek {
    namespace ove {
    
struct AssetServiceContext
{
    TaskScheduler* taskScheduler;
    AssetManfiestFactory* resourceFactory;
};
    
class AssetService
{
public:
    AssetService(const AssetServiceContext& context);
    ~AssetService();
    
    void loadManifest(const std::string& name,
        std::function<void(std::shared_ptr<AssetManifest>)> resultCb);
    
private:
    AssetServiceContext _context;
    
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* AssetService_hpp */
