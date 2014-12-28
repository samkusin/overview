//
//  WorldTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Model_WorldTypes_hpp
#define Overview_Game_Model_WorldTypes_hpp

#include "Game/SimulationTypes.hpp"
#include "Engine/Model/AABB.hpp"
#include "LinearMath/btVector3.h"
#include "LinearMath/btMatrix3x3.h"

namespace cinek { namespace overview {
    
    const btVector3 kWorldRefDir = btVector3(0,1,0);
    const Point kObjectRefDir = Point(0,1,0);
    
    //  Utilities used for converting between coordinate systems
    inline Point toPoint(const btVector3& btv3) {
        Point ret(btv3.x(), btv3.y(), btv3.z());
        return ret;
    }

    inline btVector3 toBtVector3(const Point& vec3) {
        btVector3 ret(vec3.x, vec3.y, vec3.z);
        return ret;
    }
    
    inline btVector3 translateToBtPosition(const Point& vec3,
                                           const btVector3& halfExtents)
    {
        return btVector3(vec3.x - halfExtents.x(),
                         vec3.y - halfExtents.y(),
                         vec3.z + halfExtents.z());
    }
    
    inline btMatrix3x3 orientToBtDirection(const Point& dir)
    {
        btMatrix3x3 basis;
        basis[2] = btVector3(0,0,1);
        basis[1] = btVector3(dir.x, dir.y, dir.z);
        basis[0] = basis[2].cross(basis[1]);
        return basis;
    }
    
    class WorldObjectBase
    {
    public:
        enum class ClassId
        {
            kWorldObject,
            kWorldMap
        };
        
        ClassId classId() const { return _classId; }
        
    protected:
        WorldObjectBase(ClassId classId) : _classId(classId) {}
        ~WorldObjectBase() = default;
        
    private:
        ClassId _classId;
    };

} /* namespace overview */ } /* namespace overview */

#endif
