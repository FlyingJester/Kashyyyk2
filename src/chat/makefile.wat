# OpenWatcom makefile
# Builds yyychat

CC=wcc386
CFLAGS=-i="C:\WATCOM\h;C:\WATCOM\h\nt;..\chat" -w4 -e25 -zq -otexan -d1 -6r -bt=nt -fo=.obj -mf -br -bm
CXX=wcl386
CXXFLAGS=$(CFLAGS) -xs -c

OBJDIR=objects
LIBDIR=lib
YYYLIB=$(LIBDIR)\yyychat.lib

all: $(YYYLIB)

.BEFORE:
	IF not exist $(OBJDIR)\ mkdir $(OBJDIR)\
	IF not exist $(LIBDIR)\ mkdir $(LIBDIR)\

$(OBJDIR)\yyy_chat.obj: yyy_chat.cpp yyy_chat.hpp yyy_chat_message.h
	$(CXX) yyy_chat.cpp $(CXXFLAGS)
	move /Y yyy_chat.obj $(OBJDIR)\yyy_chat.obj

$(YYYLIB): $(OBJDIR)\yyy_chat.obj
	wlib -b -c $(YYYLIB) -+$(OBJDIR)\yyy_chat.obj

clean: .SYMBOLIC
	del /q /f $(OBJDIR)\*
    del /q /f $(YYYLIB)
