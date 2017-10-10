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

#include "yyy_theme.h"
#include "kashyyyk2.hpp"

#include <assert.h>

#include "FL/Fl.H"
#include "FL/fl_draw.H"

/*---------------------------------------------------------------------------*/

static struct YYY_Theme yyy_static_themes[(unsigned)eYYY_NumThemes] = {
    {"Light", {0,0,0}, {0,0,0}, {0,0,0}, "gtk+"},
    {"Dark", {32, 32, 32}, {64, 64, 64}, {224, 224, 224}, "gtk+"},
    {"Sunset", {32, 32, 32}, {128, 64, 32}, {224, 224, 224}, "gleam"}
};

/*---------------------------------------------------------------------------*/

static inline void fl_color_to_array(const uint32_t in, uint8_t out[3]){
    out[0] = (in >> 24) & 0xFF;
    out[1] = (in >> 16) & 0xFF;
    out[2] = (in >> 8) & 0xFF;

    // FLTK docs say this should be true.
    assert((in & 0xFF) == 0);
}

/*---------------------------------------------------------------------------*/

const struct YYY_Theme *const YYY_Themes = yyy_static_themes;

/*---------------------------------------------------------------------------*/

void YYY_InitThemes(){
#ifdef __WATCOMC__
    // Watcom 1.3 does't seem to set this properly :S
    (const YYY_Theme*)YYY_Themes = yyy_static_themes;
#endif
    const uint32_t default_background = fl_lighter(Fl::get_color(FL_BACKGROUND_COLOR)),
        default_background2 = Fl::get_color(FL_BACKGROUND2_COLOR),
        default_foreground = Fl::get_color(FL_FOREGROUND_COLOR);
    // Macro to save us some copypasta
#define SET_DEFAULT_COLOR(WHAT) fl_color_to_array(default_ ## WHAT, yyy_static_themes[0].WHAT ## _color)
    
    SET_DEFAULT_COLOR(background);
    SET_DEFAULT_COLOR(background2);
    SET_DEFAULT_COLOR(foreground);
}

/*---------------------------------------------------------------------------*/

void YYY_SetTheme(YYY_EnumTheme theme_index){
    assert((unsigned)theme_index < (unsigned)eYYY_NumThemes);
    
    const YYY_Theme &theme = yyy_static_themes[(unsigned)theme_index];

    // Macro to save us some copypasta
#define SET_COLOR(WHAT) Fl::WHAT(theme.WHAT ## _color[0], theme.WHAT ## _color[1], theme.WHAT ## _color[2])
    
    SET_COLOR(background);
    SET_COLOR(background2);
    SET_COLOR(foreground);

    Fl::scheme(theme.scheme);

    for(unsigned i = 0; i < YYY_GetNumWindows(); i++)
        YYY_GetWindow(i)->redraw();
}
