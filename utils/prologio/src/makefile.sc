# Symantec C++ makefile for the PROLOGIO library
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

INCLUDE=$(BASEINC);$(MSWINC)

include $(WXDIR)\src\makesc.env

PROLOGDIR = $(WXDIR)\utils\prologio
PROLOGINC = $(PROLOGDIR)\src
PROLOGLIB = $(PROLOGDIR)\lib\prologio.lib

DOCDIR = $(PROLOGDIR)\docs
SRCDIR = $(PROLOGDIR)\src

# Default is to output RTF for WinHelp
WINHELP=-winhelp

RC=rc

LIBS=$(PROLOGLIB) $(WXLIB) libw.lib commdlg.lib shell.lib

OBJS = read.obj y_tab.obj # prorpc.obj

.c.obj:
	*$(CC) -c $(CFLAGS) -DUSE_DEFINE -DYY_USE_PROTOS -I$(INCLUDE) $<

$(PROLOGLIB): $(OBJS)
 	-del $(PROLOGLIB)
	*lib $(PROLOGLIB) y $(OBJS), nul;

$(OBJS): $(PROLOGINC)\read.h $(PROLOGINC)\expr.h

y_tab.c:      dosyacc.c lex_yy.c
        copy dosyacc.c y_tab.c

lex_yy.c:     doslex.c
        copy doslex.c lex_yy.c

test.exe: test.obj test.def test.res
	*$(CC) $(LDFLAGS) -o$@ test.obj test.def $(LIBS)
	*$(RC) -k test.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del $(PROLOGLIB)
