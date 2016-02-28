//
//  NodeRendererTypes.hpp
//  SampleCommon
//
//  Created by Samir Sinha on 2/10/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#ifndef CKGfx_NodeRendererTypes_hpp
#define CKGfx_NodeRendererTypes_hpp

#include "CKGfx/GfxTypes.hpp"
#include <bgfx/bgfx.h>
#include <array>

namespace cinek {
    namespace gfx {

enum NodeProgramSlot
{
    kNodeProgramMeshUV,
    kNodeProgramBoneMeshUV,
    kNodeProgramFlat,
    kNodeProgramMeshColor,
    kNodeProgramBoneMeshColor,
    kNodeProgramColor,
    kNodeProgramDiffuse,
    kNodeProgramLimit           = 16,
    
    kNodeProgramNone            = -1
};

enum NodeUniformSlot
{
    //  Matrices
    kNodeUniformWorldMtx,
    kNodeUniformWorldViewProjMtx,
    
    //  Color Texture 
    kNodeUniformTexDiffuse,

    //      x = intensity, y = power
    //
    kNodeUniformMatSpecular,

    //  For the basic lighting model, light param is an amalgam of:
    //      x = ambient, y = diffuse, z = distance, w = cutoff
    //      if z == 0, then directional
    //      else
    //          if w == 0, then point (omnidirectional)
    //          else angle between light and extent of light on surface (spot)
    //
    kNodeUniformLightParam,
    //  For the basic lighting model, light coefficients for spot and point
    //
    kNodeUniformLightCoeffs,
    //  For the basic lighting model, light color is the base lamp color
    //
    kNodeUniformLightColor,
    //  For the basic lighting model, the light direction
    //
    kNodeUniformLightDir,
    //  For the basic lighting model, the origin used in spot or point lights
    //
    kNodeUniformLightOrigin,
    //  For flat shading (color)
    //
    kNodeUniformColor,
    
    kNodeUniformLimit,
    
    kNodeUniformNone = -1
};

using RenderProgramMap = std::array<bgfx::ProgramHandle, kNodeProgramLimit>;
using RenderUniformMap = std::array<bgfx::UniformHandle, kNodeUniformLimit>;

    }   // namepace gfx
}   // namespace cinek

#endif /* CKGfx_NodeRendererTypes_hpp */
