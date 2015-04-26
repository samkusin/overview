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
    
    /// Vertex types, acting as indices into a VertexDefinitions container
    namespace VertexTypes
    {
        enum
        {
            kVec3_RGBA,         ///< VertexPosColor structure
            
            kCount
        };
        
        /// XYZ position, ABGR packed color (uint8 components)
        struct PosColor
        {
            float x;
            float y;
            float z;
            uint32_t abgr;
        };
    };
    
    using VertexDefintions = vector<bgfx::VertexDecl>;
    
    /// Creates the base set of vertex definitions.  Applications may append
    /// this vector as needed
    VertexDefintions createVertexDefinitions();
    
    }   // namespace gfx
}   // namespace cinek

#endif /* defined(CK_Graphics_VertexTypes_hpp) */
