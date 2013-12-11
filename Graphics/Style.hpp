//
//  Style.hpp
//  Graphics
//
//  Created by Samir Sinha on 11/14/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_Render_Style_hpp
#define CK_Graphics_Render_Style_hpp

#include "RenderTypes.hpp"

namespace cinekine {
    namespace glx {

    enum FillMethod
    {
        kFillMethod_NoFill,     /**< No fill is performed */
        kFillMethod_SolidFill   /**< Solid color fill */
    };

    enum LineMethod
    {
        kLineMethod_NoLine,     /**< No line is drawn */
        kLineMethod_Solid       /**< Solid line is drawn */
    };

    /**
     * @class Style
     * @brief A context describing how to draw primitives.
     */
    struct Style
    {
        RGBAColor fillColor;    /**< Fill color */
        FillMethod fillMethod;  /**< Fill method */

        RGBAColor lineColor;    /**< Line/Border color */
        LineMethod lineMethod;  /**< Line/Border draw method */

        FontHandle textFont;    /**< Text font */
        RGBAColor textColor;    /**< Text color */
        int32_t textHeight;     /**< Text scalar */
    };

    }   // namespace glx
}   // namespace cinekine

#endif
