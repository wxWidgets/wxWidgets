# this is the makefile for generating wxstd.po message catalog file and
# building lang.mo files from the translated lang.po catalogs

# this makefile may be invoked to build either wxstd.po or any lang.mo

# the programs we use (TODO: use configure to detect them)
MSGFMT=msgfmt
XGETTEXT=xgettext
XARGS=xargs

# implicit rules
%.mo: %.po
	$(MSGFMT) -o $@ $<


wxstd.po: FORCE
	find ../include -name "*.h" | $(XARGS) $(XGETTEXT) -C -k_ -j -o wxstd.po
	find ../src -name "*.cpp" | $(XARGS) $(XGETTEXT) -C -k_ -j -o wxstd.po

# targets depending on this one will be always remade
FORCE:

# $Id$
