# The makefile for wxWindows samples: only one sample is always built, the
# minimal one, the other samples to build are determined at configure time
# because many of them require the library to have some features compiled into
# it)
#
# $Id$

SUBDIRS = minimal \
 @SAMPLES_SUBDIRS@

# do nothing during "make install" in the samples subdirectory (can't override
# install because install-recursive is its dependency which is still built)
install-recursive:
	@true

