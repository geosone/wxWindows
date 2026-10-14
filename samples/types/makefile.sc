# Symantec C++ makefile for types example
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base
INCLUDE=$(BASEINC);$(MSWINC)

include $(WXDIR)\src\makesc.env

LIBS=$(WXLIB) libw.lib commdlg.lib shell.lib

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

types.exe: types.obj types.def types.res
	*$(CC) $(LDFLAGS) -o$@ $** $(LIBS)

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
