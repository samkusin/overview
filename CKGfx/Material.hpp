//
//  Material.hpp
//
//  Created by Samir Sinha on 9/25/15.
//
//

#ifndef CK_Graphics_Material_hpp
#define CK_Graphics_Material_hpp

#include "GfxTypes.hpp"

namespace cinek {
    namespace gfx {
    
struct Material
{
    Color4 diffuseColor;
    TextureHandle diffuseTex;
};
    
    }   // namespace gfx
}   // namespace cinek

#endif
