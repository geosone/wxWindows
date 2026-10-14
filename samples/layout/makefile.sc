# Symantec C++ makefile for layout example
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base
INCLUDE=$(BASEINC);$(MSWINC)

include $(WXDIR)\src\makesc.env

LIBS=$(WXLIB) libw.lib commdlg.lib shell.lib

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

layout.exe: layout.obj layout.def layout.res
	*$(CC) $(LDFLAGS) -o$@ layout.obj layout.def $(LIBS)
        *$(RC) -k layout.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws

