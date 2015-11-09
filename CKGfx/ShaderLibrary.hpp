//
//  ShaderLibrary.hpp
//  Overview
//
//  Created by Samir Sinha on 4/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_ShaderLibrary_hpp
#define CK_Graphics_ShaderLibrary_hpp

#include "GfxTypes.hpp"

#include <unordered_map>
#include <vector>
#include <string>

#include <bgfx/bgfx.h>

namespace cinek {
    namespace gfx {

    class ShaderLibrary
    {
        CK_CLASS_NON_COPYABLE(ShaderLibrary);

    public:
        ShaderLibrary();
        ShaderLibrary(ShaderLibrary&& other);
        ShaderLibrary& operator=(ShaderLibrary&& other);
        ~ShaderLibrary();
        
        ShaderProgramId loadProgram
        (
            ShaderProgramId progId,
            const char* vertexShaderPath,
            const char* fragShaderPath
        );
        
        bgfx::ProgramHandle program(ShaderProgramId programId) const;
        
        void unloadProgram(ShaderProgramId progId);
        
    private:
        //  Loading a program will involve an O(n) search for existing
        //  shaders. Programs shouldn't be loaded during time-critical
        //  sections.
        //  Keep a ref count of shader instances in case programs reuse
        //  shaders.
        //  When freeing shaders, blank out the reference in the vector.  This
        //  should allow  ProgramReference to lookup shaders by index.
        struct ShaderReference
        {
            std::string path;
            bgfx::ShaderHandle shader = BGFX_INVALID_HANDLE;
            int refcnt = 0;
        };
        std::vector<ShaderReference, std_allocator<ShaderReference>> _shaders;
        
        //  Programs though may require somewhat frequent lookup of Shaders for
        //  informational purposes.
        struct ProgramReference
        {
            bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
            int32_t vsIndex = -1;
            int32_t fsIndex = -1;
        };
        
        std::unordered_map<ShaderProgramId, ProgramReference,
                std::hash<ShaderProgramId>,
                std::equal_to<ShaderProgramId>,
                std_allocator<std::pair<const ShaderProgramId, ProgramReference>>> _programs;
        
        int loadShader(const char* path);
        void unloadShader(int32_t index);
    };

    }   // namespace gfx
}   // namespace cinek


#endif /* defined(CK_Graphics_ShaderLibrary_hpp) */
