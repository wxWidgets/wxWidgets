# this is the makefile for generating wxstd.po message catalog file and
# building lang.mo files from the translated lang.po catalogs

# this makefile may be invoked to build either wxstd.po or any lang.mo

# Autodetect the languages we support.  Currently this relies on make
# being called with this dir as the cwd, but if we generate this file
# with configure an explicit path should be specified -- RL.

WX_LINGUAS := `ls *.po 2> /dev/null | sed -n 's/\(wxstd\)\?\.po//p'`

# the programs we use (TODO: use configure to detect them)
MSGFMT=msgfmt --verbose
MSGMERGE=msgmerge
XGETTEXT=xgettext
XARGS=xargs

# common xgettext args: C++ syntax, use the specified macro names as markers
XGETTEXT_ARGS=-C -k_ -kwxGetTranslation -kwxTRANSLATE -s -j

# implicit rules
%.mo: %.po
	$(MSGFMT) -o $@ $<

# a PO file must be updated from wxstd.po to include new translations
%.po: wxstd.po
	if [ -f $@ ]; then $(MSGMERGE) $@ wxstd.po > $@.new && mv $@.new $@; else cp wxstd.po $@; fi

wxstd.po:
	touch $@
	find ../include -name "*.h" | $(XARGS) $(XGETTEXT) $(XGETTEXT_ARGS) -o wxstd.po
	find ../src -name "*.cpp" | $(XARGS) $(XGETTEXT) $(XGETTEXT_ARGS) -o wxstd.po

allpo: force-update
	@-for t in $(WX_LINGUAS); do $(MAKE) $$t.po; done

allmo:
	@for t in $(WX_LINGUAS); do $(MAKE) $$t.mo; done

force-update:
	$(RM) wxstd.po

# targets depending on this one will be always remade
FORCE:

.PHONY: allpo allmo force-update

# $Id$
