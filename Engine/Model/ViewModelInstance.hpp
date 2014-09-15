///
/// @file
/// Class definition for the ViewModelInstance
///
/// @author     Samir Sinha
/// @date       09/14/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_ViewModelInstance_hpp
#define Overview_Model_ViewModelInstance_hpp

#include "Engine/Model/ModelTypes.hpp"

namespace cinekine { namespace ovengine {

struct ViewModelInstance
{
    enum ClassId
    {
        kNull,
        kTile,
        kSprite
    };
    ClassId classId;
    union
    {
        SpriteInstanceId spriteInstanceId;
        TileHandle tileHandle;
    };

    ViewModelInstance(): classId(kNull) {}
    ViewModelInstance(SpriteInstanceId id) : classId(kSprite), spriteInstanceId(id) {}
    ViewModelInstance(const TileHandle& handle) : classId(kTile), tileHandle(handle) {}
};

} /* namespace ovengine */ } /* namespace cinekine */

#endif
