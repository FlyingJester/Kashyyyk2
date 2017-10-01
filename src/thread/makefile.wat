# OpenWatcom makefile
# Builds yyythread

CC=wcc386
LINKER=wlink

CFLAGS=-i="C:\WATCOM\h;C:\WATCOM\h\nt" -w2 -e25 -zq -otexan -d2 -6r -bt=nt -fo=.obj -mf -br -bm 

OBJDIR=objects
LIBDIR=lib
YYYLIB=$(LIBDIR)\yyythread.lib

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
BACKEND=win32
!endif

BACKEND_SRC=yyy_thread_$(BACKEND).c

$(OBJDIR)\yyy_thread.obj: $(BACKEND_SRC) yyy_thread.h
	$(CC) $(BACKEND_SRC) $(CFLAGS)
	move /Y yyy_thread_$(BACKEND).obj $(OBJDIR)\yyy_thread.obj

$(YYYLIB): $(OBJDIR)\yyy_thread.obj
	del /q /f $(YYYLIB)
	wlib -q -b -c $(YYYLIB) +$(OBJDIR)\yyy_thread.obj

clean: .SYMBOLIC
	del /q /f $(YYYLIB)
	del /q /f $(OBJDIR)\yyy_thread.obj

