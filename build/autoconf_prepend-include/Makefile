# Created: 2005/03/12
# Author: David Elliott

# For autoconf: Debian in their infinite wisdom decided to improve upon
# the standard autoconf 2.59 macros. Thus Debian's autoconf generates
# a totally different configure script.  This fixes it to look
# mostly like Debian's.  There are also some fixes pulled in from the OS/2
# fork of autoconf.

# Rule to freeze the m4 so autoconf will actually use it.
# NOTE: VERY important to cd to somewhere there are no .m4 files.
# or at least no aclocal.m4 or else autom4te helpfully picks it up.
.PHONY: all
all: autoconf/autoconf.m4f

AUTOCONF_SOURCES = \
  autoconf/c.m4 \
  autoconf/general.m4 \
  autoconf/libs.m4 \
  autoconf/status.m4

AUTOM4TE=autom4te

autoconf/autoconf.m4f: $(AUTOCONF_SOURCES)
	$(AUTOM4TE) -B . --language=Autoconf --freeze --output=autoconf/autoconf.m4f

