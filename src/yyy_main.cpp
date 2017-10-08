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

#include "yyy_main.h"

#include "yyy_assert.h"
#include "yyy_connect_thread.h"

// Vector must be included before Fl_Window or else Watcom literally crashes.
// The issue seems related with FL_x.H including Windows.h, which redefines
// 'max' in a way that seriously confuses Watcom. To the point of crashing.
// Somehow that redefine messes up how the std::numeric_limits<>::max() is
// understood by the compiler.
#include <vector>

// Included early to fix warnings about unary & operators.
#include <FL/fl_ask.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Tree_Item.H>

#include "kashyyyk2.hpp"

#include "ui/yyy_server_tree.hpp"

#include "yyy_server_core.hpp"
#include "yyy_server_ui.hpp"
#include "yyy_server_thread.hpp"

#include "yyy_maintainer.hpp"
#include "yyy_alloca.h"

#include "monitor/yyy_monitor.hpp"
#include "thread/yyy_thread.h"

#include <FL/Fl.H>
#include <string>
#include <stdlib.h>
#include <assert.h>

#ifdef YYY_ENABLE_IRC
#include "irc/yyy_irc.hpp"
#endif
#ifdef YYY_ENABLE_SPARK
#include "irc/yyy_spark.hpp"
#endif
#ifdef YYY_ENABLE_DISCORD
#include "irc/yyy_discord.hpp"
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "TCHAR.h"
#endif

const char help_string[] = "Kashyyyk2\n"\
    "    -v, --version   Display version information\n"\
    "    -h, --help      Display this help information\n";

namespace YYY {

/*---------------------------------------------------------------------------*/

struct Window {
    
    Fl_Double_Window *m_window;
    Maintainer<ServerCore> m_servers;
    
    ServerTree *m_server_tree;
    ServerThread *m_server_thread;
    
} yyy_main_window;

/*---------------------------------------------------------------------------*/

#ifdef YYY_ENABLE_IRC
static ChatProtocol *irc_protocol;
#endif

/*---------------------------------------------------------------------------*/

#ifdef YYY_ENABLE_SPARK
ChatProtocol *spark_protocol;
#endif

/*---------------------------------------------------------------------------*/

#ifdef YYY_ENABLE_DISCORD
ChatProtocol *discord_protocol;
#endif

/*---------------------------------------------------------------------------*/

static inline const char *yyy_determine_server_name(const char *url,
    unsigned short &name_len){
    
    const char *name;
    
    unsigned short dot_indices[2] = {0, 0};
    // Sort out the probable name of the server. This mostly depends on
    // where the domain separators (aka, a dot) appear. If there is only
    // one, everything before the separator is used. If there are two or
    // more, the name is everything after the first but before the last.
    unsigned short len = 0;
    for(unsigned short dot = 0; url[len] != '\0'; len++){
        if(url[len] == '.'){
            dot_indices[dot] = len;
            dot = 1;
        }
    }
    
    YYY_Assert(dot_indices[0] != 0 || dot_indices[1] != 0, "Invalid URI");
    
    if(dot_indices[0] == 0 && dot_indices[1] == 0){
        // What the actual fart?
        name = url;
        name_len = len;
    }
    else if(dot_indices[1] == 0){
        name = url;
        name_len = dot_indices[0];
    }
    else{
        name = url + dot_indices[0] + 1;
        name_len = dot_indices[1] - (dot_indices[0] + 1);
    }
    
    return name;
}

/*---------------------------------------------------------------------------*/

static inline Window &yyy_connection_args(const char *uri, void *arg,
    const char *&out_name, unsigned short &out_name_len){
    
    assert(uri != NULL);
    assert(*uri != '\0');
    
    out_name = yyy_determine_server_name(uri, out_name_len);
    
    return (arg == NULL) ? yyy_main_window : *static_cast<Window*>(arg);
}

/*---------------------------------------------------------------------------*/

extern "C"
void YYY_FASTCALL YYY_AttemptingConnection(const char *uri, void *arg){

    unsigned short name_len;
    const char *name;
    Window &window = yyy_connection_args(uri, arg, name, name_len);
    
    Fl::lock();

    window.m_server_tree->addConnectingServer(uri, name, name_len);
    window.m_server_tree->redraw();
    
    Fl::unlock();
}

/*---------------------------------------------------------------------------*/

extern "C"
void YYY_FASTCALL YYY_FailedConnection(const char *uri, void *arg){

    unsigned short name_len;
    const char *name;
    Window &window = yyy_connection_args(uri, arg, name, name_len);
    
    Fl::lock();

    window.m_server_tree->connectionFailed(name, name_len);
    window.m_server_tree->redraw();
    
    Fl::unlock();
}

/*---------------------------------------------------------------------------*/

extern "C"
void YYY_FASTCALL YYY_AddConnection(struct YYY_NetworkSocket *socket, const char *uri,
    void *arg){

    unsigned short name_len;
    const char *name;
    Window &window = yyy_connection_args(uri, arg, name, name_len);
    
    ServerCore &server = window.m_servers.create();
    server.setName(name, name_len);
    server.createNewUi();
    server.setSocket(socket);
    
    // TEST: Assume IRC for now.
    server.setProtocol(*irc_protocol);
    
    window.m_server_thread->addServer(server);
    
    Fl::lock();

    window.m_server_tree->connectionSucceeded(name, name_len, &server);
    window.m_server_tree->redraw();
    
    Fl::unlock();
}

/*---------------------------------------------------------------------------*/

static void yyy_server_tree_callback(Fl_Widget *w, void *arg){
    assert(w != NULL);
    assert(arg != NULL);
    Window &window = *static_cast<Window*>(arg);
    ServerTree &tree = *static_cast<ServerTree*>(w);
    
    (void)window;
    
    if(tree.callback_reason() != FL_TREE_REASON_SELECTED)
        return;
    
    Fl_Tree_Item *const item = tree.callback_item();

    const char *l = item->label();
    assert(l != NULL);
    
    if(item->parent() == tree.root()){ // Is a server
        ServerTree::ServerData *const data =
            (ServerTree::ServerData*)(item->user_data());
        assert(data != NULL);
        ServerCore *const server = data->arg;
        if(ServerTree::IsConnected(data)){
            assert(server != NULL);
            ServerUI *const serverUI = server->getUI();
            assert(serverUI != NULL);
            ChannelUI &channel = serverUI->serverChannel();
            channel.updateScroll(*chat_scroll);
            channel.updateChatWidget(*chat_widget, *chat_scroll);
            chat_scroll->redraw();
            chat_widget->redraw();
        }
        else if(server != NULL){
            // Since we only put a server/arg into the server data when a
            // connection succeeds, this means we were previously connected,
            // and then got disconnected.
        }
        
        
    }
    else{ // Is a channel
        
    }
}

/*---------------------------------------------------------------------------*/
// args must be free-able by YYY_ALLOCA_FREE.
static bool Main(unsigned num_args, const std::string *args){

#ifndef NDEBUG
    #if defined __GNUC__
        Fl::scheme("plastic");
    #elif defined __WATCOMC__
        Fl::scheme("gleam");
    #else
        Fl::scheme("gtk+");
    #endif
#else
    Fl::scheme("gtk+");
#endif
    
#ifdef YYY_ENABLE_IRC
    irc_protocol = new IRCProtocol();
#endif

#ifdef YYY_ENABLE_SPARK
    ChatProtocol *spark_protocol;
#endif

#ifdef YYY_ENABLE_DISCORD
    ChatProtocol *discord_protocol;
#endif

    YYY_StartConnectThread();
    yyy_main_window.m_window = YYY_MakeWindow();
#if _WIN32
    // Get the application icon, and set the FLTK window to this icon.
    {
        const HINSTANCE instance = GetModuleHandle(NULL);
        HICON icon = LoadIcon(instance, IDI_APPLICATION);
        if(icon != NULL){
            yyy_main_window.m_window->icon(icon);
            DestroyIcon(icon);
        }
        else{
            OutputDebugString(TEXT("Could not load icon\n"));
        }
    }
#endif
    yyy_main_window.m_server_tree = server_tree;
    yyy_main_window.m_window->show();
    yyy_main_window.m_server_thread = new ServerThread();
    yyy_main_window.m_server_thread->start();
    
    server_tree->callback(yyy_server_tree_callback, &yyy_main_window);
    
    Fl::lock();
    Fl::run();
    Fl::unlock();
    
    YYY_StopConnectThread();
    
    delete yyy_main_window.m_server_thread;
    delete yyy_main_window.m_window;
    
    YYY_ALLOCA_FREE(args);
    
    return EXIT_SUCCESS;
}

} // namespace YYY

/*---------------------------------------------------------------------------*/
// Returns false if the option means we should quit.
static bool yyy_handle_arg(const char *arg){
    if(arg[0] != '-')
        return true;
    if((arg[1] == 'h' && (arg[2] == '\0' || strcmp(arg+2, "elp") == 0)) ||
        strcmp(arg+1, "-help") == 0){
        fwrite(help_string, sizeof(help_string)-1, 1, stdout);
        fflush(stdout);
        return false;
    }
    if((arg[1] == 'v' && (arg[2] == '\0' || strcmp(arg+2, "ersion") == 0)) ||
        strcmp(arg+1, "-version") == 0){
        printf("Kashyyyk 2\nCompiled %s using %s and FLTK %f\n"\
            "Copyright Martin McDonough (C) 2017, released under the Mozilla Public License 1.1\n",
            __DATE__,
#if (defined __clang__) || (defined __CLANG__)
        "Clang",
#elif defined __GNUC__
        "GNU gcc/g++ Compiler",
#elif defined _MSC_VER
        "Microsoft Visual C/C++ Compiler",
#elif defined __WATCOMC__
        "Open Watcom C/C++ Compiler",
#else
        "Unknown C/C++ Compiler",
#endif
        Fl::version());
        fflush(stdout);
        return false;
    }
    
    return true;
}

/*---------------------------------------------------------------------------*/

#if (defined _MSC_VER) || (((defined WIN32) || (defined _WIN32)) && (defined __GNUC__))

#include <Windows.h>

extern "C"
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int){
    LPWSTR raw_args = GetCommandLineW();
    int argc = 0;
    LPWSTR *const argv = CommandLineToArgvW(raw_args, &argc);
    std::string *const args = (std::string*)YYY_ALLOCA(argc*sizeof(std::string));
    for(int i = 0; i < argc; i++){
        const unsigned len = wcslen(argv[i]);
        new (args + i) std::string(len + sizeof(TEXT("")), ' ');
        size_t resize_len;
#ifdef __WATCOMC__
        resize_len = wcstombs(&(args[i][0]), argv[i], args[i].length());
#else
        wcstombs_s(&resize_len, &(args[i][0]), args[i].length(), argv[i], len);
#endif
        args[i].resize(resize_len);
        if(!yyy_handle_arg(args[i].c_str()))
            return EXIT_SUCCESS;
    }
    
    return YYY::Main(argc, args) ? EXIT_SUCCESS : EXIT_FAILURE;
}

#else

int main(int argc, char **argv){
    std::string *const args = (std::string*)YYY_ALLOCA(argc*sizeof(std::string));
    for(unsigned i = 0; i < static_cast<unsigned>(argc); i++){
        if(!yyy_handle_arg(argv[i])){
            return EXIT_SUCCESS;
        }
        else{
            new (args + i) std::string(argv[i]);
        }
    }
    
    return YYY::Main(argc, args) ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif
