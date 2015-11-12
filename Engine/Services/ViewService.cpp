//
//  ViewService.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#include "ViewService.hpp"

#include "Engine/ViewController.hpp"
#include "Engine/ViewStack.hpp"

namespace cinek { namespace ove {

ViewService::ViewService(ViewStack* context, ViewController* owner) :
    _context(context)
{
}

void ViewService::load(const std::string& name)
{
    if (_context && _context->currentController() == _owner) {
        _context->load(name);
    }
}

void ViewService::unload(const std::string& name)
{
    if (_context && _context->currentController() == _owner) {
        _context->unload(name);
    }
}

void ViewService::present(const std::string& name)
{
    if (_context && _context->currentController() == _owner) {
        _context->present(name);
    }
}

void ViewService::open(const std::string& name)
{
    if (_context && _context->currentController() == _owner) {
        _context->push(name);
    }
}

void ViewService::close()
{
    if (_context && _context->currentController() == _owner) {
        _context->pop();
    }
}

} /* namespace ove */ } /* namespace cinek */