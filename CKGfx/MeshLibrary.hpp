//
//  MeshLibrary.hpp
//  Overview
//
//  Created by Samir Sinha on 5/22/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_MeshLibrary_hpp
#define CK_Graphics_MeshLibrary_hpp

#include "Mesh.hpp"

#include <cinek/vector.hpp>
#include <array>

namespace cinek {
    namespace gfx {

    class MeshLibrary
    {
        CK_CLASS_NON_COPYABLE(MeshLibrary);

    public:
        MeshLibrary() = default;
        MeshLibrary
        (
            uint16_t meshCount,
            const Allocator& allocator = Allocator()
        );
        MeshLibrary(MeshLibrary&& other);
        MeshLibrary& operator=(MeshLibrary&& other);
        
        ~MeshLibrary();
        
        //  use to build the atlas.
        MeshHandle load(const char* path);
        MeshHandle create(const char* name);
        void setMeshForLOD(MeshHandle handle, unique_ptr<Mesh>&& mesh,
                           LODIndex::Enum lod);
        void unload(MeshHandle handle);

        //  use to retrieve texture information.
        MeshHandle handleFromName(const char* name) const;
        const Mesh& mesh(MeshHandle handle, LODIndex::Enum lod) const;

    private:
        Allocator _allocator;
        MeshHandle _nextHandle;
        
        struct Record
        {
            char* name;
            std::array<unique_ptr<Mesh>, LODIndex::kCount> levels;
            std::array<int8_t, LODIndex::kCount> levelIndex;
            
            uint32_t handleIter;
            uint32_t refCnt;
            
            bool operator==(const char* name) const;
            bool matchesHandle(MeshHandle handle) const;
        };
        vector<Record> _meshes;
        vector<uint32_t> _freed;
        
        unique_ptr<Mesh> _placeholderMesh;
        
        void destroy(Record& mesh);
    };

    }   // namespace gfx
}   // namespace cinek

#endif
