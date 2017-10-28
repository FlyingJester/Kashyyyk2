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
/** 
 * @file yyy_server_tree.hpp
 * @brief Tree Widget to display Server and Channel names
 * @author Martin McDonough
 * @date 2017
 */

#ifndef YYY_SERVER_TREE_HPP
#define YYY_SERVER_TREE_HPP
#pragma once

#include <FL/Fl_Image.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Menu_Item.H>

/*---------------------------------------------------------------------------*/

#ifdef __WATCOMC__
#undef max
#undef min
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4200) // Flexible array element.
#endif

/*---------------------------------------------------------------------------*/

#include <vector>
#include <string>

#include <cassert>

namespace YYY {

/*---------------------------------------------------------------------------*/

class ServerController;

/*---------------------------------------------------------------------------*/

class ChannelController;

/**
 * @brief Tree Widget used to sort a two-level hierarchy with a status for each
 * item.
 *
 * The ServerTree is the same as an Fl_Tree, but with an added right-click
 * menu and some values set by default. It also allows for an item in the tree
 * to be connecting (ie, name is flashing), connected (name is normal), failed
 * (name is highlighted red), meta_updated (activity not including messages has
 * occurred), update (messages have been received), or pending (direct message
 * has been recieved, or you've been mentioned).
 * This widget should be independant of the rest of Kashyyyk.
 *
 * @note Currently the ServerTree MUST be singular. To make it work otherwise
 * we would need to add checks before removing the timeout callback, or just
 * not remove it if more than one ServerTree exists.
 */
class ServerTree : public Fl_Tree {
public:
    
    enum ServerStatus {
        eConnected, //!< Connected, no new messages
        eMetaUpdate, //!< A join/part/mode change has occured
        eUpdate, //!< A room message has occurred
        eNotify, //!< A mention, PM, or notice occurred
        
        eConnecting, //!< Still connecting to server
        
        eFailed, //!< Connection has failed or is disconnected
        
        eChannelEnd = eNotify // Inclusive
    };
    
    struct ServerData {
        ServerStatus status;
        ServerController *arg;
        const char uri[]; //!< Null-terminated
    };
    
    struct ChannelData {
        ServerStatus status;
        ChannelController *arg;
    };
    
    static inline bool IsConnected(const ServerStatus s){
        return (int)s < eConnecting;
    }
    
    static inline bool IsConnected(const ServerData& s){
        return IsConnected(s.status);
    }
    
    static inline bool IsConnected(const ServerData* const s){
        assert(s != NULL);
        return IsConnected(s->status);
    }
    
private:
    
    static Fl_Menu_Item s_right_click_menu[9];
    Fl_Tree_Item *m_last_clicked;
    
    bool m_flash_tick;
    unsigned char m_num_connecting;
    
    static void TimeoutCallback(void *data){
        Fl::repeat_timeout(0.5, TimeoutCallback, data);
        ServerTree *const tree = static_cast<ServerTree*>(data);
        tree->m_flash_tick = !tree->m_flash_tick;
        tree->updateChildren();
        tree->redraw();
    }
    
    void updateChildren();
    
    void setSelected(Fl_Tree_Item *item);

    inline const Fl_Tree_Item *getSelected() const {
        const Fl_Tree &tree_this = *static_cast<const Fl_Tree*>(this);
        Fl_Tree &mutable_tree_this = const_cast<Fl_Tree&>(tree_this);
        const Fl_Tree_Item *const selected_item = mutable_tree_this.first_selected_item();
        return selected_item;
    }
public:
    
    // Shadow this.
    inline void select_only(Fl_Tree_Item *item){ setSelected(item); }

    inline Fl_Tree_Item *root() const {
        const Fl_Tree &tree_this = *static_cast<const Fl_Tree*>(this);
        Fl_Tree &mutable_tree_this = const_cast<Fl_Tree&>(tree_this);
        return mutable_tree_this.root();
    }

    //! @brief Updates the main window menus to reflect the currently selected server.
    inline void updateServerMenus(){ setSelected(first_selected_item()); }

    ServerTree(int a_x, int a_y, int a_w, int a_h, const char *a_title = NULL);
    virtual ~ServerTree(){}
    
    struct ServerData *addConnectingServer(const char *uri,
        const char *name, unsigned name_len);
    
    inline ServerData *connectionSucceeded(const char *name,
        unsigned name_len,
        ServerController *arg = NULL){

        ServerData *const server_data = setServerStatus(name, name_len, eConnected, arg);
        updateChildren();
        assert(m_num_connecting != 0);
        if(--m_num_connecting == 0)
            Fl::remove_timeout(TimeoutCallback, this);
        return server_data;
    }
    
    inline ServerData *connectionSucceeded(const std::string &name,
        ServerController *arg = NULL){
        
        ServerData *const server_data = setServerStatus(name, eConnected, arg);
        updateChildren();
        assert(m_num_connecting != 0);
        if(--m_num_connecting == 0)
            Fl::remove_timeout(TimeoutCallback, this);
        return server_data;
    }
    
    inline ServerData *connectionFailed(const char *name, unsigned name_len){
        ServerData *const server_data = setServerStatus(name, name_len, eFailed);
        updateChildren();
        assert(m_num_connecting != 0);
        if(--m_num_connecting == 0)
            Fl::remove_timeout(TimeoutCallback, this);
        return server_data;
    }
    
    inline ServerData *connectionFailed(const std::string &name){
        ServerData *const server_data = setServerStatus(name, eFailed);
        updateChildren();
        assert(m_num_connecting != 0);
        if(--m_num_connecting == 0)
            Fl::remove_timeout(TimeoutCallback, this);
        return server_data;
    }
    
    ChannelData *addChannel(const char *server,
        unsigned server_len,
        const char *channel,
        unsigned channel_len,
        ChannelController *user_data);

    inline ChannelData *addChannel(const std::string &server,
        const std::string &channel,
        ChannelController *user_data){
        return addChannel(server.c_str(),
            server.length(),
            channel.c_str(),
            channel.length(),
            user_data);
    }
    
    inline ChannelData *addChannel(const char *server,
        unsigned server_len,
        const std::string &channel,
        ChannelController *user_data){
        return addChannel(server,
            server_len,
            channel.c_str(),
            channel.length(),
            user_data);
    }
    
    inline ChannelData *addChannel(const std::string &server,
        const char *channel,
        unsigned channel_len,
        ChannelController *user_data){
        return addChannel(server.c_str(),
            server.length(),
            channel,
            channel_len,
            user_data);
    }
    
    ServerStatus getServerStatus(const std::string &server_name) const;
    ServerStatus getServerStatus(const char *name, size_t name_len) const;

    // Note that Fl::lock should be called for multithreaded access to this.
    ServerData *setServerStatus(const std::string &server_name,
        ServerStatus,
        ServerController *arg = NULL);
    ServerData *setServerStatus(const char *name,
        size_t name_len,
        ServerStatus,
        ServerController *arg = NULL);
    
    ServerData *getData(const std::string &server_name);
    ServerData *getData(const char *name, size_t name_len);
    
    virtual int handle(int e);
    
    // Note that Fl::lock should be called for multithreaded access to this.
    
    /**
     * @brief Checks if a channel of the server is selected.
     *
     * @p channel_only If true, then the server's channel must be selected to return true.
     *   Otherwise, it will return true if any channel in the server is selected.
     */
    bool isSelected(const ServerController &controller, bool channel_only) const;

    inline bool isSelected(const ServerData *ui_data, bool channel_only) const {
        return isSelected(*ui_data->arg, channel_only);
    }
    
    bool isSelected(const ChannelController &controller) const;
    
    bool isServerSelected(const std::string &server_name) const;

    inline bool isSelected(const ChannelData *ui_data) const {
        return isSelected(*ui_data->arg);
    }

};

} // namespace YYY

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // YYY_SERVER_TREE_HPP
