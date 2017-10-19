# OpenWatcom makefile
# Builds Kashyyyk2 (including the yyy libs)

CXX=wcl386
CC=wcc386
LINKER=wlink
RC=wrc

!ifeq os unix
os=unix
EXT=
!else ifeq os dos
os=dos
EXT=.com
!else
os=win32
EXT=.exe
!endif

FLTK_DIR=..\fltk-watcom
FLUID=$(FLTK_DIR)\bin\fluid

OBJDIR=objects
GENDIR=build

INCFLAGS=-i="C:\WATCOM\h\nt;C:\WATCOM\h;include;$(FLTK_DIR);$(FLTK_DIR)\include;utils;$(GENDIR)"
CFLAGS=$(INCFLAGS) -w3 -e25 -en -zq -otexan -d2 -os -6r -bt=nt -fo=.obj -mf -br -bm -dWIN32 -dYYY_ENABLE_IRC
CXXFLAGS=$(CFLAGS) -xs -c
LINKPATH=C:\WATCOM\lib386\nt;monitor\lib
LINKFLAGS= sys nt_win op m op maxe=25 op q op symf LIBPATH $(LINKPATH) DEBUG dwarf 

OBJ0=$(OBJDIR)\yyy_server_ui.obj $(OBJDIR)\yyy_server_core.obj
OBJ1=$(OBJDIR)\yyy_channel_ui.obj $(OBJDIR)\yyy_channel_core.obj $(OBJDIR)\yyy_join.obj
OBJ2=$(OBJDIR)\kashyyyk2.obj $(OBJDIR)\yyy_main.obj $(OBJDIR)\yyy_buffer.obj
OBJ3=$(OBJDIR)\yyy_ask.obj $(OBJDIR)\yyy_connect.obj $(OBJDIR)\yyy_callbacks.obj
OBJ4=$(OBJDIR)\yyy_date.obj $(OBJDIR)\yyy_theme.obj $(OBJDIR)\yyy_chat_widget.obj
OBJ5=$(OBJDIR)\yyy_server_thread.obj $(OBJDIR)\yyy_connect_thread.obj
OBJ6=$(OBJDIR)\yyy_server_tree.obj

OBJECTS=$(OBJ0) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6)

KASHYYYK2=kashyyyk2$(EXT)
all: $(KASHYYYK2)

.BEFORE:
	IF not exist $(OBJDIR)\ mkdir $(OBJDIR)\
	IF not exist $(GENDIR)\ mkdir $(GENDIR)\

PX=
SX=lib
MK=wat

MAKE+= -h 

!include "libs.mk"

!include "yyy.mk"

# Fluid autogen files
$(GENDIR)\yyy_connect.cpp: ui\yyy_connect.fl
	$(FLUID) -c ui\yyy_connect.fl
	move /Y yyy_connect.cpp $(GENDIR)
	move /Y yyy_connect.hpp $(GENDIR)

$(GENDIR)\yyy_connect.hpp: ui\yyy_connect.fl
	$(FLUID) -c ui\yyy_connect.fl
	move /Y yyy_connect.cpp $(GENDIR)
	move /Y yyy_connect.hpp $(GENDIR)

$(GENDIR)\yyy_join.hpp: ui\yyy_join.fl
	$(FLUID) -c ui\yyy_join.fl
	move /Y yyy_join.cpp $(GENDIR)
	move /Y yyy_join.hpp $(GENDIR)

$(GENDIR)\yyy_join.cpp: ui\yyy_join.fl
	$(FLUID) -c ui\yyy_join.fl
	move /Y yyy_join.cpp $(GENDIR)
	move /Y yyy_join.hpp $(GENDIR)

$(GENDIR)\kashyyyk2.cpp: ui\kashyyyk2.fl
	$(FLUID) -c ui\kashyyyk2.fl
	move /Y kashyyyk2.cpp $(GENDIR)
	move /Y kashyyyk2.hpp $(GENDIR)

$(GENDIR)\kashyyyk2.hpp: ui\kashyyyk2.fl
	$(FLUID) -c ui\kashyyyk2.fl
	move /Y kashyyyk2.cpp $(GENDIR)
	move /Y kashyyyk2.hpp $(GENDIR)

# Compile autogen files
$(OBJDIR)\yyy_connect.obj: $(YYY_CONNECT_DEP)
	$(CXX) $(GENDIR)\yyy_connect.cpp $(CXXFLAGS) -I.\
	move /Y yyy_connect.obj $(OBJDIR)

$(OBJDIR)\yyy_join.obj: $(YYY_JOIN_DEP)
	$(CXX) $(GENDIR)\yyy_join.cpp $(CXXFLAGS) -I.\
	move /Y yyy_join.obj $(OBJDIR)

$(OBJDIR)\kashyyyk2.obj: $(KASHYYYK2_DEP)
	$(CXX) $(GENDIR)\kashyyyk2.cpp $(CXXFLAGS) -I.\
	move /Y kashyyyk2.obj $(OBJDIR)

# Non-autogen source
$(OBJDIR)\yyy_ask.obj: $(YYY_ASK_DEP)
	$(CXX) yyy_ask.cpp $(CXXFLAGS)
	move /Y yyy_ask.obj $(OBJDIR)

$(OBJDIR)\yyy_callbacks.obj: $(YYY_CALLBACKS_DEP)
	$(CC) yyy_callbacks.c $(CFLAGS)
	move /Y yyy_callbacks.obj $(OBJDIR)

$(OBJDIR)\yyy_main.obj: $(YYY_MAIN_DEP)
	$(CXX) yyy_main.cpp $(CXXFLAGS) -I$(GENDIR)
	move /Y yyy_main.obj $(OBJDIR)

$(OBJDIR)\yyy_server_thread.obj: $(YYY_SERVER_THREAD_DEP)
	$(CXX) yyy_server_thread.cpp $(CXXFLAGS)
	move /Y yyy_server_thread.obj $(OBJDIR)

$(OBJDIR)\yyy_connect_thread.obj: $(YYY_CONNECT_THREAD_DEP)
	$(CC) yyy_connect_thread.c $(CFLAGS)
	move /Y yyy_connect_thread.obj $(OBJDIR)

$(OBJDIR)\yyy_theme.obj: $(YYY_THEME_DEP)
	$(CXX) yyy_theme.cpp $(CXXFLAGS)
	move /Y yyy_theme.obj $(OBJDIR)

# UI and Core
$(OBJDIR)\yyy_chat_widget.obj: $(YYY_CHAT_WIDGET_DEP)
	$(CXX) ui\yyy_chat_widget.cpp $(CXXFLAGS)
	move /Y yyy_chat_widget.obj $(OBJDIR)
    
$(OBJDIR)\yyy_server_tree.obj: $(YYY_SERVER_TREE_DEP)
	$(CXX) ui\yyy_server_tree.cpp $(CXXFLAGS)
	move /Y yyy_server_tree.obj $(OBJDIR)

$(OBJDIR)\yyy_channel_ui.obj: $(YYY_CHANNEL_UI_DEP)
	$(CXX) yyy_channel_ui.cpp $(CXXFLAGS)
	move /Y yyy_channel_ui.obj $(OBJDIR)

$(OBJDIR)\yyy_channel_core.obj: $(YYY_CHANNEL_CORE_DEP)
	$(CXX) yyy_channel_core.cpp $(CXXFLAGS)
	move /Y yyy_channel_core.obj $(OBJDIR)

$(OBJDIR)\yyy_server_ui.obj: $(YYY_SERVER_UI_DEP)
	$(CXX) yyy_server_ui.cpp $(CXXFLAGS)
	move /Y yyy_server_ui.obj $(OBJDIR)

$(OBJDIR)\yyy_server_core.obj: $(YYY_SERVER_CORE_DEP)
	$(CXX) yyy_server_core.cpp $(CXXFLAGS)
	move /Y yyy_server_core.obj $(OBJDIR)

# utils
$(OBJDIR)\yyy_buffer.obj:  $(YYY_BUFFER_DEP)
	$(CC) yyy_buffer.c $(CFLAGS)
	move /Y yyy_buffer.obj $(OBJDIR)
    
$(OBJDIR)\yyy_date.obj: $(YYY_DATE_DEP)
	$(CC) yyy_date.c $(CFLAGS)
	move /Y yyy_date.obj $(OBJDIR)

resource.res: resource.rc
	wrc -r -bt=nt -I"C:\Watcom\h\nt;C:\Watcom\h" resource.rc

# Application linking
SYSLIBS=Advapi32.lib Kernel32.lib Advapi32.lib User32.lib Ws2_32.lib
LINK_KASHYYYK=$(LINKER) $(LINKFLAGS) RES resource.res FILE { $(OBJECTS) } LIBRARY { $(FLTK_DIR)\lib\fltk.lib $(SYSLIBS) $(YYYLIBS) } NAME $(KASHYYYK2)
$(KASHYYYK2): $(OBJECTS) $(YYYLIBS) resource.res
	$(LINK_KASHYYYK)

test: .SYMBOLIC
	cd $(IRCDIR) && $(MAKE) -f makefile.wat test

relink: .SYMBOLIC
	del /q $(KASHYYYK2)
	del /q $(MONLIB)
	del /q $(IRCLIB)
	del /q $(NETLIB)
	del /q $(THRLIB)
	cd $(MONDIR) && $(MAKE) -f makefile.wat
	cd $(NETDIR) && $(MAKE) -f makefile.wat
	cd $(IRCDIR) && $(MAKE) -f makefile.wat
	cd $(THRDIR) && $(MAKE) -f makefile.wat
	$(LINK_KASHYYYK)

clean: .SYMBOLIC
	cd $(MONDIR) && $(MAKE) -f makefile.wat clean
	cd $(NETDIR) && $(MAKE) -f makefile.wat clean
	cd $(IRCDIR) && $(MAKE) -f makefile.wat clean
	cd $(THRDIR) && $(MAKE) -f makefile.wat clean
	del /q /f $(OBJDIR)\*
	del /q /f $(GENDIR)\*
	del /q $(KASHYYYK2)
	del /q kashyyyk2.map
    del /q *.err
    del *.err
