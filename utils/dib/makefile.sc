# Symantec C++ makefile for the DIB library

WXDIR = $(WXWIN)

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base
INCLUDE=$(BASEINC);$(MSWINC)

include $(WXDIR)\src\makesc.env

DIBDIR = $(WXDIR)\utils\dib
DIBINC = $(DIBDIR)
DIBLIB = $(DIBDIR)\dib.lib

OBJS = dib.obj

$(DIBLIB): $(OBJS)
 	-del $(DIBLIB)
	*lib $(DIBLIB) y $(OBJS), nul;

$(OBJS): $(DIBINC)\dib.h

clean:
        -del *.obj
	-del $(DIBLIB)
