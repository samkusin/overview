/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
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
 */

#ifndef Overview_Components_Rocket_UIElementInstancer_hpp
#define Overview_Components_Rocket_UIElementInstancer_hpp

#include "Rocket/Core/ElementInstancer.h"

namespace cinekine {
    namespace ovengine {

    class RocketServer;

    template<typename T>
    class RocketUIElementInstancer: public Rocket::Core::ElementInstancer
    {
    public:
    	RocketUIElementInstancer(const Allocator& allocator, const RocketServer& ui);
		virtual ~RocketUIElementInstancer() = default;
	
		Rocket::Core::Element* InstanceElement(Rocket::Core::Element* parent,
										 const Rocket::Core::String& tag,
										 const Rocket::Core::XMLAttributes& attributes);

		void ReleaseElement(Rocket::Core::Element* element);

		void Release();

	private:
		const RocketServer& _ui;
		Allocator _allocator;
	};


	template<typename T>
	RocketUIElementInstancer<T>::RocketUIElementInstancer(const Allocator& allocator,
														  const RocketServer& ui) :
		_ui(ui),
		_allocator(allocator)
	{
	}

	template<typename T>
	Rocket::Core::Element* RocketUIElementInstancer<T>::InstanceElement(
									Rocket::Core::Element* /*parent*/,
									const Rocket::Core::String& tag,
									const Rocket::Core::XMLAttributes& /*attributes*/)
	{
		return _allocator.newItem<T, const Rocket::Core::String&>(tag, _ui);
	}

	template<typename T>
	void RocketUIElementInstancer<T>::ReleaseElement(Rocket::Core::Element* element)
	{
		_allocator.deleteItem(element);
	}

	template<typename T>
	void RocketUIElementInstancer<T>::Release()
	{
		_allocator.deleteItem(this);	
	}
	
	}	// namespace ovengine
}	// namespace cinekine

#endif
