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

#define YYY_NETWORK_COMMON_INTERNAL
#include "yyy_network_common.h"

#include <assert.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------*/

static const char *yyy_network_error_messages[eYYYNETNUMERRORS] = {
    "Success",
    "General Failure",
    "Socket Not Connected",
    "Socket Disconnected",
    "Connection Refused",
    "Operation Timed Out",
    "Socket Already Connected",
    "Invalid Permission",
    "Address Not Reachable",
    "Invalid Address",
    "Poked",
    "Fatal Error"
};

/*---------------------------------------------------------------------------*/

const char *YYY_ExplainError(enum YYY_NetworkError error){
    if(error < eYYYNETNUMERRORS)
        return yyy_network_error_messages[(int)error];
    else
        return NULL;
}
