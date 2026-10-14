# Makefile : Builds GNU wxString library
# for Symantec C++

# Set WXDIR for your system
WXDIR = $(WXWIN)

WXLIB = $(WXDIR)\lib\wx.lib
STRINGDIR = $(WXDIR)\contrib\wxstring
STRINGLIB = $(STRINGDIR)\wxstring.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

INCLUDE=$(BASEINC);$(MSWINC);$(WXDIR)\contrib\wxstring

include $(WXDIR)\src\makesc.env

OBJS = wxstrgnu.obj # wxregex.obj # Causes internal error in SC

all: $(STRINGLIB)

$(STRINGLIB): $(OBJS)
        -del $(STRINGLIB)
	*lib $(STRINGLIB) y $(OBJS), nul;

wxstrgnu.obj: wxstrgnu.cpp wxstrgnu.h

wxregex.obj: wxregex.cpp wxregex.h

clean:
        -del *.obj
	-del $(STRINGLIB)
