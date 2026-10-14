# Makefile : Builds wxWindows library wx.lib for Windows 3.1
# for Symantec C++ 6.0
# THIS MAY NEED ALTERING FOR wxWin 1.61: specifically
# checking my peripheral library stuff works (haven't
# checked the syntax). -- JACS Jan '95

### system configuration section ##########################################

# directories:

WXDIR = $(WXWIN)
SRCDIR = $(WXDIR)\src
INCDIR = $(WXDIR)\include
LIBDIR = $(WXDIR)\lib
DOCDIR = $(WXDIR)\docs
MSWSRC = $(SRCDIR)\msw
MSWINC = $(INCDIR)\msw
BASESRC = $(SRCDIR)\base
BASEINC = $(INCDIR)\base

DEBUG=0

LIBTARGET = $(LIBDIR)\wx.lib

# Miscellaneous wxWindows source options. Hopefully we can keep these
# options settable in wxWindows library compilation only, and not
# have to set them in each application's makefile. I.e. keep these
# #ifdefs in source files only, not header files.
#
# Zero or more of:
# -DCTL3D          (CTL3D support; see docs\install.txt for instructions)
# -DNO_COPYRIGHT   (no non-AIAI code) // Not implemented
# -DFAFA_LIB       (Contributed bitmap/new look control stuff; requires
#                  that you compile FAFA lib too)
# -DENHANCED_FONTS (Define to have predefined fonts in wxEhDialogBox)

# OPTIONS = -DCTL3D
# OPTIONS = -DFAFA_LIB
OPTIONS =

# Default is to output RTF for WinHelp
WINHELP = -winhelp

# Please set these according to the settings in wx_setup.h, so we can include
# the appropriate libraries in wx.lib
# NOT TESTED YET - JACS
USE_CTL3D=1
USE_FAFA=1
USE_ITSYBITS=1
USE_GAUGE=1
USE_IMAGE_LOADING_IN_MSW=1
USE_XPM_IN_MSW=1
USE_WX_RESOURCES=1
USE_RESOURCE_LOADING_IN_MSW=1

PERIPH_LIBS=
PERIPH_TARGET=
PERIPH_CLEAN=

# This one is for WIN16 only
#PERIPH_LIBS=$(WXDIR)\contrib\ctl3d\msvc\ctl3dv2.lib $(PERIPH_LIBS)
#PERIPH_TARGET=$(PERIPH_TARGET)

PERIPH_LIBS=$(WXDIR)\contrib\fafa\fafa.lib $(PERIPH_LIBS)
PERIPH_CLEAN=fafa_clean $(PERIPH_CLEAN)
PERIPH_TARGET=fafa $(PERIPH_TARGET)

PERIPH_LIBS=$(WXDIR)\contrib\itsybits\itsy.lib $(PERIPH_LIBS)
PERIPH_CLEAN=itsy_clean $(PERIPH_CLEAN)
PERIPH_TARGET=itsy $(PERIPH_TARGET)

PERIPH_LIBS=$(WXDIR)\contrib\gauge\gauge.lib $(PERIPH_LIBS)
PERIPH_CLEAN=gauge_clean $(PERIPH_CLEAN)
PERIPH_TARGET=gauge $(PERIPH_TARGET)

# PERIPH_LIBS=$(WXDIR)\contrib\wxxpm\xpm.lib $(PERIPH_LIBS)
# PERIPH_CLEAN=xpm_clean $(PERIPH_CLEAN)
# PERIPH_TARGET=xpm $(PERIPH_TARGET)

PERIPH_LIBS=$(WXDIR)\utils\dib\dib.lib $(PERIPH_LIBS)
PERIPH_CLEAN=dib_clean $(PERIPH_CLEAN)
PERIPH_TARGET=dib $(PERIPH_TARGET)

PERIPH_LIBS=$(WXDIR)\utils\prologio\lib\prologio.lib $(PERIPH_LIBS)
PERIPH_CLEAN=prologio_clean $(PERIPH_CLEAN)
PERIPH_TARGET=prologio $(PERIPH_TARGET)

PERIPH_LIBS=$(WXDIR)\utils\rcparser\lib\rcparser.lib $(PERIPH_LIBS)
PERIPH_CLEAN=rcparser_clean $(PERIPH_CLEAN)
PERIPH_TARGET=rcparser $(PERIPH_TARGET)

# PERIPH_LIBS=$(WXDIR)\contrib\wxstring\wxstring.lib $(PERIPH_LIBS)
# PERIPH_CLEAN=wxstring_clean $(PERIPH_CLEAN)
# PERIPH_TARGET=wxstring $(PERIPH_TARGET)

### end of system configuration section ###################################

MSWOBJS = msw\wx_win.obj msw\wx_frame.obj msw\wx_panel.obj \
msw\wx_utils.obj msw\wx_main.obj msw\wx_item.obj msw\wx_text.obj \
msw\wx_gdi.obj msw\wx_dialg.obj msw\wx_canvs.obj msw\wx_dc.obj \
msw\wx_mf.obj msw\wx_ipc.obj msw\wx_timer.obj msw\wx_clipb.obj \
msw\wx_buttn.obj msw\wx_messg.obj msw\wx_check.obj msw\wx_choic.obj msw\wx_rbox.obj msw\wx_lbox.obj \
msw\wx_group.obj msw\wx_gauge.obj msw\wx_txt.obj msw\wx_mtxt.obj msw\wx_slidr.obj msw\wx_menu.obj \
msw\wx_vlbox.obj msw\wx_stat.obj msw\wx_scrol.obj msw\wx_db.obj msw\wx_cmdlg.obj msw\wx_combo.obj

BASEOBJS = base\wb_win.obj base\wb_data.obj base\wb_frame.obj base\wb_panel.obj \
base\wb_utils.obj base\wb_main.obj  base\wb_item.obj base\wb_list.obj \
base\wb_obj.obj base\wb_ps.obj base\wb_text.obj base\wb_gdi.obj \
base\wb_dialg.obj base\wb_canvs.obj base\wb_dc.obj base\wb_mf.obj \
base\wb_hash.obj base\wb_ipc.obj  base\wb_form.obj base\wb_timer.obj \
base\wb_help.obj base\wx_enhdg.obj base\wb_sysev.obj  base\wb_stdev.obj \
base\wb_types.obj base\wb_mgstr.obj base\wb_res.obj base\wx_lay.obj base\wx_doc.obj \
base\wb_scrol.obj base\wb_stat.obj base\wb_vlbox.obj base\wb_print.obj\
base\wx_tbar.obj base\wx_bbar.obj base\wx_mem.obj base\wb_cmdlg.obj\
base\wx_date.obj base\wx_time.obj base\wx_frac.obj base\wx_split.obj base\wxstring.obj

OBJS = $(MSWOBJS) $(BASEOBJS) 


all:    $(PERIPH_TARGET) base msw $(LIBTARGET)

base:
	cd $(BASESRC)
	*make -f makefile.sc
	cd $(SRCDIR)

msw:
	cd $(MSWSRC)
	*make -f makefile.sc
	cd $(SRCDIR)

$(LIBTARGET): $(OBJS)
	-del $(LIBTARGET)
	*lib /PAGESIZE:256 $(LIBTARGET) y $(OBJS) $(PERIPH_LIBS), nul;

# Making documents
docs:   hlp
hlp:    $(DOCDIR)/wx.hlp
rtf:    $(DOCDIR)/wx.rtf

alldocs: allhlp
allhlp:
	make -f makefile.sc hlp
	cd $(WXDIR)\utils\toolbar\src
	make -f makefile.sc 'WINHELP=$(WINHELP)' hlp
	cd $(WXDIR)\utils\hytext\src
	make -f makefile.sc 'WINHELP=$(WINHELP)' hlp
	cd $(WXDIR)\utils\wxclips\src
	make -f makefile.sc 'WINHELP=$(WINHELP)' hlp
	cd $(WXDIR)\utils\wxhelp\src
	make -f makefile.sc 'WINHELP=$(WINHELP)' hlp
	cd $(WXDIR)\utils\prologio\src
	make -f makefile.sc 'WINHELP=$(WINHELP)' hlp
	cd $(WXDIR)\utils\tex2rtf\src
	make -f makefile.sc 'WINHELP=$(WINHELP)' hlp
	cd $(WXDIR)\utils\wxgraph\src
	make -f makefile.sc 'WINHELP=$(WINHELP)' hlp
	cd $(WXDIR)\utils\wxtree\src
	make -f makefile.sc 'WINHELP=$(WINHELP)' hlp
	cd $(SRCDIR)

$(DOCDIR)/wx.hlp:         $(DOCDIR)/wx.rtf $(DOCDIR)/wx.hpj
	cd $(DOCDIR)
	-erase wx.ph
	hc wx
	cd $(SRCDIR)

$(DOCDIR)/wx.rtf:         $(DOCDIR)/classes.tex $(DOCDIR)/body.tex $(DOCDIR)/manual.tex
	cd $(DOCDIR)
	tex2rtf $(DOCDIR)/manual.tex $(DOCDIR)/wx.rtf -twice $(WINHELP)
	cd $(SRCDIR)

# Peripheral components

fafa:
    cd $(WXDIR)\contrib\fafa
    *make -f makefile.sc
    cd $(WXDIR)\src\msw

fafa_clean:
    cd $(WXDIR)\contrib\fafa
    *make -f makefile.sc clean
    cd $(WXDIR)\src\msw

itsy:
    cd $(WXDIR)\contrib\itsybits
    *make -f makefile.sc
    cd $(WXDIR)\src\msw

itsy_clean:
    cd $(WXDIR)\contrib\itsybits
    *make -f makefile.sc clean
    cd $(WXDIR)\src\msw

gauge:
    cd $(WXDIR)\contrib\gauge
    *make -f makefile.sc
    cd $(WXDIR)\src\msw

gauge_clean:
    cd $(WXDIR)\contrib\gauge
    *make -f makefile.sc clean
    cd $(WXDIR)\src\msw

xpm:
    cd $(WXDIR)\contrib\wxxpm
    *make -f makefile.sc
    cd $(WXDIR)\src\msw

xpm_clean:
    cd $(WXDIR)\contrib\wxxpm
    *make -f makefile.sc clean
    cd $(WXDIR)\src\msw

dib:
    cd $(WXDIR)\utils\dib
    *make -f makefile.sc 'DEBUG=$(DEBUG)'
    cd $(WXDIR)\src\msw

dib_clean:
    cd $(WXDIR)\utils\dib
    *make -f makefile.sc clean
    cd $(WXDIR)\src\msw

prologio:
    cd $(WXDIR)\utils\prologio\src
    *make -f makefile.sc 'DEBUG=$(DEBUG)'
    cd $(WXDIR)\src\msw

prologio_clean:
    cd $(WXDIR)\utils\prologio\src
    *make -f makefile.sc clean
    cd $(WXDIR)\src\msw

rcparser:
    cd $(WXDIR)\utils\rcparser\src
    *make -f makefile.sc 'DEBUG=$(DEBUG)'
    cd $(WXDIR)\src\msw

rcparser_clean:
    cd $(WXDIR)\utils\rcparser\src
    *make -f makefile.sc clean
    cd $(WXDIR)\src\msw

wxstring:
    cd $(WXDIR)\contrib\wxstring
    *make -f makefile.sc 'DEBUG=$(DEBUG)'
    cd $(WXDIR)\src\msw

wxstring_clean:
    cd $(WXDIR)\contrib\wxstring
    *make -f makefile.sc clean
    cd $(WXDIR)\src\msw

clean: $(PERIPH_CLEAN) cleancore

cleancore:
	cd $(BASESRC)
	make -f makefile.sc clean
	cd $(MSWSRC)
	make -f makefile.sc clean
	-del $(LIBTARGET)
	cd $(SRCDIR)

