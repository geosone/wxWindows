# Symantec C++ makefile for docview example
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base
INCLUDE=$(BASEINC);$(MSWINC)

include $(WXDIR)\src\makesc.env

LIBS=$(WXLIB) libw.lib commdlg.lib shell.lib

OBJECTS=docview.obj view.obj doc.obj

docview.exe: $(OBJECTS) docview.def docview.res
	*$(CC) $(LDFLAGS) -o$@ $(OBJECTS) docview.def $(LIBS)
	*$(RC) -k docview.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
