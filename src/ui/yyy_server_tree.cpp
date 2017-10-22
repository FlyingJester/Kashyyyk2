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

#include "yyy_server_tree.hpp"

#include "kashyyyk2.hpp"

#include <algorithm>
#include <iterator>

#include <string.h>
#include <assert.h>

#ifndef YYY_FASTCALL
#if (defined _MSC_VER) || (defined __WATCOMC__)
#define YYY_FASTCALL __fastcall
#else
#define YYY_FASTCALL
#endif
#endif // YYY_FASTCALL

#ifdef __GNUC__

#include <alloca.h>
#define YYY_HAS_ALLOCA 1

#elif (defined __WATCOMC__) || (defined _MSC_VER)

#include <malloc.h>
#define YYY_HAS_ALLOCA 1

#else
// Do someting here if alloca isn't defined!
#endif

#ifdef YYY_HAS_ALLOCA
#define YYY_ALLOCA alloca
#define YYY_ALLOCA_FREE(X) ((void)(X))
#else
#define YYY_ALLOCA malloc
#define YYY_ALLOCA_FREE free
#endif

namespace YYY {

/*---------------------------------------------------------------------------*/

Fl_Menu_Item ServerTree::s_right_click_menu[9] = {
    {"Disconnect"},
    {"Reconnect"},
    {"Join on Startup", 0, NULL, NULL, FL_MENU_INACTIVE|FL_MENU_TOGGLE},
    {"Join...", 0, NULL, NULL, FL_MENU_DIVIDER},
    {"Remove", 0, NULL, NULL, FL_MENU_INACTIVE},
    {"Properties"},
    {NULL},
    {NULL},
    {NULL}
};

#define YYY_DISCONNECT 0
#define YYY_RECONNECT 2
#define YYY_JOIN 3
#define YYY_JOIN_ON_STARTUP 4
#define YYY_PROPERTIES 5

/*---------------------------------------------------------------------------*/

ServerTree::ServerTree(int a_x, int a_y, int a_w, int a_h, const char *a_title)
  : Fl_Tree(a_x, a_y, a_w, a_h, a_title)
  , m_last_clicked(NULL)
  , m_flash_tick(false)
  , m_num_connecting(0){
    
    selectmode(FL_TREE_SELECT_SINGLE);
    showroot(0);
    
}

/*---------------------------------------------------------------------------*/
    
struct ServerTree::ServerData *ServerTree::addConnectingServer(const char *uri,
    const char *name, unsigned name_len){
    
    if(m_num_connecting++ == 0)
        Fl::add_timeout(0.5, TimeoutCallback, this);
    
    // Store the URI of the server
    Fl_Tree_Item *const item = add(root(), std::string(name, name_len).c_str());
    
    const unsigned uri_len = strlen(uri);
    ServerData *const data = (ServerData *)malloc(sizeof(ServerData *) + uri_len);
    data->arg = NULL;
    data->status = eConnecting;
    memcpy(const_cast<char*>(data->uri), uri, uri_len);
    item->user_data(data);
    
    // Refresh the view
    updateChildren();

    return data;
}

/*---------------------------------------------------------------------------*/

void ServerTree::setSelected(Fl_Tree_Item *item){
    Fl_Tree::select_only(item);
    if(item != NULL){
        ServerData *const server_data = (ServerData*)item->user_data();
        if(server_data->arg == NULL)
            YYY_SetNoServer();
        else if(IsConnected(server_data))
            YYY_SetServerConnected();
        else
            YYY_SetServerDisconnected();
    }
}

/*---------------------------------------------------------------------------*/

int ServerTree::handle(int e){

    if(children() == 0)
        YYY_SetNoServer();

    // Check that the last clicked item is still in the tree.
    bool still_exists = false;
    if(m_last_clicked != NULL){
        for(Fl_Tree_Item *item = first(); item; item = next(item)){
            if(item == m_last_clicked){
                still_exists = true;
                break;
            }
        }
    }

    if(m_last_clicked == NULL || !still_exists){
        m_last_clicked = first();
        if(m_last_clicked == root())
            m_last_clicked = next(m_last_clicked);
        select_only(m_last_clicked);
    }
    if(e == FL_PUSH && Fl::event_button() == FL_RIGHT_MOUSE){
        
        Fl_Tree_Item *const l_item = find_clicked();
        if(l_item == NULL)
            return Fl_Tree::handle(e);
        
        const unsigned l_x = Fl::event_x(), l_y = Fl::event_y();
        const Fl_Menu_Item *l_menu_item =
            s_right_click_menu->popup(l_x, l_y, NULL, 0, 0);
        if(l_menu_item == NULL)
            return Fl_Tree::handle(e);
        /*
        const unsigned l_index =
            std::distance(static_cast<const Fl_Menu_Item *>(s_right_click_menu),
                l_menu_item);
        */
        return 1;
    }
    
    // Update the m_last_clicked.
    m_last_clicked = first_selected_item();
    if(m_last_clicked == root())
        m_last_clicked = next(m_last_clicked);

    // Do the default handle
    const int ret = Fl_Tree::handle(e);

    // If we deselected everything, reselect the last item.
    if(first_selected_item() == NULL);
        select_only(m_last_clicked);
    return ret;
}

/*---------------------------------------------------------------------------*/

ServerTree::ServerData *ServerTree::getData(const std::string &server_name){
    Fl_Tree_Item *const l_root = root();
    const unsigned num = l_root->children();
    for(unsigned i = 0; i < num; i++){
        Fl_Tree_Item *const l_child = l_root->child(i);
        if(l_child->label() == server_name)
            return static_cast<ServerTree::ServerData*>(l_child->user_data());
    }
    
    return NULL;
}

/*---------------------------------------------------------------------------*/

ServerTree::ServerData *ServerTree::getData(const char *name, unsigned name_len){
    Fl_Tree_Item *const l_root = root();
    const unsigned num = l_root->children();
    for(unsigned i = 0; i < num; i++){
        Fl_Tree_Item *const l_child = l_root->child(i);
        const char *const l_label = l_child->label();
        if(strncmp(l_label, name, name_len) == 0 && l_label[name_len] == '\0')
            return static_cast<ServerTree::ServerData*>(l_child->user_data());
    }
    
    return NULL;
}

/*---------------------------------------------------------------------------*/

ServerTree::ChannelData *ServerTree::addChannel(const char *server,
    unsigned server_len,
    const char *channel,
    unsigned channel_len,
    ChannelCore *core){
    
    // Store the URI of the server
    
    char *const path = (char*)YYY_ALLOCA(server_len + 1 + channel_len + 1);
    memcpy(path, server, server_len);
    path[server_len] = '/';
    memcpy(path + server_len + 1, channel, channel_len);
    path[server_len + 1 + channel_len] = 0;

    Fl_Tree_Item *const item = add(path);

    YYY_ALLOCA_FREE(path);
    
    ChannelData *const data = new ChannelData();
    data->arg = core;
    data->status = eConnected;
    item->user_data(data);
    
    // Refresh the view
    updateChildren();

    return data;
}

/*---------------------------------------------------------------------------*/

ServerTree::ServerStatus ServerTree::getServerStatus(const std::string &server_name) const{
    const ServerTree::ServerData *const data =
        const_cast<ServerTree*>(this)->getData(server_name);
    if(data)
        return data->status;
    else
        return eFailed;
}

/*---------------------------------------------------------------------------*/

ServerTree::ServerStatus ServerTree::getServerStatus(const char *name, unsigned name_len) const{
    const ServerTree::ServerData *const data =
        const_cast<ServerTree*>(this)->getData(name, name_len);
    if(data)
        return data->status;
    else
        return eFailed;
}

/*---------------------------------------------------------------------------*/

// Note that Fl::lock should be called for multithreaded access to this.
ServerTree::ServerData *ServerTree::setServerStatus(const std::string &server_name,
    ServerTree::ServerStatus status,
    ServerCore *arg){
    
    ServerTree::ServerData *const data = getData(server_name);
    if(data){
        data->status = status;
        data->arg = arg;
    }
    return data;
}

/*---------------------------------------------------------------------------*/

ServerTree::ServerData *ServerTree::setServerStatus(const char *name,
    unsigned name_len,
    ServerTree::ServerStatus status,
    ServerCore *arg){

    ServerTree::ServerData *const data = getData(name, name_len);
    if(data){
        data->status = status;
        data->arg = arg;
    }
    return data;
}

/*---------------------------------------------------------------------------*/
// Fastcall forces flash to be on the stack, which for Watcom allows it to be
// put in the same stack slot for the entire body of updateChildren.
static void YYY_FASTCALL yyy_apply_item_color(Fl_Tree_Item *const item,
    ServerTree::ServerStatus s,
    bool flash){
    
    switch(s){
        case ServerTree::eConnected:
            item->labelcolor(FL_FOREGROUND_COLOR);
            return;
        case ServerTree::eMetaUpdate:
            item->labelcolor(FL_GREEN);
            return;
        case ServerTree::eUpdate:
            item->labelcolor(FL_BLUE);
            return;
        case ServerTree::eNotify:
            item->labelcolor(FL_RED);
            return;
        case ServerTree::eConnecting:
            item->labelcolor(flash ? FL_YELLOW : FL_DARK2);
            return;
        case ServerTree::eFailed:
            item->labelcolor(FL_DARK2);
            {
                const char *const l = item->label();
                const unsigned l_size = strlen(l);
                
                // HACK
                if(l_size && l[l_size - 1] == ')')
                    return;
                
                // Use an array to get sizeof() support
                const char suffix[] = " (OFFLINE)";
                char *const buffer = (char*)YYY_ALLOCA(l_size + sizeof(suffix));
                // No need for a NULL char yet since we have the size.
                memcpy(buffer, l, l_size);
                // We also get the NULL char from the end of suffix with this.
                memcpy(buffer + l_size, suffix, sizeof(suffix));
                item->label(buffer);
                YYY_ALLOCA_FREE(buffer);
            }
            return;
    }
    return;
}

/*---------------------------------------------------------------------------*/

void ServerTree::updateChildren(){
    
    assert(root() != NULL);
    Fl_Tree_Item &l_root = *root();
    const unsigned num_servers = l_root.children();
    
    for(unsigned i = 0; i < num_servers; i++){
        Fl_Tree_Item *const l_server = l_root.child(i);
        const unsigned num_channels = l_server->children();
        
        ServerTree::ServerData *const l_data =
            static_cast<ServerTree::ServerData *>(l_server->user_data());
        
        int s = (int)l_data->status;
        
        if(s <= (int)eChannelEnd){
            s = 0;
            for(unsigned e = 0; e < num_channels; e++){
                Fl_Tree_Item *const l_channel = l_server->child(e);
                assert(l_channel->children() == 0);
                const int other_s =
                    static_cast<ChannelData*>(l_channel->user_data())->status;
                assert(other_s <= (int)eChannelEnd);
                if(other_s > s)
                    s = other_s;
                yyy_apply_item_color(l_channel, (ServerTree::ServerStatus)other_s, false);
            }
        }
        l_data->status = (ServerTree::ServerStatus)s;
        yyy_apply_item_color(l_server, (ServerTree::ServerStatus)s, m_flash_tick);
    }
}

/*---------------------------------------------------------------------------*/

bool ServerTree::isSelected(const char *server_name, size_t server_len,
    const char *channel_name, size_t channel_len) const{
    
    assert(server_len > 0);
    
    const size_t buffer_size = server_len + channel_len + 2;
    const bool use_alloca = buffer_size < 0xFF;

    char *const buffer = (char *)(use_alloca ? YYY_ALLOCA(buffer_size) : malloc(buffer_size));

    memcpy(buffer, server_name, server_len);
    
    if(channel_len == 0){
        buffer[server_len] = '\0';
    }
    else{
        buffer[server_len] = '/';
        memcpy(buffer + server_len + 1, channel_name, channel_len);
        buffer[server_len + channel_len + 1] = '\0';
    }

    // Not sure why Fl_Tree::is_selected(const char *) is not a const method...
    const bool ret = const_cast<ServerTree*>(this)->Fl_Tree::is_selected((const char *)buffer);

    if(use_alloca){
        YYY_ALLOCA_FREE(buffer);
    }
    else{
        free(buffer);
    }

    return ret;
}

/*---------------------------------------------------------------------------*/

bool ServerTree::isSelected(const std::string &server_name, const std::string& channel_name) const{
    return isSelected(server_name.c_str(), server_name.length(),
        channel_name.c_str(), channel_name.length());
}

/*---------------------------------------------------------------------------*/

bool ServerTree::isSelected(const char *server_name, size_t server_len) const{

    // WTF? Why are first_selected_item and root() not const!?
    Fl_Tree &mutable_this = *const_cast<ServerTree*>(this);
    Fl_Tree_Item *const selected = mutable_this.first_selected_item();
    const Fl_Tree_Item *const l_root = mutable_this.root();
    if(selected == NULL)
        return false;

    // Get server, or if it is a channel, the server of the channel.
    Fl_Tree_Item *const server_item =
        (selected->parent() == l_root) ? selected : selected->parent();
    const char *label = server_item->label();
    if(*label == '/')
        label++;
    const size_t label_size =
#ifndef __WATCOMC__
        strnlen(label, server_len+1);
#else
        strlen(label);
#endif
    if(label_size != server_len)
        return false;
    else
        return memcmp(server_name, label, server_len) == 0;
}

} // namespace YYY
