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
#include "Core/JsonUtilities.hpp"
#include "Core/FileStreamBuf.hpp"
#include "Core/StreamBufRapidJson.hpp"

#include "cinek/string.hpp"

namespace cinek {
    namespace glx {

    GLShaderLibrary::GLShaderLibrary(const char* shaderDir, const Allocator& allocator) :
        _allocator(allocator),
        _shaderRootDir(shaderDir),
        _shaderNameToHandle(allocator),
        _programSet(allocator)
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
        auto filePath = directoryPath({_shaderRootDir}).append(programFilename);

        RENDER_LOG_INFO("GLShaderLibrary.loadProgram - loading program %s", filePath.c_str());

        FileStreamBuf instream(filePath.c_str());
        if (!instream)
        {
            RENDER_LOG_ERROR("GLShaderLibrary.loadProgram - failed to load program");
            return GL_INVALID_VALUE;
        }

        RapidJsonStdStreamBuf jsonStream(instream);
        JsonDocument jsonDoc;

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
                const auto& binds = jsonDoc["inputs"];
                for (auto it = binds.MemberBegin(), itEnd = binds.MemberEnd();
                     it != itEnd;
                     ++it)
                {
                    const auto& bind = *it;
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
                const auto& binds = jsonDoc["outputs"];
                for (auto it = binds.MemberBegin(), itEnd = binds.MemberEnd();
                     it != itEnd;
                     ++it)
                {
                    const auto& bind = *it;
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

    void GLShaderLibrary::loadShaders(GLShaderLoader& loader, GLenum shaderType,
                                      const JsonValue& shaderJSONArray)
    {
        /**
         * @todo Clean up file path generation so we don't add to the stack
         */
        for (auto it = shaderJSONArray.Begin(), itEnd = shaderJSONArray.End();
                 it != itEnd;
                 ++it)
        {
            const auto& shaderName = *it;
            if (shaderName.IsString())
            {
                auto it = _shaderNameToHandle.find(shaderName.GetString());
                if (it == _shaderNameToHandle.end())
                {
                    auto filePath = directoryPath({_shaderRootDir}).append(shaderName.GetString());
                    RENDER_LOG_INFO("GLShaderLibrary.loadShaders - loading shader %s",
                                    shaderName.GetString());
                    GLuint shader = loader.load(filePath.c_str(),shaderType);

                    if (shader != 0)
                    {
                        _shaderNameToHandle.insert( {shaderName.GetString(),shader} );
                    }
                    else
                    {
                        RENDER_LOG_WARN("GLShaderLibrary.loadShaders - failed to load %s",
                                        shaderName.GetString());
                    }
                }

            }
        }
    }

    bool GLShaderLibrary::attachShaders(GLuint program, GLenum shaderType, const JsonValue& shaderJSONArray)
    {
        for (auto it = shaderJSONArray.Begin(), itEnd = shaderJSONArray.End();
                 it != itEnd;
                 ++it)
        {
            const auto& shaderName = *it;
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
}   // namespace cinek

