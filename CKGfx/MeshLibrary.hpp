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
        MeshHandle acquire(unique_ptr<Mesh>&& mesh, const char* name);
        void unload(MeshHandle handle);

        //  use to retrieve texture information.
        MeshHandle handleFromName(const char* name) const;
        const Mesh* mesh(MeshHandle handle) const;

    private:
        Allocator _allocator;
        MeshHandle _nextHandle;
        
        struct Record
        {
            char* name;
            unique_ptr<Mesh> mesh;
            
            uint32_t handleIter;
            uint32_t refCnt;
            
            bool operator==(const char* name) const;
            bool matchesHandle(MeshHandle handle) const;
        };
        vector<Record> _meshes;
        vector<uint32_t> _freed;
        
        void destroy(Record& mesh);
    };

    }   // namespace gfx
}   // namespace cinek

#endif
