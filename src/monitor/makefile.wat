# OpenWatcom makefile
# Builds yyymonitor

CXX=wcl386
CC=wcc386
LINKER=wlink

CFLAGS=-i="C:\WATCOM\h\nt;C:\WATCOM\h" -w2 -e25 -zq -otexan -d3 -6r -bt=nt -fo=.obj -mf -br -bm 
CXXFLAGS=$(CFLAGS)

OBJDIR=objects
LIBDIR=lib
YYYLIB=$(LIBDIR)\yyymonitor.lib

all: $(YYYLIB)

.BEFORE:
	IF not exist $(OBJDIR)\ mkdir $(OBJDIR)\
	IF not exist $(LIBDIR)\ mkdir $(LIBDIR)\

!ifeq os unix
os=unix
!else ifeq os dos
os=dos
!else
os=win32
!endif

!ifeq os unix
BACKEND=pthreads
!else
BACKEND=winXP
!endif

BACKEND_SRC=yyy_monitor_$(BACKEND).c

$(OBJDIR)\yyy_monitor_backend.obj: yyy_monitor_backend.h $(BACKEND_SRC)
	$(CC) $(BACKEND_SRC) $(CFLAGS)
	move /Y yyy_monitor_$(BACKEND).obj $(OBJDIR)\yyy_monitor_backend.obj

$(OBJDIR)\yyy_monitor.obj: yyy_monitor.cpp yyy_monitor.hpp yyy_monitor_backend.h
	$(CXX) yyy_monitor.cpp $(CXXFLAGS) -c
	move /Y yyy_monitor.obj $(OBJDIR)

$(YYYLIB): $(OBJDIR)\yyy_monitor_backend.obj $(OBJDIR)\yyy_monitor.obj
    del /q /f $(YYYLIB)
	wlib -b -c $(YYYLIB) +$(OBJDIR)\yyy_monitor_backend.obj +$(OBJDIR)\yyy_monitor.obj

clean: .SYMBOLIC
	del /q /f $(OBJDIR)
    del /q /f $(LIBDIR)
