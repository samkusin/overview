/**
 * \file    Common/Debug.hpp
 *
 * Internal debugging utilities for the Overview Engine.
 *
 * \note    Created by Samir Sinha on 7/16/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Overview_Debug_hpp
#define Overview_Debug_hpp

#include <cinek/framework/debug.hpp>

/*****************************************************************************/

/** @cond */

#ifndef OVENGINE_LOG_SOURCE
#define OVENGINE_LOG_SOURCE "ovengine" 
#endif

#define OVENGINE_LOG_TRACE(...) CK_LOG_TRACE(OVENGINE_LOG_SOURCE, __VA_ARGS__)
#define OVENGINE_LOG_INFO(...) CK_LOG_INFO(OVENGINE_LOG_SOURCE, __VA_ARGS__)
#define OVENGINE_LOG_WARN(...) CK_LOG_WARN(OVENGINE_LOG_SOURCE, __VA_ARGS__)
#define OVENGINE_LOG_ERROR(...) CK_LOG_ERROR(OVENGINE_LOG_SOURCE, __VA_ARGS__)

/** @endcond */
/*****************************************************************************/

#endif
