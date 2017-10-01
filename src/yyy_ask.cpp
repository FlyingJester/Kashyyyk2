/* 
 *  Copyright (c) 2014-2017 Martin McDonough.  All rights reserved.
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

#include "yyy_ask.h"

#include <FL/fl_ask.H>

#include <stdio.h>

void yyy_alert(const char *msg){
    fl_alert("%s", msg);
    fputs(msg, stderr);
    fputc('\n', stderr);
#ifdef _WIN32
    fflush(stderr);
#endif
}

void yyy_alert_assert(const char *msg, const char *file, unsigned line_number){
    fl_alert("%s at %s line %i", msg, file, line_number);
    fprintf(stderr, "%s at %s line %i\n", msg, file, line_number);
#ifdef _WIN32
    fflush(stderr);
#endif
}

int yyy_choice(const char *msg, const char *b0, const char *b1, const char *b2){
    return fl_choice(msg, b0, b1, b2);
}
