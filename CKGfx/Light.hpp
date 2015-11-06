//
//  Light.hpp
//
//  Created by Samir Sinha on 11/3/15.
//
//

#ifndef CK_Graphics_Light_hpp
#define CK_Graphics_Light_hpp

#include "GfxTypes.hpp"

namespace cinek {
    namespace gfx {
    
enum class LightType
{
    kNone,
    kAmbient,
    kDirectional,
    kPoint,
    kSpot,
};

    
struct Light
{    
    LightType type;
    float ambientComp;              ///< ambient contribution
    float diffuseComp;              ///< diffuse contribution
    uint32_t color;                 ///< Color components (abgr)
    Vector3 coeff;                  ///< Lighting coefficients (point,spot)
    float distance;                 ///< Lighting distance (point,spot)
    float cutoff;                   ///< Lighting cutoff (spot)
};
    
    }   // namespace gfx
}   // namespace cinek

#endif
