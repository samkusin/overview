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
        Vector4& from(value_type x, value_type y, value_type z, value_type w) {
            comp[0] = x;
            comp[1] = y;
            comp[2] = z;
            comp[3] = w;
            return *this;
        }
        Vector4& fromABGR(uint32_t abgr) {
            comp[3] = (abgr >> 24)/255.0f;
            comp[2] = ((abgr >> 16) & 0xff)/255.0f;
            comp[1] = ((abgr >> 8) & 0xff)/255.0f;
            comp[0] = (abgr & 0xff)/255.0f;
            return *this;
        }
        
        uint32_t toABGR() const {
            return ((uint32_t)(comp[3]*255.0f) << 24) |
                   ((uint32_t)(comp[2]*255.0f) << 16) |
                   ((uint32_t)(comp[1]*255.0f) << 8) |
                   ((uint32_t)(comp[0]*255.0f));
        }
        value_type x() const { return comp[0]; }
        value_type y() const { return comp[1]; }
        value_type z() const { return comp[2]; }
        value_type w() const { return comp[3]; }
    };
    
    /// A 3x1 uniform
    struct Vector3
    {
        typedef float value_type;
        value_type comp[3];
        operator float*() { return comp; }
        operator const float*() const { return comp; }
        Vector3& from(value_type x, value_type y, value_type z) {
            comp[0] = x;
            comp[1] = y;
            comp[2] = z;
            return *this;
        }
        value_type x() const { return comp[0]; }
        value_type y() const { return comp[1]; }
        value_type z() const { return comp[2]; }
    };
    
    struct Vector2
    {
        typedef float value_type;
        value_type comp[2];
        operator float*() { return comp; }
        operator const float*() const { return comp; }
        Vector2& from(value_type x, value_type y) {
            comp[0] = x;
            comp[1] = y;
            return *this;
        }
        value_type x() const { return comp[0]; }
        value_type y() const { return comp[1]; }
    };
    
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

#endif /* defined(CK_Graphics_VertexTypes_hpp) */
