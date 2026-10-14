# Symantec C++ makefile for the MFUTILS library
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

include $(WXDIR)\src\makesc.env

MFDIR = $(WXDIR)\utils\mfutils
MFINC = $(MFDIR)\src
MFLIB = $(MFDIR)\lib\mfutils.lib

INCLUDE=$(BASEINC);$(MSWINC)

OBJS = mfutils.obj

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

$(MFLIB): $(OBJS)
 	-del $(MFLIB)
	*lib $(MFLIB) y $(OBJS), nul;

$(OBJS): $(MFINC)\mfutils.h

clean:
        -del *.obj
	-del $(MFLIB)
