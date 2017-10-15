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
/*---------------------------------------------------------------------------*/

#ifndef KASHYYYK_YYY_BUFFER_H
#define KASHYYYK_YYY_BUFFER_H
#pragma once

/*
 * An IRC message is always terminated by a /r/n, and may be up to 255 chars
 * long, although we accept up to 640, since some networks use up to 257 and if
 * 257 is fine, we should really pick something unambiguously too big to be
 * acceptable by the standard. The reason we need more than, say, 300 is that
 * it might be possible to have all but the last byte of a message, and then
 * receive 257-ish more bytes. We need more than 500 to handle this.
 *
 * Since BSD-style socket libraries (including Windows implementations) do not
 * have a proper operation to read up to a certain pattern, and there are no
 * guarantees that the IRC server will write exactly an entire message at a
 * time (or that our socket library will have an entire message ready), we must
 * buffer any partial messages that are encountered.
 */

#include "utils/yyy_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define YYY_MAX_MSG_LEN 312

/*---------------------------------------------------------------------------*/

struct YYY_MSGBuffer;

/*---------------------------------------------------------------------------*/

unsigned YYY_MSGBufferSize();

/*---------------------------------------------------------------------------*/

void YYY_InitMSGBuffer(struct YYY_MSGBuffer *buffer);

/*---------------------------------------------------------------------------*/

void YYY_DestroyMSGBuffer(struct YYY_MSGBuffer *buffer);

/*---------------------------------------------------------------------------*/
/* Adds a message or message fragment to the buffer. Always retrieve a message
 * if any are waiting using YYY_GetMSGBuffer before placing another message.
 * message should be no larger than YYY_MAX_MSG_LEN
 */
void YYY_PutMSGBuffer(struct YYY_MSGBuffer *YYY_RESTRICT buffer,
    const void * YYY_RESTRICT message,
    unsigned message_len);

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieves a message from a buffer, or zero if there are no messages.
 *
 * Output will be null-terminated. @p output must be at least YYY_MAX_MSG_LEN
 * bytes long.
 *
 * @returns Number of characters written.
 */
unsigned YYY_GetMSGBuffer(struct YYY_MSGBuffer *buffer,
    char output[YYY_MAX_MSG_LEN]);

#ifdef __cplusplus
}
#endif

#endif /* KASHYYYK_YYY_BUFFER_H */
