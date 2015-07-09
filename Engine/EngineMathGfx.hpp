//
//  EngineMathGfx.hpp
//  Overview
//
//  Created by Samir Sinha on 7/9/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Engine_Math_Gfx_hpp
#define Overview_Engine_Math_Gfx_hpp

#include "EngineMathTypes.hpp"

namespace ckm {

template<typename _SrcType, typename _DestType>
_DestType& convert(_SrcType const& src, _DestType& dest);

}

#endif /* defined(Overview_Engine_Math_Gfx_hpp) */
