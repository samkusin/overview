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
 */

#include "RocketFileInterface.hpp"

#include "SDL2/SDL_rwops.h"

namespace cinek {
    namespace overview {

    RocketFileInterface::RocketFileInterface(const char* rootdir) :
        _rootDir(rootdir ? rootdir : "")
    {

    }

    Rocket::Core::FileHandle RocketFileInterface::Open(const Rocket::Core::String& path)
    {
        Rocket::Core::String pathname = _rootDir + path;
        return reinterpret_cast<Rocket::Core::FileHandle>(SDL_RWFromFile(pathname.CString(), "rb"));
    }

    void RocketFileInterface::Close(Rocket::Core::FileHandle file)
    {
        SDL_RWops* rwops = reinterpret_cast<SDL_RWops*>(file);
        SDL_RWclose(rwops);
    }

    size_t RocketFileInterface::Read(void* buffer, size_t size, Rocket::Core::FileHandle file)
    {
        SDL_RWops* rwops = reinterpret_cast<SDL_RWops*>(file);
        return SDL_RWread(rwops, buffer, 1, size);
    }

    bool RocketFileInterface::Seek(Rocket::Core::FileHandle file, long offset, int origin)
    {
        SDL_RWops* rwops = reinterpret_cast<SDL_RWops*>(file);
        return SDL_RWseek(rwops, offset, origin) >= 0;
    }

    size_t RocketFileInterface::Tell(Rocket::Core::FileHandle file)
    {
        SDL_RWops* rwops = reinterpret_cast<SDL_RWops*>(file);
        return (size_t)SDL_RWtell(rwops);
    }

    }   // namespace overview
}   // namespace cinek