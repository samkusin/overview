//
//  GL/GLShaderLibrary.hpp
//  Graphics
//
//  Created by Samir Sinha on 11/16/2013.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_GL_GLShaderLibrary_hpp
#define CK_Graphics_GL_GLShaderLibrary_hpp

#include "GLUtils.hpp"
#include "Graphics/JSON.hpp"

#include "cinek/cpp/ckalloc.hpp"
#include "cinek/cpp/ckstring.hpp"

#include <unordered_map>
#include <set>

namespace cinekine {
    namespace glx {

    class GLShaderLoader;

    /**
     * @class GLShaderLibrary
     * @brief An OpenGL Shader loader and housekeeper
     */    
    class GLShaderLibrary
    {
    public:    
        GLShaderLibrary(const char* shaderDir, const Allocator& allocator);
        virtual ~GLShaderLibrary();
        
        /**
         * Loads a GL shader program using the supplied JSON resource.
         * The shader sources are loaded and linked using the description JSON
         * @param  programFilename  The resource file path
         * @return                  The GL program resource
         */
        GLuint loadProgram(const char *programFilename);
        /**
         * Unloads a GL shader program resource
         * @param program Program resource
         */
        void unloadProgram(GLuint program);

    private:
        void loadShaders(GLShaderLoader& loader, GLenum shaderType, const Value& shaderJSONArray);
        bool attachShaders(GLuint program, GLenum shaderType, const Value& shaderJSONArray);

        Allocator _allocator;
        string _shaderRootDir;
        std::unordered_map<string, GLuint,
            std::hash<string>,
            std::equal_to<string>,
            std_allocator<std::pair<string, GLuint>> > _shaderNameToHandle;  
        std::set<GLuint, std::less<GLuint>, std_allocator<GLuint>> _programSet;
    };
    

    }   // namespace glx
}   // namespace cinekine

#endif
