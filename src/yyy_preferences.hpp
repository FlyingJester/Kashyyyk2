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

#ifndef KASHYYYK_YYY_PREFERENCES_HPP
#define KASHYYYK_YYY_PREFERENCES_HPP
#pragma once

/*---------------------------------------------------------------------------*/

#include "containers/yyy_string_arena.hpp"
#include <string>

namespace YYY {

/*---------------------------------------------------------------------------*/

class YYY_ServerPrefs {
    /* Any of these values may be zero or empty to indicate defaults. */
    unsigned short port;
    StringArena<char> channels;
    std::string url, username;
};

/*---------------------------------------------------------------------------*/

struct YYY_Prefs{
    std::string theme;
    
    std::string default_username;
    unsigned short default_port;
    
    StringArena<char> m_autojoin_servers;
    
    std::vector<YYY_ServerPrefs> servers;
};

} // namespace YYY

#endif /* KASHYYYK_YYY_PREFERENCES_HPP */
