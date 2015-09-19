/*
 *  commonPriv.h
 *  common
 *
 *  Created by Samir Sinha on 9/19/15.
 *  Copyright (c) 2015 Cinekine. All rights reserved.
 *
 */

/* The classes below are not exported */
#pragma GCC visibility push(hidden)

namespace cinek {
    namespace overview {

class commonPriv
{
	public:
		void HelloWorldPriv(const char *);
};

    }
}

#pragma GCC visibility pop
