/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @file    GL/GLShaderLibrary.hpp
 * @author  Samir Sinha
 * @date    11/16/2013
 * @brief   Utility class for storing shader programs
 * @copyright Cinekine
 */

#ifndef CK_Graphics_GL_GLShaderLibrary_hpp
#define CK_Graphics_GL_GLShaderLibrary_hpp

#include "GLUtils.hpp"

#include "Core/JsonUtilities.hpp"

#include "cinek/allocator.hpp"
#include "cinek/string.hpp"

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
        void loadShaders(GLShaderLoader& loader, GLenum shaderType, const JsonValue& shaderJSONArray);
        bool attachShaders(GLuint program, GLenum shaderType, const JsonValue& shaderJSONArray);

        Allocator _allocator;
        std::string _shaderRootDir;
        std::unordered_map<std::string, GLuint,
            std::hash<std::string>,
            std::equal_to<std::string>,
            std_allocator<std::pair<const std::string,GLuint>> > _shaderNameToHandle;
        std::set<GLuint, std::less<GLuint>, std_allocator<GLuint>> _programSet;
    };


    }   // namespace glx
}   // namespace cinekine

#endif
