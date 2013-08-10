//
//  prototype.hpp
//  Overview
//
//  Created by Samir Sinha on 7/7/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_prototype_hpp
#define Overview_prototype_hpp

#include "Engine/Game.hpp"

#include "cinek/cpp/ckalloc.hpp"

namespace cinekine {

class Prototype: public ovengine::Game
{
public:
    Prototype();
    ~Prototype();
    
    void update();

private:
    class Impl;
    struct ImplPtr: cinekine::unique_ptr<Impl> { ImplPtr(Impl* ptr); ~ImplPtr() {} };
    ImplPtr _impl;
};

}   // namespace cinekine

#endif
