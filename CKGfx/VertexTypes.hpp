//
//  VertexTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 4/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_VertexTypes_hpp
#define CK_Graphics_VertexTypes_hpp

#include "GfxTypes.hpp"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

namespace cinek {
    namespace gfx {

    /// Vertex types, acting as indices into a VertexDefinitions container
    namespace VertexTypes
    {
        enum Format
        {
            kVec3,              ///< XYZ floats only
            kVec3_RGBA,         ///< XYZ floats + ABGR byte comps
            kVec3_Normal,       ///< XYZ floats + Normals
            kVec3_Normal_RGBA,  ///< XYZ floats + Normals + ABGR byte components
            kVec3_Normal_Tex0,  ///< XYZ floats + Normals + vec2 UVs
            kVec3_RGBA_Tex0,    ///< XYZ floats + ABGR byte comps + vec2 UV
            
            kPresetCount,       ///< Preset Limit
            
            kCustomPreset0 = kPresetCount,  ///< First Custom Preset index
            kCustomPreset1,
            kCustomPreset2,
            kCustomPreset3,
            kCustomPreset4,
            kCustomPreset5,
            kCustomPreset6,
            kCustomPreset7,
            
            kFormatLimit,
            kInvalid = -1
        };
        
        /// Creates the base set of vertex definitions.  Applications may add
        /// their own custom declarations, but this function will initialize
        /// the base set of declarations.  Calling this function will also reset
        /// the global declarations array
        void initialize();
        
        /// Creates a custom preset with the specified ID. The ID must be in the
        /// range of kPresetCount to kFormatLimit-1, or otherwise this function
        /// will return nullptr.
        bgfx::VertexDecl* createCustomPreset(int formatId);
                
        /// Return a vertex declaration by format type
        const bgfx::VertexDecl& declaration(int tex);
    };
    
    }   // namespace gfx
}   // namespace cinek

#endif
