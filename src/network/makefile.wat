# OpenWatcom makefile
# Builds yyynetwork

CXX=wcl386
CC=wcc386
LINKER=wlink

CFLAGS=-i="C:\WATCOM\h\nt;C:\WATCOM\h" -w2 -e25 -zq -otexan -d1 -6r -bt=nt -fo=.obj -mf -br -bm
CXXFLAGS=$(CFLAGS)

OBJDIR=objects
LIBDIR=lib
YYYLIB=$(LIBDIR)\yyynetwork.lib

all: $(YYYLIB)

.BEFORE:
	IF not exist $(OBJDIR)\ mkdir $(OBJDIR)\
	IF not exist $(LIBDIR)\ mkdir $(LIBDIR)\

!ifeq os unix
os=unix
poll=poll
osdefine=UNIX
!else ifeq os dos
os=dos
poll=select
osdefine=DOS
!else
os=win32
poll=select
osdefine=WIN32
!endif

BACKEND=yyy_network_$(os)
BACKEND_C=$(BACKEND).c
BACKEND_H=$(BACKEND).h
BACKEND_OBJ=$(OBJDIR)\$(BACKEND).obj

POLLAPI=yyy_network_$(poll)
POLLAPI_C=$(POLLAPI).c
POLLAPI_OBJ=$(OBJDIR)\$(POLLAPI).obj

$(BACKEND_OBJ): $(BACKEND_H) $(BACKEND_C)
	$(CC) $(BACKEND_C) $(CFLAGS)
	move /Y $(BACKEND).obj $(BACKEND_OBJ)
    
$(POLLAPI_OBJ): $(POLLAPI_C) $(BACKEND_H) yyy_network.h yyy_network_common.h
	$(CC) $(POLLAPI_C) $(CFLAGS) -DYYY_NETWORK_$(osdefine) 
	move /Y $(POLLAPI).obj $(POLLAPI_OBJ)

$(OBJDIR)\yyy_network_common.obj: yyy_network_common.c yyy_network_common.h
	$(CC) yyy_network_common.c $(CCFLAGS)
	move /Y yyy_network_common.obj $(OBJDIR)

# Intentionally using the C++ compiler on a C file, this means that __iob is
# inlined and we don't need to depend on the Watcom C++ network library, only
# the C version.
$(OBJDIR)\yyy_network.obj: yyy_network.c yyy_network.h $(BACKEND_H)
	$(CXX) yyy_network.c -DYYY_NETWORK_$(osdefine) $(CXXFLAGS) -c
	move /Y yyy_network.obj $(OBJDIR)

$(YYYLIB): $(OBJDIR)\yyy_network.obj $(BACKEND_OBJ) $(POLLAPI_OBJ) $(OBJDIR)\yyy_network_common.obj
	wlib -b -c $(YYYLIB) -+$(OBJDIR)\yyy_network.obj -+$(BACKEND_OBJ) -+$(POLLAPI_OBJ) -+$(OBJDIR)\yyy_network_common.obj

clean: .SYMBOLIC
	del /q /f $(OBJDIR)\*
    del /q /f $(YYYLIB)
