/* 
 *  Copyright (c) 2017 Martin McDonough.  All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 
 * - Products derived from this software may not be called "Kashyyyk", nor may
 *     "YYY" appear in their name, without prior written permission of
 *     the copyright holders.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
/*---------------------------------------------------------------------------*/

#ifndef YYY_THEME_HPP
#define YYY_THEME_HPP
#pragma once

/*---------------------------------------------------------------------------*/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

enum YYY_EnumTheme{
    eYYY_DefaultTheme,
    eYYY_DarkTheme,
    eYYY_SunsetTheme,
    eYYY_NumThemes
};

/*---------------------------------------------------------------------------*/

struct YYY_Theme {
    const char *name;
    uint8_t background_color[3];
    uint8_t background2_color[3];
    uint8_t foreground_color[3];
    const char *scheme;
};

/*---------------------------------------------------------------------------*/

extern const struct YYY_Theme *const YYY_Themes;

/*---------------------------------------------------------------------------*/

void YYY_SetTheme(enum YYY_EnumTheme);

/*---------------------------------------------------------------------------*/

void YYY_InitThemes();

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* YYY_THEME_HPP */
