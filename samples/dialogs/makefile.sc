# Symantec C++ makefile for dialogs example
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base
INCLUDE=$(BASEINC);$(MSWINC)

include $(WXDIR)\src\makesc.env

LIBS=$(WXLIB) libw.lib commdlg.lib shell.lib

dialogs.exe: dialogs.obj dialogs.def dialogs.res
	*$(CC) $(LDFLAGS) -o$@ $** $(LIBS)

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
