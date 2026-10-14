# wxWindows 1.68E for 16-bit Windows (Windows 3.1x, 9x) with OpenWatcom on
# Linux.  Takes the place of src/makewat.env and the makefile.wat files
# (MODE=windows: wpp large model, wlink system windows).
#
#   make                  library build/lib/wx.lib and all samples
#   make lib              only the library
#   make samples          only the samples (build/samples/<name>/<name>.exe)
#   make dll              build/dll/WX.DLL, import library build/lib/wxdll.lib
#                         and build/lib/wxdllmain.obj (WinMain, new/delete)
#   make samples-dll      the samples against WX.DLL (build/samples-dll)
#   make clean
#
# Variables: WATCOM (default /opt/watcom), DEBUG=1 (debug all)

WATCOM  ?= /opt/watcom
export WATCOM
export INCLUDE := $(WATCOM)/h:$(WATCOM)/h/win
BIN     := $(WATCOM)/binl64
CXX     := $(BIN)/wpp
CC      := $(BIN)/wcc
WLIB    := $(BIN)/wlib
WLINK   := $(BIN)/wlink
WRC     := $(BIN)/wrc

TOP     := $(abspath .)
OUT     := $(TOP)/build
OBJ     := $(OUT)/obj
LIB     := $(OUT)/lib/wx.lib

IFLAGS  := -i=$(TOP)/include/msw -i=$(TOP)/include/base -i=$(TOP)/contrib/fafa \
           -i=$(WATCOM)/h -i=$(WATCOM)/h/win
# makewat.env (16-bit windows): /zw /w2 /zq /d1 /ml /dwx_msw; -zt256: larger
# data outside DGROUP (makewat.env mentions /zt4: DGROUP overflow)
CFLAGS  := -zq -fr -zw -ml -zt256 -bt=windows -dwx_msw -w1 -d1 $(if $(DEBUG),-d2) $(IFLAGS)

# --- library ------------------------------------------------------------
# src/msw/makefile.wat: OBJECTS, BASEOBJECTS, EXTRAMODULES
MSW     := wx_win wx_frame wx_panel wx_utils wx_item wx_text wx_gdi wx_dialg \
           wx_canvs wx_dc wx_mf wx_ipc wx_timer wx_clipb wx_vlbox wx_stat \
           wx_scrol wx_buttn wx_messg wx_check wx_choic wx_rbox wx_lbox \
           wx_group wx_gauge wx_txt wx_mtxt wx_slidr wx_menu wx_db wx_cmdlg \
           wx_main wx_combo
BASE    := wb_win wb_frame wb_panel wb_utils wx_lay wx_doc wb_res wb_main \
           wb_item wb_list wb_obj wb_text wb_gdi wb_dialg wb_canvs wb_dc wb_mf \
           wb_ps wx_enhdg wb_hash wb_ipc wb_form wb_timer wb_help wb_sysev \
           wb_stdev wb_types wb_mgstr wb_data wb_stat wb_scrol wb_vlbox \
           wb_print wx_tbar wx_bbar wx_mem wx_date wb_cmdlg wx_time wx_frac \
           wx_split
# module directory : objects (.c or .cpp sources)
MODULES := contrib/fafa:button,check,cont,dialog,draw,fafa,static \
           contrib/gauge:zyz3d,zyzgauge \
           contrib/itsybits:itsybits \
           contrib/wxstring:wxstrgnu,wxregex \
           utils/prologio/src:y_tab,read \
           utils/dib:dib \
           utils/rcparser/src:curico,rcobject,rcparser

LIBOBJS := $(addprefix $(OBJ)/src/msw/,$(addsuffix .obj,$(MSW))) \
           $(addprefix $(OBJ)/src/base/,$(addsuffix .obj,$(BASE)))
define MODULE
LIBOBJS += $(addprefix $(OBJ)/$(1)/,$(addsuffix .obj,$(subst $(comma), ,$(2))))
$(OBJ)/$(1)/%.obj: $(TOP)/$(1)/%.cpp | $(OBJ)/$(1)/.dir
	cd $(TOP)/$(1) && $(CXX) $(CFLAGS) -fo=$$@ $$< > $$@.log 2>&1 || { cat $$@.log; exit 1; }
$(OBJ)/$(1)/%.obj: $(TOP)/$(1)/%.c | $(OBJ)/$(1)/.dir
	cd $(TOP)/$(1) && $(CC) $(CFLAGS) -fo=$$@ $$< > $$@.log 2>&1 || { cat $$@.log; exit 1; }
endef
comma := ,
$(foreach m,$(MODULES),$(eval $(call MODULE,$(word 1,$(subst :, ,$(m))),$(word 2,$(subst :, ,$(m))))))
$(eval $(call MODULE,src/msw,))
$(eval $(call MODULE,src/base,))

# prologio: the DOS parser sources under the names makefile.wat uses
$(TOP)/utils/prologio/src/y_tab.c: $(TOP)/utils/prologio/src/dosyacc.c
	cp $< $@
$(TOP)/utils/prologio/src/lex_yy.c: $(TOP)/utils/prologio/src/doslex.c
	cp $< $@
$(OBJ)/utils/prologio/src/y_tab.obj: $(TOP)/utils/prologio/src/lex_yy.c

%/.dir:
	mkdir -p $(@D) && touch $@

lib: $(LIB)
$(LIB): $(LIBOBJS) | $(OUT)/lib/.dir
	rm -f $@
	$(WLIB) -q -b -c -n -p=512 $@ $(addprefix +,$(LIBOBJS))

# --- samples ------------------------------------------------------------
# samples.lst: program, directory, resource script, sources
WINLIBS := commdlg mmsystem shell ddeml ctl3dv2
SAMPLELINES := $(shell sed -e 's/\#.*//' -e '/^ *$$/d' -e 's/  */:/g' $(TOP)/samples.lst)

define SAMPLE
$(1)_OBJS := $(addprefix $(OUT)/samples/$(2)/,$(addsuffix .obj,$(4)))
$(OUT)/samples/$(2)/%.obj: $(TOP)/samples/$(2)/%.cpp $(LIB) | $(OUT)/samples/$(2)/.dir
	cd $(TOP)/samples/$(2) && $(CXX) $(CFLAGS) -fo=$$@ $$< > $$@.log 2>&1 || { cat $$@.log; exit 1; }
$(OUT)/samples/$(2)/$(1).res: $(TOP)/samples/$(2)/$(3) $(TOP)/include/msw/wx.rc | $(OUT)/samples/$(2)/.dir
	cd $(TOP)/samples/$(2) && $(WRC) -q -r -bt=windows -i=$(TOP)/include/msw -i=$(TOP)/contrib/fafa -i=$(WATCOM)/h/win -fo=$$@ $(3) > $$@.log 2>&1 || { cat $$@.log; exit 1; }
$(OUT)/samples/$(2)/$(1).exe: $$($(1)_OBJS) $(OUT)/samples/$(2)/$(1).res $(LIB)
	cd $(OUT)/samples/$(2) && $(WLINK) option quiet format windows $(if $(DEBUG),debug all) name $$@ \
	  libpath $(WATCOM)/lib286/win libpath $(WATCOM)/lib286 library windows \
	  $$(foreach o,$$($(1)_OBJS),file $$(o)) library $(LIB) $(foreach l,$(WINLIBS),library $(l)) \
	  library $(WATCOM)/lib286/win/plibl.lib \
	  option stack=16k option heapsize=16k option version=3.10 \
	  option resource=$(OUT)/samples/$(2)/$(1).res option map=$(1).map \
	  > $$@.log 2>&1 || { cat $$@.log; exit 1; }
samples: $(OUT)/samples/$(2)/$(1).exe
endef
$(foreach l,$(SAMPLELINES),$(eval $(call SAMPLE,$(word 1,$(subst :, ,$(l))),$(word 2,$(subst :, ,$(l))),$(word 3,$(subst :, ,$(l))),$(wordlist 4,99,$(subst :, ,$(l))))))

# --- DLL --------------------------------------------------------------
# the library objects once more: -bd (DLL), -zu (SS != DS), -s (the stack
# check would run before DS is loaded), owdll.h (every function loads DS)
DOBJ    := $(OUT)/dllobj
DLLFLAGS := $(CFLAGS) -bd -zu -s -fi=$(TOP)/src/msw/owdll.h -dWXMAKINGDLL
DLLOBJS := $(patsubst $(OBJ)/%,$(DOBJ)/%,$(LIBOBJS))
DLLWINLIBS := commdlg mmsystem shell ddeml ctl3dv2
define DMODULE
$(DOBJ)/$(1)/%.obj: $(TOP)/$(1)/%.cpp | $(DOBJ)/$(1)/.dir
	cd $(TOP)/$(1) && $(CXX) $(DLLFLAGS) -fo=$$@ $$< > $$@.log 2>&1 || { cat $$@.log; exit 1; }
$(DOBJ)/$(1)/%.obj: $(TOP)/$(1)/%.c | $(DOBJ)/$(1)/.dir
	cd $(TOP)/$(1) && $(CC) $(DLLFLAGS) -fo=$$@ $$< > $$@.log 2>&1 || { cat $$@.log; exit 1; }
endef
$(foreach m,$(MODULES),$(eval $(call DMODULE,$(word 1,$(subst :, ,$(m))))))
$(eval $(call DMODULE,src/msw))
$(eval $(call DMODULE,src/base))
$(DOBJ)/utils/prologio/src/y_tab.obj: $(TOP)/utils/prologio/src/lex_yy.c

DLL     := $(OUT)/dll/WX.DLL
DLLIMP  := $(OUT)/lib/wxdll.lib
DLLMAIN := $(OUT)/lib/wxdllmain.obj
dll: $(DLL) $(DLLIMP) $(DLLMAIN)
$(OUT)/dll/exports.lnk: $(DLLOBJS) $(TOP)/tools/wxdll.py | $(OUT)/dll/.dir
	python3 $(TOP)/tools/wxdll.py exports WX $@ $(DLLOBJS)
$(DLL): $(DLLOBJS) $(OUT)/dll/exports.lnk
	cd $(OUT)/dll && $(WLINK) option quiet format windows dll name $@ \
	  libpath $(WATCOM)/lib286/win libpath $(WATCOM)/lib286 library windows \
	  file $(WATCOM)/lib286/win/libentry.obj $(foreach o,$(DLLOBJS),file $(o)) \
	  $(foreach l,$(DLLWINLIBS),library $(l)) library $(WATCOM)/lib286/win/plibl.lib \
	  option heapsize=16k option version=3.10 option map=WX.map @exports.lnk \
	  > $@.log 2>&1 || { cat $@.log; exit 1; }
$(DLLIMP): $(DLL) $(OUT)/dll/exports.lnk | $(OUT)/lib/.dir
	python3 $(TOP)/tools/wxdll.py implib WX $(OUT)/dll/exports.lnk $(OUT)/dll/implib.cmd
	rm -f $@ && $(WLIB) -q -n $@ @$(OUT)/dll/implib.cmd
# a program using the DLL: -dWXUSINGDLL; its functions load DS too
# (owdll.h), because the DLL calls them (virtual functions, callbacks) with
# the DLL's DS - so one instance per program (dummy.cpp's WinMain), the
# code would load the first instance's DGROUP; -s: no stack check before
# DS is loaded.  wxdllmain.obj (WinMain, new/delete) is linked as a file.
CLIENTFLAGS := $(filter-out -zw,$(CFLAGS)) -s -fi=$(TOP)/src/msw/owdll.h -dWXUSINGDLL
$(DLLMAIN): $(TOP)/src/msw/dummy.cpp | $(OUT)/lib/.dir
	cd $(TOP)/src/msw && $(CXX) $(CLIENTFLAGS) -fo=$@ dummy.cpp > $@.log 2>&1 || { cat $@.log; exit 1; }

define DSAMPLE
$(1)_DOBJS := $(addprefix $(OUT)/samples-dll/$(2)/,$(addsuffix .obj,$(4)))
$(OUT)/samples-dll/$(2)/%.obj: $(TOP)/samples/$(2)/%.cpp $(DLLIMP) | $(OUT)/samples-dll/$(2)/.dir
	cd $(TOP)/samples/$(2) && $(CXX) $(CLIENTFLAGS) -fo=$$@ $$< > $$@.log 2>&1 || { cat $$@.log; exit 1; }
$(OUT)/samples-dll/$(2)/$(1).exe: $$($(1)_DOBJS) $(OUT)/samples/$(2)/$(1).res $(DLLIMP) $(DLLMAIN)
	cd $(OUT)/samples-dll/$(2) && $(WLINK) option quiet format windows name $$@ \
	  libpath $(WATCOM)/lib286/win libpath $(WATCOM)/lib286 library windows \
	  file $(DLLMAIN) $$(foreach o,$$($(1)_DOBJS),file $$(o)) library $(DLLIMP) \
	  $(foreach l,$(WINLIBS),library $(l)) library $(WATCOM)/lib286/win/plibl.lib \
	  option stack=16k option heapsize=4k option version=3.10 \
	  option resource=$(OUT)/samples/$(2)/$(1).res option map=$(1).map \
	  > $$@.log 2>&1 || { cat $$@.log; exit 1; }
samples-dll: $(OUT)/samples-dll/$(2)/$(1).exe
endef
$(foreach l,$(SAMPLELINES),$(eval $(call DSAMPLE,$(word 1,$(subst :, ,$(l))),$(word 2,$(subst :, ,$(l))),$(word 3,$(subst :, ,$(l))),$(wordlist 4,99,$(subst :, ,$(l))))))

.PHONY: all lib samples dll samples-dll clean
all: lib samples
.DEFAULT_GOAL := all
clean:
	rm -rf $(OUT) $(TOP)/utils/prologio/src/y_tab.c $(TOP)/utils/prologio/src/lex_yy.c
