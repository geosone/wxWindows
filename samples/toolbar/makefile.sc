# Symantec C++ makefile for the toolbar sample.
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base
TOOLBARDIR = $(WXDIR)\samples\toolbar
TOOLBARINC = $(TOOLBARDIR)

INCLUDE=$(BASEINC);$(MSWINC)
include $(WXDIR)\src\makesc.env

LIBS=$(WXLIB) libw.lib commdlg.lib shell.lib

all:    test.exe

test.exe: test.obj test.def test.res
	*$(CC) $(LDFLAGS) -o$@ test.obj test.def $(LIBS)
	*$(RC) -k test.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map

