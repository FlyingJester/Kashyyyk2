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

#ifndef KASHYYYK_YYY_PREFS_H
#define KASHYYYK_YYY_PREFS_H
#pragma once

/*---------------------------------------------------------------------------*/

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

struct YYY_ServerPrefs {
    /* Any of these values may be zero or null to indicate defaults. */
    unsigned short port;
    unsigned short num_channels;
    char **channels; /* IMPL Note: This is one arena. */
    char *url, *username, *name;
};

/*---------------------------------------------------------------------------*/

struct YYY_Prefs{
    char *theme;
    
    char *default_username;
    unsigned short default_port;
    
    unsigned short num_autojoin_servers;
    char **autojoin_servers; /* IMPL Note: This is one arena. */
    
    unsigned short num_servers;
    struct YYY_ServerPrefs *servers;
};

/*---------------------------------------------------------------------------*/

bool YYY_LoadPrefs(const char *path, struct YYY_Prefs *prefs);

/*---------------------------------------------------------------------------*/

void YYY_FreePrefs(struct YYY_Prefs *prefs);

#ifdef __cplusplus
}
#endif

#endif /* KASHYYYK_YYY_PREFS_H */
