AUTOMAKE_OPTIONS = 1.3 no-dependencies

SUFFIXES = .cpp

DEFS = @DEFS@ $(TOOLKIT_DEF) $(WXDEBUG_DEFINE)

noinst_PROGRAMS = wxpoem

wxpoem_SOURCES = wxpoem.cpp

# create a link to the file needed to run the sample
all-local:
	@if [ ! -e wxpoem.dat ]; then ln -s $(srcdir)/wxpoem.dat wxpoem.dat; fi
