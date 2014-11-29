//
//  CommandResponse.hpp
//  Overview
//
//  Created by Samir Sinha on 11/28/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_CommandResponse_hpp
#define Overview_Game_CommandResponse_hpp

#include "Core/SDO/SDO.hpp"

namespace cinek { namespace overview {

class CommandResponse : public SDO
{
public:
    CINEK_SDO_CLASS(CommandResponse)

    enum Code
    {
        kSuccess,
        kFailure,
        kInvalidParameter
    };

    CommandResponse() = default;

    void setResponseCode(Code code) { _code = code; }
    Code responseCode() const { return _code; }
    
private:
    Code _code;
};

} /* namespace overview */ } /* namespace cinek */

#endif
