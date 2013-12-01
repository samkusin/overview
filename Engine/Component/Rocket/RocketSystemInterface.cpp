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

#include "RocketSystemInterface.hpp"
#include "Engine/Debug.hpp"

namespace cinekine {
    namespace ovengine {

    RocketSystemInterface::RocketSystemInterface() :
        _currentTime(0.0f)
    {
    }

    bool RocketSystemInterface::LogMessage(Rocket::Core::Log::Type type, 
                                           const Rocket::Core::String& message)
    {
        switch (type)
        {
        case Rocket::Core::Log::LT_DEBUG:
            OVENGINE_LOG_TRACE(message.CString());
            break;
        case Rocket::Core::Log::LT_ALWAYS:
        case Rocket::Core::Log::LT_INFO:
            OVENGINE_LOG_INFO(message.CString());
            break;
        case Rocket::Core::Log::LT_WARNING:
             OVENGINE_LOG_WARN(message.CString());
             break;       
        case Rocket::Core::Log::LT_ERROR:
            OVENGINE_LOG_ERROR(message.CString());
            break;
        case Rocket::Core::Log::LT_ASSERT:
            OVENGINE_LOG_ERROR(message.CString());
            return false;
        default:
            break;
        }
        return true;
    }

    void RocketSystemInterface::ActivateKeyboard()
    {

    }
        
    void RocketSystemInterface::DeactivateKeyboard()
    {

    }

    }   // namespace ovengine
}   // namespace cinekine
