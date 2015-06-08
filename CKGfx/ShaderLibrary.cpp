//
//  ShaderLibrary.cpp
//  Overview
//
//  Created by Samir Sinha on 4/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "ShaderLibrary.hpp"

#include <cinek/file.hpp>
#include <cinek/debug.hpp>

namespace cinek {
    namespace gfx {

ShaderLibrary::ShaderLibrary(const Allocator& allocator) :
    _shaders(allocator),
    _programs(allocator)
{
}

ShaderLibrary::ShaderLibrary(ShaderLibrary&& other) :
    _shaders(std::move(other._shaders)),
    _programs(std::move(other._programs))
{
}

ShaderLibrary& ShaderLibrary::operator=(ShaderLibrary&& other)
{
    _shaders = std::move(other._shaders);
    _programs = std::move(other._programs);
    return *this;
}

ShaderLibrary::~ShaderLibrary()
{
    //  cleanup programs and shaders
    for (auto& program : _programs)
    {
        auto& uniforms = program.second.uniforms;
        for (auto& uniform : uniforms)
        {
            if (bgfx::isValid(uniform))
            {
                bgfx::destroyUniform(uniform);
            }
        }
        if (bgfx::isValid(program.second.program))
        {
            bgfx::destroyProgram(program.second.program);
        }
    }
    
    for (auto& shader : _shaders)
    {
        if (bgfx::isValid(shader.shader))
        {
            bgfx::destroyShader(shader.shader);
        }
    }
}
        
bgfx::ProgramHandle ShaderLibrary::loadProgram
(
    uint32_t progId,
    const char* vertexShaderPath,
    const char* fragShaderPath,
    vector<bgfx::UniformHandle>&& uniforms
)
{
    int vsIdx = loadShader(vertexShaderPath);
    int fsIdx = loadShader(fragShaderPath);
    
    if (vsIdx == -1)
        return BGFX_INVALID_HANDLE;
    if (fsIdx == -1)
        return BGFX_INVALID_HANDLE;

    bgfx::ProgramHandle prog = bgfx::createProgram(
        _shaders[vsIdx].shader,
        _shaders[fsIdx].shader
    );
    if (!bgfx::isValid(prog))
        return BGFX_INVALID_HANDLE;
    
    ProgramReference ref;
    ref.program = prog;
    ref.vsIndex = vsIdx;
    ref.fsIndex = fsIdx;
    ref.uniforms = std::move(uniforms);
    _programs.emplace(progId, ref);
    return prog;
}

int ShaderLibrary::loadShader(const char* path)
{
    bgfx::ShaderHandle handle = BGFX_INVALID_HANDLE;
    int shaderIndex = -1;
    for (auto it = _shaders.begin(); it != _shaders.end(); ++it)
    {
        auto& shader = *it;
        if (bgfx::isValid(shader.shader))
        {
            if (shader.path == path)
            {
                handle = shader.shader;
                shaderIndex = (int)(it - _shaders.begin());
                break;
            }
        }
        else if (shaderIndex < 0)
        {
            shaderIndex = (int)(it - _shaders.begin());
        }
    }
    if (!bgfx::isValid(handle))
    {
        auto file = file::open(path);
        if (file)
        {
            uint32_t sz = (uint32_t)file::size(file);
            const bgfx::Memory* mem = bgfx::alloc(sz+1);
            if (mem)
            {
                file::read(file, mem->data, sz);
                mem->data[mem->size-1] = '\0';
                handle = bgfx::createShader(mem);
            }
            file::close(file);
        }
    }
    if (!bgfx::isValid(handle))
        return BGFX_INVALID_HANDLE;
    
    if (shaderIndex < 0)
    {
        shaderIndex = (int)_shaders.size();
        _shaders.emplace_back();
        if (shaderIndex >= _shaders.size())
        {
            CK_ASSERT(false);   // leaking resource on bad alloc
            return BGFX_INVALID_HANDLE;
        }
        _shaders[shaderIndex].path = path;
        _shaders[shaderIndex].shader = handle;
        _shaders[shaderIndex].refcnt = 0;
    }
    
    ++_shaders[shaderIndex].refcnt;

    return shaderIndex;
}

bgfx::ProgramHandle ShaderLibrary::program(uint32_t programId) const
{
    auto itProgram = _programs.find(programId);
    if (itProgram == _programs.end())
        return BGFX_INVALID_HANDLE;
    
    return (*itProgram).second.program;
}

bgfx::UniformHandle ShaderLibrary::uniformFromProgram
(
    uint32_t uniformIndex,
    uint32_t progId
) const
{
    auto itProgram = _programs.find(progId);
    if (itProgram == _programs.end())
        return BGFX_INVALID_HANDLE;
    
    auto& uniforms = itProgram->second.uniforms;
    if (uniformIndex >= uniforms.size())
        return BGFX_INVALID_HANDLE;
    
    return uniforms[uniformIndex];
}
        
    }   // namespace gfx
}   // namespace cinek
