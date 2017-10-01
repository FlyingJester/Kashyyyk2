#pragma once

#include <FL/Fl_Image.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Menu_Item.H>

#ifdef __WATCOMC__
#undef max
#undef min
#endif

#include <vector>
#include <string>

#include <cassert>

/** 
 * @file yyy_server_tree.hpp
 * @brief Tree Widget to display Server and Channel names
 * @author Martin McDonough
 * @date 2017
 * @copyright YYY Project License
 */

namespace YYY {

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
        void *arg;
        const char uri[]; //!< Null-terminated
    };
    
    struct ChannelData {
        ServerStatus status;
        void *arg;
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
    
public:
    
    ServerTree(int a_x, int a_y, int a_w, int a_h, const char *a_title = NULL);
    virtual ~ServerTree(){}
    
    void addConnectingServer(const char *uri,
        const char *name, unsigned name_len);
    
    inline void connectionSucceeded(const char *name, unsigned name_len, void *arg = NULL){
        setServerStatus(name, name_len, eConnected, arg);
        updateChildren();
        assert(m_num_connecting != 0);
        if(--m_num_connecting == 0)
            Fl::remove_timeout(TimeoutCallback, this);
    }
    
    inline void connectionSucceeded(const std::string &name, void *arg = NULL){
        setServerStatus(name, eConnected, arg);
        updateChildren();
        assert(m_num_connecting != 0);
        if(--m_num_connecting == 0)
            Fl::remove_timeout(TimeoutCallback, this);
    }
    
    inline void connectionFailed(const char *name, unsigned name_len){
        setServerStatus(name, name_len, eFailed);
        updateChildren();
        assert(m_num_connecting != 0);
        if(--m_num_connecting == 0)
            Fl::remove_timeout(TimeoutCallback, this);
    }
    
    inline void connectionFailed(const std::string &name){
        setServerStatus(name, eFailed);
        updateChildren();
        assert(m_num_connecting != 0);
        if(--m_num_connecting == 0)
            Fl::remove_timeout(TimeoutCallback, this);
    }
    
    void addChannel(const char *server, const std::string &name);
    
    ServerStatus getServerStatus(const std::string &server_name) const;
    ServerStatus getServerStatus(const char *name, unsigned name_len) const;

    // Note that Fl::lock should be called for multithreaded access to this.
    void setServerStatus(const std::string &server_name, ServerStatus, void *arg = NULL);
    void setServerStatus(const char *name, unsigned name_len, ServerStatus, void *arg = NULL);
    
    ServerData *getData(const std::string &server_name);
    ServerData *getData(const char *name, unsigned name_len);
    
    virtual int handle(int e);
    
};

} // namespace YYY
