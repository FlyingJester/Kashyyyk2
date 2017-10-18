# OpenWatcom makefile
# Builds yyyirc

CC=wcc386
CXX=wcl386
LINKER=wlink
CFLAGS=-i="C:\WATCOM\h;C:\WATCOM\h\nt;..\chat;..\..\test" -w4 -e25 -zq -otexan -d1 -6r -bt=nt -fo=.obj -mf -br -bm
CXXFLAGS=$(CFLAGS) -xs -c

OBJDIR=objects
LIBDIR=lib
TESTDIR=test
YYYLIB=$(LIBDIR)\yyyirc.lib
TESTEXE=$(TESTDIR)\yyy_irc_core_test.exe
LINKPATH=C:\WATCOM\lib386\nt
LINKFLAGS= sys nt_win op m op maxe=25 op q op symf LIBPATH $(LINKPATH) DEBUG dwarf 


all: $(YYYLIB)
test: $(TESTEXE) .SYMBOLIC
	$(TESTEXE) > test.log || type test.log
	type test.log

.BEFORE:
	IF not exist $(OBJDIR)\ mkdir $(OBJDIR)\
	IF not exist $(LIBDIR)\ mkdir $(LIBDIR)\
	IF not exist $(TESTDIR)\ mkdir $(TESTDIR)\

$(OBJDIR)\yyy_irc.obj: yyy_irc.cpp yyy_irc.hpp yyy_irc_core.h ..\chat\yyy_chat.hpp ..\chat\yyy_chat_message.h
	$(CXX) yyy_irc.cpp $(CXXFLAGS)
	move /Y yyy_irc.obj $(OBJDIR)\yyy_irc.obj

$(OBJDIR)\yyy_irc_core.obj: yyy_irc_core.c yyy_irc_core.h ..\chat\yyy_chat_message.h
	$(CC) yyy_irc_core.c $(CFLAGS)
	move /Y yyy_irc_core.obj $(OBJDIR)\yyy_irc_core.obj

$(OBJDIR)\yyy_irc_core_test.obj: yyy_irc_core_test.c yyy_irc_core.h ..\chat\yyy_chat_message.h
	$(CC) yyy_irc_core_test.c $(CFLAGS) 
	move /Y yyy_irc_core_test.obj $(OBJDIR)\yyy_irc_core_test.obj

$(YYYLIB): $(OBJDIR)\yyy_irc.obj $(OBJDIR)\yyy_irc_core.obj
	wlib -b -c $(YYYLIB) -+$(OBJDIR)\yyy_irc.obj -+$(OBJDIR)\yyy_irc_core.obj

SYSLIBS=Advapi32.lib Kernel32.lib Advapi32.lib User32.lib Ws2_32.lib
TESTOBJS=$(OBJDIR)\yyy_irc_core.obj $(OBJDIR)\yyy_irc_core_test.obj
LINK_YYY_IRC_CORE_TEST=$(LINKER) $(LINKFLAGS) FILE { $(TESTOBJS) } LIBRARY { $(SYSLIBS) } NAME $(TESTEXE)

$(TESTEXE): $(TESTOBJS)
	$(LINK_YYY_IRC_CORE_TEST)

clean: .SYMBOLIC
	del /q /f $(OBJDIR)\*
    del /q /f $(YYYLIB)
