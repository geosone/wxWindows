# Symantec C++ makefile for printing example
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

printing.exe: printing.obj printing.def printing.res
	*$(CC) $(LDFLAGS) -o$@ printing.obj printing.def $(LIBS)
        *$(RC) -k printing.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws

