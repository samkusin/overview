/**
 * \file    Render/RenderDebug.hpp
 *
 * Internal debugging utilities for the Overview Engine.
 *
 * \note    Created by Samir Sinha on 7/16/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CK_Graphics_Debug_hpp
#define CK_Graphics_Debug_hpp

#include <cinek/framework/debug.hpp>

/*****************************************************************************/

/** @cond */

#ifndef RENDER_LOG_SOURCE
#define RENDER_LOG_SOURCE "render" 
#endif

#define RENDER_LOG_TRACE(...) CK_LOG_TRACE(RENDER_LOG_SOURCE, __VA_ARGS__)
#define RENDER_LOG_DEBUG(...) CK_LOG_DEBUG(RENDER_LOG_SOURCE, __VA_ARGS__)
#define RENDER_LOG_INFO(...) CK_LOG_INFO(RENDER_LOG_SOURCE, __VA_ARGS__)
#define RENDER_LOG_WARN(...) CK_LOG_WARN(RENDER_LOG_SOURCE, __VA_ARGS__)
#define RENDER_LOG_ERROR(...) CK_LOG_ERROR(RENDER_LOG_SOURCE, __VA_ARGS__)

/** @endcond */
/*****************************************************************************/

#endif
