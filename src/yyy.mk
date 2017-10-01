# Any copyright of this file is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# Defines the files that objects depend on.

YYY_UTILS=utils/yyy_alloca.h utils/yyy_maintainer.hpp utils/yyy_attributes.h

YYY_CONNECT_DEP=$(GENDIR)/yyy_connect.cpp $(GENDIR)/yyy_connect.hpp yyy_callbacks.h
KASHYYYK2_DEP=$(GENDIR)/kashyyyk2.cpp $(GENDIR)/kashyyyk2.hpp yyy_callbacks.h $(GENDIR)/yyy_connect.hpp ui/yyy_chat_widget.hpp
YYY_ASK_DEP=yyy_ask.cpp yyy_ask.h
YYY_CALLBACKS_DEP=yyy_callbacks.c yyy_callbacks.h
YYY_MAIN_DEP=yyy_main.cpp yyy_main.h yyy_prefs.h $(GENDIR)/kashyyyk2.hpp yyy_buffer.h ui/yyy_server_tree.hpp $(YYY_UTILS)
YYY_SERVER_THREAD_DEP=yyy_server_thread.cpp yyy_server_thread.hpp yyy_server_core.hpp $(NETDIR)/yyy_network.h $(THRDIR)/yyy_thread.h $(MONDIR)/yyy_monitor.hpp
YYY_CHAT_WIDGET_DEP=ui/yyy_chat_widget.cpp ui/yyy_chat_widget.hpp $(YYY_UTILS)
YYY_SERVER_TREE_DEP=ui/yyy_server_tree.cpp ui/yyy_server_tree.hpp yyy_main.h
YYY_CHANNEL_UI_DEP=yyy_channel_ui.cpp yyy_channel_ui.hpp yyy_channel_core.hpp yyy_date.h
YYY_CHANNEL_CORE_DEP=yyy_channel_core.cpp yyy_channel_core.hpp yyy_date.h
YYY_SERVER_UI_DEP=yyy_server_ui.cpp yyy_server_ui.hpp yyy_server_core.hpp yyy_channel_ui.hpp yyy_channel_core.hpp yyy_date.h
YYY_SERVER_CORE_DEP=yyy_server_core.cpp yyy_server_core.hpp yyy_channel_core.hpp yyy_buffer.h
YYY_BUFFER_DEP=yyy_buffer.c yyy_buffer.h
YYY_DATE_DEP=yyy_date.c yyy_date.h
YYY_CONNECT_THREAD_DEP=yyy_connect_thread.c yyy_connect_thread.h yyy_main.h $(THRDIR)/yyy_thread.h $(MONDIR)/yyy_monitor_backend.h $(NETDIR)/yyy_network.h
YYY_PREFS_DEP=yyy_prefs.c yyy_prefs.h
	
