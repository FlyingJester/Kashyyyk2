# OpenWatcom makefile
# Builds yyyirc

CC=wcc386
CFLAGS=-i="C:\WATCOM\h;C:\WATCOM\h\nt;..\chat" -w4 -e25 -zq -otexan -d1 -6r -bt=nt -fo=.obj -mf -br -bm
CXX=wcl386
CXXFLAGS=$(CFLAGS) -xs -c

OBJDIR=objects
LIBDIR=lib
YYYLIB=$(LIBDIR)\yyyirc.lib

all: $(YYYLIB)

.BEFORE:
	IF not exist $(OBJDIR)\ mkdir $(OBJDIR)\
	IF not exist $(LIBDIR)\ mkdir $(LIBDIR)\

$(OBJDIR)\yyy_irc.obj: yyy_irc.cpp yyy_irc.hpp yyy_irc_core.h ..\chat\yyy_chat.hpp ..\chat\yyy_chat_message.h
	$(CXX) yyy_irc.cpp $(CXXFLAGS)
	move /Y yyy_irc.obj $(OBJDIR)\yyy_irc.obj

$(OBJDIR)\yyy_irc_core.obj: yyy_irc_core.c yyy_irc_core.h ..\chat\yyy_chat_message.h
	$(CC) yyy_irc_core.c $(CFLAGS)
	move /Y yyy_irc_core.obj $(OBJDIR)\yyy_irc_core.obj

$(YYYLIB): $(OBJDIR)\yyy_irc.obj $(OBJDIR)\yyy_irc_core.obj
	wlib -b -c $(YYYLIB) -+$(OBJDIR)\yyy_irc.obj -+$(OBJDIR)\yyy_irc_core.obj

clean: .SYMBOLIC
	del /q /f $(OBJDIR)\*
    del /q /f $(YYYLIB)
