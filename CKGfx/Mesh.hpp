//
//  Mesh.hpp
//  Overview
//
//  Created by Samir Sinha on 4/27/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_Mesh_hpp
#define CK_Graphics_Mesh_hpp

#include "GfxTypes.hpp"
#include "VertexTypes.hpp"

#include <cinek/allocator.hpp>

#include <bgfx/bgfx.h>

namespace cinek {
    namespace gfx {

class Mesh
{
    CK_CLASS_NON_COPYABLE(Mesh);

public:
    Mesh();
    Mesh(VertexTypes::Format format,
         const bgfx::Memory* vertexData,
         const bgfx::Memory* indexData);
    ~Mesh();
    
    Mesh(Mesh&& other);
    Mesh& operator=(Mesh&& other);

    VertexTypes::Format format() const;

private:
    VertexTypes::Format _format;
    
    bgfx::VertexBufferHandle _vertBufH;
    bgfx::IndexBufferHandle _idxBufH;
};

/// Generates an IcoSphere mesh using the given format and number of passes.)
///
/// Note for UVs, currently uses sphere mapping to generate U and V coords
/// Future: support multiple types of texture mapping
///
unique_ptr<Mesh> createIcoSphere(float radius, int subdividePasses,
                                 VertexTypes::Format vertexType);


////////////////////////////////////////////////////////////////////////////////

inline VertexTypes::Format Mesh::format() const
{
    return _format;
}
    
    }   // namespace gfx
}   // namespace cinek

#endif /* defined(CK_Graphics_Mesh_hpp) */
