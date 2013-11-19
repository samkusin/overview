//
//  GL/GLShaderLibrary.cpp
//  Graphics
//
//  Created by Samir Sinha on 11/16/2014.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "GLShaderLibrary.hpp"
#include "GLShaderLoader.hpp"
#include "Graphics/RenderDebug.hpp"
#include "Graphics/JSON.hpp"
#include "Stream/FileStreamBuf.hpp"

#include "cinek/core/ckfspath.h"

namespace cinekine {
    namespace glx {

    GLShaderLibrary::GLShaderLibrary(const char* shaderDir, const Allocator& allocator) :
        _allocator(allocator),
        _shaderRootDir(shaderDir),
        _shaderNameToHandle(std_allocator<std::pair<string, GLuint>>(allocator)),
        _programSet(std_allocator<GLuint>(allocator))
    {
    }
    
    GLShaderLibrary::~GLShaderLibrary()
    {
        for (auto program: _programSet)
        {
            glDeleteProgram(program);
        }
        for (auto &it: _shaderNameToHandle)
        {
            glDeleteShader(it.second);
        }
    }

    GLuint GLShaderLibrary::loadProgram(const char *programFilename)
    {
        char filePath[MAX_PATH];
        cinek_path_join(filePath, sizeof(filePath), _shaderRootDir.c_str(), programFilename);

        RENDER_LOG_INFO("GLShaderLibrary.loadProgram - loading program %s", programFilename);

        FileStreamBuf instream(filePath);
        if (!instream)
        {
            RENDER_LOG_ERROR("GLShaderLibrary.loadProgram - failed to load program");
            return GL_INVALID_VALUE;
        }

        RapidJsonStdStreamBuf jsonStream(instream);
        Document jsonDoc;

        jsonDoc.ParseStream<0>(jsonStream);
        
        if (jsonDoc.HasParseError() || !jsonDoc.IsObject())
        {
            RENDER_LOG_ERROR("GLShaderLibrary.loadProgram - failed to parse program");
            return GL_INVALID_VALUE;
        }
        if (!jsonDoc.HasMember("vertex") || !jsonDoc["vertex"].IsArray())
        {
            RENDER_LOG_ERROR("GLShaderLibrary.loadProgram - no vertex shaders found");
            return GL_INVALID_VALUE;
        }
        if (!jsonDoc.HasMember("fragment") || !jsonDoc["fragment"].IsArray())
        {
            RENDER_LOG_ERROR("GLShaderLibrary.loadProgram - no fragment shaders found");
            return GL_INVALID_VALUE;
        }

        const bool hasGeometryShaders = jsonDoc.HasMember("geometry") && jsonDoc["geometry"].IsArray();

        //  load and cache shaders from our program script
        GLShaderLoader shaderLoader;
        loadShaders(shaderLoader, GL_VERTEX_SHADER, jsonDoc["vertex"]);
        loadShaders(shaderLoader, GL_FRAGMENT_SHADER, jsonDoc["fragment"]);
        if (hasGeometryShaders)
        {
            loadShaders(shaderLoader, GL_GEOMETRY_SHADER, jsonDoc["geometry"]);
        } 

        //  attempt to create program using our script
        GLuint program = glCreateProgram();
        if (!program)
        {
            RENDER_LOG_ERROR("GLShaderLibrary.loadProgram - failed to create program");
            return GL_INVALID_VALUE;           
        }

        bool success = attachShaders(program, GL_VERTEX_SHADER, jsonDoc["vertex"]);
        if (hasGeometryShaders)
        {
            success = success && attachShaders(program, GL_GEOMETRY_SHADER, jsonDoc["geometry"]);
        }
        success = success && attachShaders(program, GL_FRAGMENT_SHADER, jsonDoc["fragment"]);
        if (success)
        {
            if (jsonDoc.HasMember("inputs") && jsonDoc["inputs"].IsObject())
            {
                //  link stage - fixup locations here if specified in program file
                const Value& binds = jsonDoc["inputs"];
                for (Value::ConstMemberIterator it = binds.MemberBegin(), itEnd = binds.MemberEnd();
                     it != itEnd;
                     ++it)
                {
                    const Value::Member& bind = *it;
                    const char* name = bind.name.GetString();
                    if (bind.value.IsUint())
                    {
                        glBindAttribLocation(program, bind.value.GetUint(), name);
                        RENDER_LOG_DEBUG("GLShaderLibrary.loadProgram - input %s=>%u",
                                         name, bind.value.GetUint());
                    }
                    else
                    {
                        RENDER_LOG_ERROR("GLShaderLibrary.loadProgram - input %s must be a uint",
                                         name);
                    }
                }
            }
            if (jsonDoc.HasMember("outputs") && jsonDoc["outputs"].IsObject())
            {
                //  link stage - fixup locations here if specified in program file
                const Value& binds = jsonDoc["outputs"];
                for (Value::ConstMemberIterator it = binds.MemberBegin(), itEnd = binds.MemberEnd();
                     it != itEnd;
                     ++it)
                {
                    const Value::Member& bind = *it;
                    const char* name = bind.name.GetString();
                    if (bind.value.IsUint())
                    {
                        glBindFragDataLocation(program, bind.value.GetUint(), name);
                        RENDER_LOG_DEBUG("GLShaderLibrary.loadProgram - output %s=>%u",
                                         name, bind.value.GetUint());
                    }
                    else
                    {
                        RENDER_LOG_ERROR("GLShaderLibrary.loadProgram - output %s must be a uint",
                                         name);
                    }
                }
            }
            
            glLinkProgram(program);
            if (!GLError("GLShaderLibrary.loadProgram.glLinkProgram"))
            {
                RENDER_LOG_INFO("GLShaderLibrary.loadProgram - program %s linked", programFilename);
                return program;
            }
        }


        glDeleteProgram(program);
        RENDER_LOG_ERROR("GLShaderLibrary.loadProgram - program %s build failed", programFilename);
        return 0;
    }
    
    void GLShaderLibrary::loadShaders(GLShaderLoader& loader, GLenum shaderType, const Value& shaderJSONArray)
    {
        /**
         * @todo Clean up file path generation so we don't add to the stack
         */
        char filePath[MAX_PATH];
        for (Value::ConstValueIterator it = shaderJSONArray.Begin(), itEnd = shaderJSONArray.End();
                 it != itEnd;
                 ++it)
        {
            const Value& shaderName = *it;
            if (shaderName.IsString())
            {
                auto it = _shaderNameToHandle.find(shaderName.GetString());
                if (it == _shaderNameToHandle.end())
                {
                    RENDER_LOG_INFO("GLShaderLibrary.loadShaders - loading shader %s", shaderName.GetString());
                    GLuint shader = loader.load( cinek_path_join(filePath,
                                                                 sizeof(filePath),
                                                                 _shaderRootDir.c_str(),
                                                                 shaderName.GetString()),
                                                shaderType );

                    if (shader != 0)
                    {
                        _shaderNameToHandle.insert( {shaderName.GetString(),shader} );
                    }
                    else
                    {
                        RENDER_LOG_WARN("GLShaderLibrary.loadShaders - failed to load %s", shaderName.GetString());
                    }
                }
                
            }
        }
    }

    bool GLShaderLibrary::attachShaders(GLuint program, GLenum shaderType, const Value& shaderJSONArray)
    {
        for (Value::ConstValueIterator it = shaderJSONArray.Begin(), itEnd = shaderJSONArray.End();
                 it != itEnd;
                 ++it)
        {
            const Value& shaderName = *it;
            if (shaderName.IsString())
            {
                auto it = _shaderNameToHandle.find(shaderName.GetString());
                if (it == _shaderNameToHandle.end())
                {
                    RENDER_LOG_ERROR("GLShaderLibrary.linkShaders - shader %s not found", shaderName.GetString());
                    return false;
                }
                else
                {
                    glAttachShader(program, (*it).second);
                    if (!GLError("GLShaderLibrary.linkShaders.glAttachShader"))
                    {
                        RENDER_LOG_DEBUG("GLShaderLibrary.linkShaders - attached shader %s", shaderName.GetString());
                    }
                }
            }
        }
        return true;
    }


    void GLShaderLibrary::unloadProgram(GLuint program)
    {
        if (!program)
            return;

        GLint currentProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        if (program == (GLuint)currentProgram)
        {
            RENDER_LOG_WARN("GLShaderLibrary - Unloading the current shader program");
            glUseProgram(0);
        }

        auto it = _programSet.find(program);
        if (it != _programSet.end())
        {
            glDeleteProgram(*it);
            RENDER_LOG_INFO("GLShaderLibrary - Shader %u unloaded", program);
        }
    }
    

    }   // namespace glx
}   // namespace cinekine

