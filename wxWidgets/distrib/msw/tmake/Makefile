# FIXME we'll generate this makefile with configure later, but for now please
#       change it manually
#TMAKEDIR=/home/zeitlin/build/tmake
#TMAKE=$(TMAKEDIR)/bin/tmake
TMAKE=tmake

WXDIR=../../..
MSW_MAKEFILES_DIR=$(WXDIR)/src/msw

all: $(MSW_MAKEFILES_DIR)/makefile.vc \
     $(MSW_MAKEFILES_DIR)/makefile.b32 \
     $(MSW_MAKEFILES_DIR)/makefile.dos \
     $(MSW_MAKEFILES_DIR)/makefile.bcc \
     $(MSW_MAKEFILES_DIR)/makefile.wat \
     $(MSW_MAKEFILES_DIR)/makefile.g95 \
     $(WXDIR)/Makefile.in \
     $(WXDIR)/src/files.lst \
     $(WXDIR)/src/msw/files.lst \
     $(WXDIR)/src/gtk/files.lst \
     $(WXDIR)/src/mac/files.lst \
     $(WXDIR)/src/cocoa/files.lst \
     $(WXDIR)/src/motif/files.lst \
     $(WXDIR)/src/os2/files.lst \
     $(WXDIR)/src/mgl/files.lst \
     $(WXDIR)/src/mgl/makefile.wat \
     $(WXDIR)/src/microwin/files.lst \
     $(WXDIR)/src/x11/files.lst \
     $(WXDIR)/src/univ/files.lst \
     $(WXDIR)/src/wxBase.dsp \
     $(WXDIR)/src/wxWindows.dsp \
     $(WXDIR)/src/wxUniv.dsp

$(MSW_MAKEFILES_DIR)/makefile.vc: vc.t filelist.txt wxwin.pro
	$(TMAKE) -t vc wxwin.pro -o $@

$(MSW_MAKEFILES_DIR)/makefile.b32: b32.t filelist.txt wxwin.pro
	$(TMAKE) -t b32 wxwin.pro -o $@

$(MSW_MAKEFILES_DIR)/makefile.dos: dos.t filelist.txt wxwin.pro
	$(TMAKE) -t dos wxwin.pro -o $@

$(MSW_MAKEFILES_DIR)/makefile.bcc: bcc.t filelist.txt wxwin.pro
	$(TMAKE) -t bcc wxwin.pro -o $@

$(MSW_MAKEFILES_DIR)/makefile.wat: wat.t filelist.txt wxwin.pro
	$(TMAKE) -t wat wxwin.pro -o $@

$(MSW_MAKEFILES_DIR)/makefile.g95: g95.t filelist.txt wxwin.pro
	$(TMAKE) -t g95 wxwin.pro -o $@

$(WXDIR)/src/files.lst: base.t filelist.txt wxwin.pro
	$(TMAKE) -t base wxwin.pro -o $@

$(WXDIR)/src/msw/files.lst: msw.t filelist.txt wxwin.pro
	$(TMAKE) -t msw wxwin.pro -o $@

$(WXDIR)/src/mgl/files.lst: mgl.t filelist.txt wxwin.pro
	$(TMAKE) -t mgl wxwin.pro -o $@

$(WXDIR)/src/mgl/makefile.wat: watmgl.t filelist.txt wxwin.pro
	$(TMAKE) -t watmgl wxwin.pro -o $@

$(WXDIR)/src/microwin/files.lst: micro.t filelist.txt wxwin.pro
	$(TMAKE) -t micro wxwin.pro -o $@

$(WXDIR)/src/gtk/files.lst: gtk.t filelist.txt wxwin.pro
	$(TMAKE) -t gtk wxwin.pro -o $@

$(WXDIR)/src/mac/files.lst: mac.t filelist.txt wxwin.pro
	$(TMAKE) -t mac wxwin.pro -o $@

$(WXDIR)/src/cocoa/files.lst: cocoa.t filelist.txt wxwin.pro
	$(TMAKE) -t cocoa wxwin.pro -o $@

$(WXDIR)/src/motif/files.lst: motif.t filelist.txt wxwin.pro
	$(TMAKE) -t motif wxwin.pro -o $@

$(WXDIR)/src/os2/files.lst: os2.t filelist.txt wxwin.pro
	$(TMAKE) -t os2 wxwin.pro -o $@

$(WXDIR)/src/univ/files.lst: univ.t filelist.txt wxwin.pro
	$(TMAKE) -t univ wxwin.pro -o $@

$(WXDIR)/src/x11/files.lst: x11.t filelist.txt wxwin.pro
	$(TMAKE) -t x11 wxwin.pro -o $@

$(WXDIR)/src/wxBase.dsp: vc6base.t filelist.txt wxwin.pro
	WXWIN=$(WXDIR) $(TMAKE) -t vc6base wxwin.pro -o $@

$(WXDIR)/src/wxWindows.dsp: vc6msw.t filelist.txt wxwin.pro
	WXWIN=$(WXDIR) $(TMAKE) -t vc6msw wxwin.pro -o $@

$(WXDIR)/src/wxUniv.dsp: vc6univ.t filelist.txt wxwin.pro
	$(TMAKE) -t vc6univ wxwin.pro -o $@
