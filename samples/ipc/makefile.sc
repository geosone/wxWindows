# Symantec C++ makefile for ipc example
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base
INCLUDE=$(BASEINC);$(MSWINC)

include $(WXDIR)\src\makesc.env

LIBS=$(WXLIB) libw.lib commdlg.lib ddeml.lib shell.lib

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

all: client.exe server.exe

client.exe: client.obj client.def client.res
	*$(CC) $(LDFLAGS) -o$@ client.obj client.def $(LIBS)
	*$(RC) -k client.res

server.exe: server.obj server.def server.res
	*$(CC) $(LDFLAGS) -o$@ server.obj server.def $(LIBS)
	*$(RC) -k server.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
