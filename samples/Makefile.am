# The makefile for wxWindows samples: only one sample is always built, the
# minimal one, the other samples to build are determined at configure time
# because many of them require the library to have some features compiled into
# it)
#
# $Id$

SUBDIRS = minimal \
 @SAMPLES_SUBDIRS@

# do nothing during "make [un]install" in the samples subdirectory (can't
# override install because install-recursive is its dependency which is still
# built)
#
# this results in warnings from make about "overriding commands for garget
# install-recursive", but it's more than compensated by the time we gain by not
# entering/leaving all samples directories (but if somebody has a better
# solution it would be welcome!)
install-recursive:
	@true

uninstall-recursive:
	@true

