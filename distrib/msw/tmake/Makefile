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
     $(MSW_MAKEFILES_DIR)/makefile.sc \
     $(MSW_MAKEFILES_DIR)/makefile.wat \
     $(MSW_MAKEFILES_DIR)/makefile.g95 \
     $(MSW_MAKEFILES_DIR)/makefile.g295 \
     $(WXDIR)/Makefile.in

$(MSW_MAKEFILES_DIR)/makefile.vc: filelist.txt wxwin.pro
	$(TMAKE) -t vc wxwin.pro -o $@

$(MSW_MAKEFILES_DIR)/makefile.b32: filelist.txt wxwin.pro
	$(TMAKE) -t b32 wxwin.pro -o $@

$(MSW_MAKEFILES_DIR)/makefile.dos: filelist.txt wxwin.pro
	$(TMAKE) -t dos wxwin.pro -o $@

$(MSW_MAKEFILES_DIR)/makefile.bcc: filelist.txt wxwin.pro
	$(TMAKE) -t bcc wxwin.pro -o $@

$(MSW_MAKEFILES_DIR)/makefile.sc: filelist.txt wxwin.pro
	$(TMAKE) -t sc wxwin.pro -o $@

$(MSW_MAKEFILES_DIR)/makefile.wat: filelist.txt wxwin.pro
	$(TMAKE) -t wat wxwin.pro -o $@

$(MSW_MAKEFILES_DIR)/makefile.g95: filelist.txt wxwin.pro
	$(TMAKE) -t g95 wxwin.pro -o $@

$(MSW_MAKEFILES_DIR)/makefile.g295: filelist.txt wxwin.pro g295.t
	$(TMAKE) -t g295 wxwin.pro -o $@

$(WXDIR)/src/gtk/files.lst: filelist.txt wxwin.pro
	$(TMAKE) -t gtk wxwin.pro -o $@

$(WXDIR)/src/motif/files.lst: filelist.txt wxwin.pro
	$(TMAKE) -t motif wxwin.pro -o $@

$(WXDIR)/src/os2/files.lst: filelist.txt wxwin.pro
	$(TMAKE) -t os2 wxwin.pro -o $@
