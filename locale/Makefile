# this is the makefile for generating wxstd.po message catalog file and
# building lang.mo files from the translated lang.po catalogs

# this makefile may be invoked to build either wxstd.po or any lang.mo

# Autodetect the languages we support.  Currently this relies on make
# being called with this dir as the cwd, but if we generate this file
# with configure an explicit path should be specified -- RL.

WX_LINGUAS := $(shell ls *.po */*.po 2> /dev/null | sed 's/wxstd.po//g' | sed 's/.po//g')
WX_LINGUAS_UPDATE := $(shell ls *.po 2> /dev/null | sed 's/wxstd.po//g' | sed 's/.po//g')

# the programs we use (TODO: use configure to detect them)
MSGFMT=msgfmt --verbose
MSGMERGE=msgmerge
XGETTEXT=xgettext
XARGS=xargs

# common xgettext args: C++ syntax, use the specified macro names as markers
XGETTEXT_ARGS=-C -k_ -kwxPLURAL:1,2 -kwxTRANSLATE -s -j

# implicit rules
%.mo: %.po
	$(MSGFMT) -c -o $@ $<

# a PO file must be updated from wxstd.po to include new translations
# (but not manually maintained platform-specific files like msw/it.po)
$(foreach lang,$(WX_LINGUAS_UPDATE),$(lang).po): wxstd.po

%.po:
	if [ -f $@ ]; then $(MSGMERGE) $@ wxstd.po > $@.new && mv $@.new $@; else cp wxstd.po $@; fi

wxstd.po:
	touch $@
	find ../include -name "*.h" | $(XARGS) $(XGETTEXT) $(XGETTEXT_ARGS) -o wxstd.po
	find ../src -name "*.cpp" | $(XARGS) $(XGETTEXT) $(XGETTEXT_ARGS) -o wxstd.po
	find ../contrib/include -name "*.h" | $(XARGS) $(XGETTEXT) $(XGETTEXT_ARGS) -o wxstd.po
	find ../contrib/src -name "*.cpp" | $(XARGS) $(XGETTEXT) $(XGETTEXT_ARGS) -o wxstd.po

allpo: force-update
	@-for t in $(WX_LINGUAS_UPDATE); do $(MAKE) $$t.po; done

allmo:
	@for t in $(WX_LINGUAS); do $(MAKE) $$t.mo; done

force-update:
	$(RM) wxstd.po

# print out the percentage of the translated strings
stats: FORCE
	@for i in $(WX_LINGUAS); do \
		x=`$(MSGFMT) -o /dev/null "$$i.po" 2>&1 | sed -e 's/[,\.]//g' \
			-e 's/\([0-9]\+\) translated messages\?/TR=\1/' \
			-e 's/\([0-9]\+\) fuzzy translations\?/FZ=\1/' \
			-e 's/\([0-9]\+\) untranslated messages\?/UT=\1/'`; \
		TR=0 FZ=0 UT=0; \
		eval $$x; \
		TOTAL=`expr $$TR + $$FZ + $$UT`; \
		echo "$$i.po `expr 100 "*" $$TR / $$TOTAL`% of $$TOTAL strings"; \
	done

FORCE:

.PHONY: allpo allmo force-update percentage FORCE

# $Id$
