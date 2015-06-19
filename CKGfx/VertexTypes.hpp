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

#include <cinek/vector.hpp>
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

namespace cinek {
    namespace gfx {
    
    /// A 4x4 uniform
    struct Matrix4
    {
        typedef float value_type;
        value_type comp[16];
        operator float*() { return comp; }
        operator const float*() const { return comp; }
    };
    
    /// A 4x1 uniform
    struct Vector4
    {
        typedef float value_type;
        value_type comp[4];
        operator float*() { return comp; }
        operator const float*() const { return comp; }
    };
    
    /// A 3x1 uniform
    struct Vector3
    {
        typedef float value_type;
        value_type comp[3];
        operator float*() { return comp; }
        operator const float*() const { return comp; }
    };
    
    struct Vector2
    {
        typedef float value_type;
        value_type comp[2];
        operator float*() { return comp; }
        operator const float*() const { return comp; }
    };
    
    /// Vertex types, acting as indices into a VertexDefinitions container
    namespace VertexTypes
    {
        enum Format
        {
            kVec3,              ///< XYZ floats only
            kVec3_RGBA,         ///< XYZ floats + ARGB byte components
            kVec3_Normal,       ///< XYZ floats + Normals
            kVec3_Normal_Tex0,  ///< XYZ floats + Normals + Vector2 UVs
            
            kPresetCount,       ///< Preset Limit
            kFormatLimit = kPresetCount + 16,
            kInvalid = -1
        };
        
        /// Creates the base set of vertex definitions.  Applications may add
        /// their own custom declarations, but this function will initialize
        /// the base set of declarations.  Calling this function will also reset
        /// the global declarations array
        void initialize();
                
        /// Return a vertex declaration by format type
        const bgfx::VertexDecl& declaration(Format tex);
    };
    
    }   // namespace gfx
}   // namespace cinek

#endif /* defined(CK_Graphics_VertexTypes_hpp) */
