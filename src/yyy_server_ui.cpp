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

#include "yyy_server_ui.hpp"

#include "yyy_server_core.hpp"

#ifdef __WATCOMC__
// Silences warnings about unary operator
#include <FL/Fl_Image.H>
#endif

#include "utils/yyy_alloca.h"

#include <FL/Fl_Tree_Item.H>
#include <FL/Fl_Tree.H>

#include <assert.h>

/*---------------------------------------------------------------------------*/

namespace YYY {

/*---------------------------------------------------------------------------*/

ServerUI::ServerUI(ServerCore &core)
  : m_core(core)
  , m_ui_data(NULL)
  , m_channel(core.serverChannel()){
    
}

/*---------------------------------------------------------------------------*/

void ServerUI::setupNewChannel(ChannelCore &core, ChannelUI &ui) const {
    core.setUI(ui);
    ui.setCore(core);
    const std::string &server_name = m_core.name(), &channel_name = core.name();
    ServerTree::ChannelData *const data =
        server_tree->addChannel(server_name, channel_name, &core);
    ui.setData(data);
}

/*---------------------------------------------------------------------------*/

void ServerUI::setUIData(ServerTree::ServerData *ui_data){
    assert(m_ui_data == NULL);
    assert(ui_data != NULL);
    m_ui_data = ui_data;
}

/*---------------------------------------------------------------------------*/

ChannelUI &ServerUI::addChannel(const char *name, unsigned len){
    ChannelCore &core = m_core.addChannel(name, len);
    ChannelUI &ui = m_channels.create();
    setupNewChannel(core, ui);
    return ui;
}

/*---------------------------------------------------------------------------*/

ChannelUI &ServerUI::addChannel(const std::string &name){
    ChannelCore &core = m_core.addChannel(name);
    ChannelUI &ui = m_channels.create();
    setupNewChannel(core, ui);
    return ui;
}

} // namespace YYY
