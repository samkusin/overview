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

#ifndef Overview_Components_Rocket_SystemInterface_hpp
#define Overview_Components_Rocket_SystemInterface_hpp

#include "Rocket/Core/SystemInterface.h"

namespace cinek {
    namespace overview {

    class RocketSystemInterface : public Rocket::Core::SystemInterface
    {
    public:
        RocketSystemInterface();

        void setCurrentTime(float seconds) { _currentTime = seconds; }

        /// Get the number of seconds elapsed since the start of the application.
        /// @return Elapsed time, in seconds.
        virtual float GetElapsedTime() { return _currentTime; }

        /// Log the specified message.
        /// @param[in] type Type of log message, ERROR, WARNING, etc.
        /// @param[in] message Message to log.
        /// @return True to continue execution, false to break into the debugger.
        virtual bool LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message);

        /// Activate keyboard (for touchscreen devices)
        virtual void ActivateKeyboard();

        /// Deactivate keyboard (for touchscreen devices)
        virtual void DeactivateKeyboard();

    private:
        float _currentTime;
    };

    }   // namespace overview
}   // namespace cinek

#endif