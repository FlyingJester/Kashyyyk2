#include "yyy_server_tree.hpp"

#include <algorithm>
#include <iterator>

#include <cstring>
#include <cassert>

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

typedef ServerTree::ServerStatus ServerStatus;
typedef ServerTree::ServerData ServerData;

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
  , m_flash_tick(false)
  , m_num_connecting(0){
    
    selectmode(FL_TREE_SELECT_SINGLE);
    showroot(0);
    
}

/*---------------------------------------------------------------------------*/
    
void ServerTree::addConnectingServer(const char *uri,
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
}

/*---------------------------------------------------------------------------*/

int ServerTree::handle(int e){
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
    else
        return Fl_Tree::handle(e);
}

/*---------------------------------------------------------------------------*/

ServerData *ServerTree::getData(const std::string &server_name){
    Fl_Tree_Item *const l_root = root();
    const unsigned num = l_root->children();
    for(unsigned i = 0; i < num; i++){
        Fl_Tree_Item *const l_child = l_root->child(i);
        if(l_child->label() == server_name)
            return static_cast<ServerData*>(l_child->user_data());
    }
    
    return NULL;
}

/*---------------------------------------------------------------------------*/

ServerData *ServerTree::getData(const char *name, unsigned name_len){
    Fl_Tree_Item *const l_root = root();
    const unsigned num = l_root->children();
    for(unsigned i = 0; i < num; i++){
        Fl_Tree_Item *const l_child = l_root->child(i);
        const char *const l_label = l_child->label();
        if(strncmp(l_label, name, name_len) == 0 && l_label[name_len] == '\0')
            return static_cast<ServerData*>(l_child->user_data());
    }
    
    return NULL;
}

/*---------------------------------------------------------------------------*/

ServerStatus ServerTree::getServerStatus(const std::string &server_name) const{
    const ServerData *const data =
        const_cast<ServerTree*>(this)->getData(server_name);
    if(data)
        return data->status;
    else
        return eFailed;
}

/*---------------------------------------------------------------------------*/

ServerStatus ServerTree::getServerStatus(const char *name, unsigned name_len) const{
    const ServerData *const data =
        const_cast<ServerTree*>(this)->getData(name, name_len);
    if(data)
        return data->status;
    else
        return eFailed;
}

/*---------------------------------------------------------------------------*/

// Note that Fl::lock should be called for multithreaded access to this.
void ServerTree::setServerStatus(const std::string &server_name, ServerStatus status, void *arg){
    ServerData *const data = getData(server_name);
    if(data){
        data->status = status;
        data->arg = arg;
    }
}

/*---------------------------------------------------------------------------*/

void ServerTree::setServerStatus(const char *name, unsigned name_len, ServerStatus status, void *arg){
    ServerData *const data = getData(name, name_len);
    if(data){
        data->status = status;
        data->arg = arg;
    }
}

/*---------------------------------------------------------------------------*/
// Fastcall forces flash to be on the stack, which for Watcom allows it to be
// put in the same stack slot for the entire body of updateChildren.
static void YYY_FASTCALL yyy_apply_item_color(Fl_Tree_Item *const item,
    ServerStatus s, bool flash){
    
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

void ServerTree::updateChildren(){
    
    assert(root() != NULL);
    Fl_Tree_Item &l_root = *root();
    const unsigned num_servers = l_root.children();
    
    for(unsigned i = 0; i < num_servers; i++){
        Fl_Tree_Item *const l_server = l_root.child(i);
        const unsigned num_channels = l_server->children();
        
        ServerData *const l_data =
            static_cast<ServerData *>(l_server->user_data());
        
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
                yyy_apply_item_color(l_channel, (ServerStatus)other_s, false);
            }
        }
        l_data->status = (ServerStatus)s;
        yyy_apply_item_color(l_server, (ServerStatus)s, m_flash_tick);
    }
}

} // namespace YYY
