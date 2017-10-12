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

#ifndef KASHYYYK_YYY_CHANNEL_UI_HPP
#define KASHYYYK_YYY_CHANNEL_UI_HPP
#pragma once

/*---------------------------------------------------------------------------*/

#include "ui/yyy_server_tree.hpp"

#include <stdlib.h>

/*---------------------------------------------------------------------------*/

class Fl_Valuator;

/*---------------------------------------------------------------------------*/

namespace YYY {

/*---------------------------------------------------------------------------*/

class ChatWidget;

/*---------------------------------------------------------------------------*/

class ChannelCore;

/*---------------------------------------------------------------------------*/

class ChannelUI {
    // Must not be a reference to allow us to have a default constructor so
    // that this can be held in a vector in older C++ standard compilers.
    ChannelCore *m_core;

    ServerTree::ChannelData *m_ui_data;
    
    // Message at the bottom of the screen. This is zero for the default.
    unsigned m_at;
public:
    
    ChannelUI() : m_core(NULL){}
    ChannelUI(ChannelCore &channel);
    
    /**
     * @brief Sets the ChannelCore.
     *
     * This asserts that the current core is NULL, and should only be called during initialization.
     */
    void setCore(ChannelCore &core);
    ChannelCore &getCore();
    const ChannelCore &getCore() const;

    // Allows us to check the core without actually exposing it.
    bool matches(const ChannelCore &c) const;
    void updateScroll(Fl_Valuator &to) const;
    void updateChatWidget(ChatWidget &to, const Fl_Valuator &scroll) const;
};

} // namespace YYY

#endif // KASHYYYK_YYY_CHANNEL_UI_HPP
