# Makefile : Builds Gauge library
# for Symantec C++

# Set WXDIR for your system
WXDIR = $(WXWIN)
WXLIB = $(WXDIR)\lib\wx.lib
GAUGEDIR = $(WXDIR)\contrib\gauge
GAUGELIB = $(GAUGEDIR)\gauge.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

INCLUDE=$(BASEINC);$(MSWINC);$(WXDIR)\contrib\gauge

include $(WXDIR)\src\makesc.env


OBJS = zyz3d.obj zyzgauge.obj

.c.obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

all: $(GAUGELIB)

$(GAUGELIB): $(OBJS)
        -del $(GAUGELIB)
	*lib $(GAUGELIB) y $(OBJS), nul;

zyz3d.obj: zyz3d.h zyz3d.c

zyzgauge.obj: zyz3d.h zyzgauge.h zyzgauge.c

clean:
        -del *.obj
	-del $(GAUGELIB)
