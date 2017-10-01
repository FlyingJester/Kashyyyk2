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

#ifndef KASHYYYK_YYY_ASK_H
#define KASHYYYK_YYY_ASK_H
#pragma once

/* C wrappers around FLTK functions that do not (strongly) require C++. */

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

void yyy_alert(const char *msg);

/*---------------------------------------------------------------------------*/

void yyy_alert_assert(const char *msg, const char *file, unsigned line_number);

/*---------------------------------------------------------------------------*/

int yyy_choice(const char *msg, const char *b0, const char *b1, const char *b2);

#ifdef __cplusplus
}
#endif

#endif /* KASHYYYK_YYY_ASK_H */
