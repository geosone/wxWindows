# Symantec C++ makefile for fractal example
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base
INCLUDE=$(BASEINC);$(MSWINC)

!include $(WXDIR)\src\makesc.env

LIBS=$(WXLIB) libw.lib commdlg.lib shell.lib

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

fractal.exe: fractal.obj fractal.def fractal.res
	*$(CC) $(LDFLAGS) -o$@ fractal.obj fractal.def $(LIBS)
        *$(RC) -k fractal.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws

