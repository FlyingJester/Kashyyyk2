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

#ifndef YYY_NETWORK_TLS_H
#define YYY_NETWORK_TLS_H
#pragma once

#if (!defined YYY_TLS_INTERNAL)
#error User applications should NOT include this file.
#endif

#include <stddef.h>

struct YYY_NetworkSocket;

#ifdef __cplusplus
extern "C" {
#endif

void YYY_TLSBeginConnection(struct YYY_NetworkSocket *sock);
void YYY_TLSEndConnection(struct YYY_NetworkSocket *sock);

void *YYY_TLSDecode(struct YYY_NetworkSocket *sock, void *data, size_t size, size_t *out_size);
void YYY_TLSFreeDecode(struct YYY_NetworkSocket *sock, void *decoded);

const void *YYY_TLSEncode(struct YYY_NetworkSocket *sock,
    const void *data,
    size_t size,
    size_t *out_size);
void YYY_TLSFreeEncode(struct YYY_NetworkSocket *sock, void *decoded);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* YYY_NETWORK_TLS_H */
