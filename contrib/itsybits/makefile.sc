# Makefile : Builds ITSYBITS library
# for Symantec C++

# Set WXDIR for your system
WXDIR = $(WXWIN)

WXLIB = $(WXDIR)\lib\wx.lib
ITSYDIR = $(WXDIR)\contrib\itsybits
ITSYLIB = $(ITSYDIR)\itsy.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

INCLUDE=$(BASEINC);$(MSWINC);$(WXDIR)\contrib\itsybits

include $(WXDIR)\src\makesc.env


OBJS = itsybits.obj

all: $(ITSYLIB)

$(ITSYLIB): $(OBJS)
        -del $(ITSYLIB)
	*lib $(ITSYLIB) y $(OBJS), nul;

itsybits.obj: itsybits.h itsybits.$(SRCSUFF)

clean:
        -del *.obj
	-del $(ITSYLIB)
