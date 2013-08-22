//
//  Controller.hpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Controller_hpp
#define Overview_Controller_hpp

#include "cinek/cpp/ckalloc.hpp"

namespace cinekine {    
    namespace ovengine {
    
    class SpriteDatabaseLoader;

    //  The Controller drives execution of the Application.
    class Controller
    {
        CK_CLASS_NON_COPYABLE(Controller);

    public:
        Controller(const Allocator& allocator);
        ~Controller();
        
        SpriteDatabaseLoader& getSpriteDatabaseLoader();

    private:
        Allocator _allocator;

        class Impl;
        unique_ptr<Impl> _impl;
    };
        
        
    }   // namespace ovengine
}   // namespace cinekine

#endif
