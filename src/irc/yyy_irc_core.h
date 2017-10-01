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

#ifndef YYY_IRC_CORE_H
#define YYY_IRC_CORE_H
#pragma once

/*---------------------------------------------------------------------------*/

#include "yyy_chat_message.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define YYY_IRC_CALL __fastcall
#elif defined __GNUC__
#define YYY_IRC_CALL __attribute__((fastcall))
#else
#define YYY_IRC_CALL
#endif


/*---------------------------------------------------------------------------*/

int YYY_IRC_CALL YYY_IRCParseMessage(const char *src,
    size_t len,
    struct YYY_Message *out_msg);

/*---------------------------------------------------------------------------*/

const char *YYY_IRC_CALL YYY_IRCMessageToString(const struct YYY_Message *msg,
    size_t *out_len);

#ifdef __cplusplus
}
#endif

#endif /* YYY_IRC_CORE_H */
