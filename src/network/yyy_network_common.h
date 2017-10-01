/* 
 *  Copyright (c) 2016-2017 Martin McDonough.  All rights reserved.
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

#ifndef YYY_NETWORK_COMMON_H
#define YYY_NETWORK_COMMON_H
#pragma once

#if (!defined YYY_NETWORK_HEADER_1) && (!defined YYY_NETWORK_COMMON_INTERNAL)
#error Never include this file directly, always include yyy_network.h
#endif

/*---------------------------------------------------------------------------*/

#include "yyy_network_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError {
    eYYYNetworkSuccess = 0,
    eYYYNetworkFailure,
    eYYYNetworkNotConnected,
    eYYYNetworkDisconnected,
    eYYYNetworkRefused,
    eYYYNetworkTimeout,
    eYYYNetworkAlreadyConnected,
    eYYYNetworkPermission,
    eYYYNetworkAddressUnreachable,
    eYYYNetworkBadAddress,
    eYYYNetworkPoked,
    eYYYNetworkFatal,
    eYYYNETNUMERRORS
};

/*---------------------------------------------------------------------------*/

enum YYY_NetworkBlocking {
    eYYYNetworkBlocking,
    eYYYNetworkNonBlocking
};

/*---------------------------------------------------------------------------*/

enum YYY_NetworkSocketType {
    eYYYNetworkRead = 1,
    eYYYNetworkWrite = 2,
    eYYYNetworkReadAndWrite = 3,
    eYYYNetworkError = 4
};

/*---------------------------------------------------------------------------*/

YYY_NETWORK_CONST(
    const char *YYY_ExplainError(enum YYY_NetworkError)
);

#ifdef __cplusplus
}
#endif

#endif /* YYY_NETWORK_COMMON_H */
