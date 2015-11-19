//
//  ViewService.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#ifndef Overview_Services_View_hpp
#define Overview_Services_View_hpp

#include "Engine/EngineTypes.hpp"

namespace cinek { namespace ove {

class ViewService
{
public:
    ViewService(ViewStack* context, ViewController* owner);
    ViewService() = default;

    void load(const std::string& name);
    void unload(const std::string& name);
    void present(const std::string& name);
    void open(const std::string& name);
    void close();

private:
    ViewStack* _context = nullptr;
    ViewController* _owner = nullptr;
};

inline ViewService::ViewService(ViewStack* context, ViewController* owner) :
    _context(context)
{
}


} /* namespace ove */ } /* namespace cinek */

#endif /* ViewService_hpp */
