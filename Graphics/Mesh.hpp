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
 * @file    Mesh.hpp
 * @author  Samir Sinha
 * @date    11/29/2013
 * @brief   Abstract inteface for renderer specific mesh objects
 * @copyright Cinekine
 */

#ifndef CK_Graphics_Mesh_hpp
#define CK_Graphics_Mesh_hpp


#include "cinek/rendermodel/types.h"
#include <memory>

namespace cinekine {
    namespace glx {
        
    /**
     * @class Mesh
     * @brief Abstract interface for renderer specific mesh objects
     */
    class Mesh
    {
    public:
        /** Describes how the mesh is drawn by the renderer. */
        enum Type
        {
            kTriangles,             /**< Render as a list of triangles */
            kTriangleFan            /**< Render as a 'fan' of triangles */
        };
    public:
        virtual ~Mesh() {}
        /** @return Checks whether a Mesh was created successfully */ 
        virtual operator bool() const = 0;
    };

    /** A Mesh managed pointer */
    typedef std::shared_ptr<Mesh> MeshPtr;
    
    }   // namespace glx
}   // namespace cinekine

#endif