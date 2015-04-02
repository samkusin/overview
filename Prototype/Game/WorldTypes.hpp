//
//  WorldTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Model_WorldTypes_hpp
#define Overview_Game_Model_WorldTypes_hpp

#include "Shared/GameTypes.hpp"
#include "Engine/Model/AABB.hpp"
#include "LinearMath/btVector3.h"
#include "LinearMath/btMatrix3x3.h"

namespace cinek { namespace overview {
    
    const btVector3 kWorldRefDir = btVector3(0,-1,0);
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
        basis[2] = btVector3(0,0,1);                    // Z
        basis[1] = btVector3(dir.x, -dir.y, -dir.z);   // Y
        basis[0] = basis[1].cross(basis[2]);            // X
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
        
        enum
        {
            kFlag_Transformed           = (1 << 0)
        };
        
        ClassId classId() const { return _classId; }
        void* context() { return _context; }
        
        void setUserFlags(uint32_t flag) { _userFlags |= flag; }
        void clearUserFlags(uint32_t flag=0xffffffff) { _userFlags &= (~flag); }
        uint32_t userFlags() const { return _userFlags; }
        bool testUserFlag(uint32_t flag) const { return (_userFlags & flag) != 0; }
        
    protected:
        WorldObjectBase(ClassId classId, void* context) :
            _classId(classId), _userFlags(0), _context(context) {}
        ~WorldObjectBase() = default;
        
    private:
        ClassId _classId;
        uint32_t _userFlags;
        void* _context;
    };


} /* namespace overview */ } /* namespace overview */

#endif
