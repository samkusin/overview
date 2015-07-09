//
//  Camera.hpp
//  Overview
//
//  Created by Samir Sinha on 5/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Component_Camera_hpp
#define Overview_Entity_Component_Camera_hpp

#include "Engine/Entity/EntityTypes.hpp"

namespace cinek { namespace overview {

namespace component
{
    struct Camera
    {
        COMPONENT_DEFINITION(Camera);
        
        void init(int viewIndex, ckm::scalar fovRads,
                  ckm::scalar nearZ,
                  ckm::scalar farZ);
        
        void setDirty(bool dirty) { _dirty = dirty; }
        bool dirty() const { return _dirty; }
        
        ckm::scalar fov() const { return _fov; }
        ckm::scalar nearZ() const { return _nearZClip; }
        ckm::scalar farZ() const { return _farZClip; }
        int viewIndex() const { return _viewIndex; }
        
    private:
        ckm::scalar _fov;          // in degrees
        ckm::scalar _nearZClip;
        ckm::scalar _farZClip;
        int _viewIndex;
        bool _dirty;
    };
}

} /* namespace overview */ } /* namespace cinek */

#endif
