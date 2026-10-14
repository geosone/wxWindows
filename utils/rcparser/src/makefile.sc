# Symantec C++ makefile for the rcparser library

WXDIR = $(WXWIN)

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

INCLUDE=$(BASEINC);$(MSWINC)

include $(WXDIR)\src\makesc.env

CTL3DLIB = $(WXDIR)\contrib\ctl3d\ctl3dv2.lib
FAFALIB = $(WXDIR)\contrib\fafa\fafa.lib
ITSYLIB = $(WXDIR)\contrib\itsybits\itsy.lib

RCPARSEDIR = $(WXDIR)\utils\rcparser
RCPARSEINC = $(RCPARSEDIR)\src
RCPARSELIB = $(RCPARSEDIR)\lib\rcparser.lib

DOCDIR = $(RCPARSEDIR)\docs
SRCDIR = $(RCPARSEDIR)\src

OBJS = rcparser.obj rcobject.obj curico.obj

$(RCPARSELIB): $(OBJS)
 	-del $(RCPARSELIB)
	*lib $(RCPARSELIB) y $(OBJS), nul;

$(OBJS): $(RCPARSEINC)\rcparser.h $(RCPARSEINC)\rcobject.h $(RCPARSEINC)\curico.h $(RCPARSEINC)\cric_prv.h $(RCPARSEINC)\wxcurico.h

clean:
        -del *.obj
	-del $(RCPARSELIB)
