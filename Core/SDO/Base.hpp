/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Cinekine Media
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
 * @file    Core/SDO/Base.hpp
 * @author  Samir Sinha
 * @date    10/20/2014
 * @brief   Queued request and result dispatch to application supplied callbacks
 * @copyright Cinekine
 */

#ifndef CK_Core_SDO_Base_hpp
#define CK_Core_SDO_Base_hpp

#include "cinek/types.hpp"

namespace cinek { namespace sdo {

class Base
{
public:
    virtual ~Base() {}
};

class Test : public Base
{
public:
    Test() :
        _x(), _y(), _z()
    {
    }
    Test(int32_t x, int32_t y, int32_t z) :
        _x(x), _y(y), _z(z)
    {
    }
    Test(Test&& other) :
        _x(other._x), _y(other._y), _z(other._z)
    {
        other._x = 0;
        other._y = 0;
        other._z = 0;
    }
    Test& operator=(Test&& other)
    {
        _x = other._x;
        _y = other._y;
        _z = other._z;
        other._x = 0;
        other._y = 0;
        other._z = 0;
        return *this;
    }
    Test(const Test& other) :
        _x(other._x), _y(other._y), _z(other._z)
    {
    }
    Test& operator=(const Test& other)
    {
        _x = other._x;
        _y = other._y;
        _z = other._z;
        return *this;
    }
    virtual ~Test() {}

    void setX(int32_t x) { _x = x; }
    int32_t getX() const { return _x; }
    void setY(int32_t y) { _y = y; }
    int32_t getY() const { return _y; }
    void setZ(int32_t z) { _z = z; }
    int32_t getZ() const { return _z; }

private:
    int32_t _x;
    int32_t _y;
    int32_t _z;
};

} /* namespace sdo */ } /* namespace cinek */

#endif
