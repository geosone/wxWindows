# wxWindows 1.68E for 16-bit Windows (Windows 3.1x, 9x) with OpenWatcom on
# Linux.  Takes the place of src/makewat.env and the makefile.wat files
# (MODE=windows: wpp large model, wlink system windows).
#
#   make                  library build/lib/wx.lib and all samples
#   make lib              only the library
#   make samples          only the samples (build/samples/<name>/<name>.exe)
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
# makewat.env (16-bit windows): /zw /w2 /zq /d1 /ml /dwx_msw
CFLAGS  := -zq -zw -ml -bt=windows -dwx_msw -w1 -d1 $(if $(DEBUG),-d2) $(IFLAGS)

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
# a sample: samples/<name> with <name>.cpp; all its .cpp files, resources
# from <name>.rc (else the library's wx.rc)
SAMPLES := $(notdir $(patsubst %/,%,$(dir $(wildcard $(TOP)/samples/*/*.cpp))))
SAMPLES := $(sort $(foreach s,$(SAMPLES),$(if $(wildcard $(TOP)/samples/$(s)/$(s).cpp),$(s))))
# not for 16-bit Windows: ODBC
SAMPLES := $(filter-out odbc,$(SAMPLES))
WINLIBS := commdlg mmsystem shell ddeml

define SAMPLE
$(1)_SRCS := $(wildcard $(TOP)/samples/$(1)/*.cpp)
$(1)_OBJS := $$(patsubst $(TOP)/samples/$(1)/%.cpp,$(OUT)/samples/$(1)/%.obj,$$($(1)_SRCS))
$(1)_RC   := $(firstword $(wildcard $(TOP)/samples/$(1)/$(1).rc) $(TOP)/include/msw/wx.rc)
$(OUT)/samples/$(1)/%.obj: $(TOP)/samples/$(1)/%.cpp $(LIB) | $(OUT)/samples/$(1)/.dir
	cd $(TOP)/samples/$(1) && $(CXX) $(CFLAGS) -fo=$$@ $$< > $$@.log 2>&1 || { cat $$@.log; exit 1; }
$(OUT)/samples/$(1)/$(1).res: $$($(1)_RC) | $(OUT)/samples/$(1)/.dir
	cd $(TOP)/samples/$(1) && $(WRC) -q -r -bt=windows -i=$(TOP)/include/msw -i=$(TOP)/contrib/fafa -i=$(WATCOM)/h/win -fo=$$@ $$< > $$@.log 2>&1 || { cat $$@.log; exit 1; }
$(OUT)/samples/$(1)/$(1).exe: $$($(1)_OBJS) $(OUT)/samples/$(1)/$(1).res $(LIB)
	cd $(OUT)/samples/$(1) && $(WLINK) option quiet system windows $(if $(DEBUG),debug all) \
	  option stack=16k option heapsize=16k name $$@ \
	  $$(foreach o,$$($(1)_OBJS),file $$(o)) library $(LIB) $(foreach l,$(WINLIBS),library $(l)) \
	  > $$@.log 2>&1 || { cat $$@.log; exit 1; }
	$(WRC) -q $(OUT)/samples/$(1)/$(1).res $$@ > $$@.rc.log 2>&1 || { cat $$@.rc.log; exit 1; }
samples: $(OUT)/samples/$(1)/$(1).exe
endef
$(foreach s,$(SAMPLES),$(eval $(call SAMPLE,$(s))))

.PHONY: all lib samples clean
all: lib samples
.DEFAULT_GOAL := all
clean:
	rm -rf $(OUT) $(TOP)/utils/prologio/src/y_tab.c $(TOP)/utils/prologio/src/lex_yy.c
