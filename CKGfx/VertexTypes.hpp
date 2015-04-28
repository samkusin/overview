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

namespace cinek {
    namespace gfx {
    
    /// A 4x4 uniform
    struct Matrix4
    {
        float comp[16];
    };
    
    /// A 4x1 uniform
    struct Vector4
    {
        float comp[4];
    };
    
    
    /// Vertex types, acting as indices into a VertexDefinitions container
    namespace VertexTypes
    {
        enum Format
        {
            kVec3,              ///< Pos structure
            kVec3_RGBA,         ///< PosColor structure
            
            kPresetCount,
            
            kFormatLimit = kPresetCount + 16
        };
        
        /// XYZ position
        struct Pos
        {
            float x;
            float y;
            float z;
        };
        
        /// ABGR color
        struct Color
        {
            float a;
            float b;
            float g;
            float r;
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
