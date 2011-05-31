wxAutohacks

wxAutohacks is a way of ensuring the configure script remains consistent
between developer commits.  Previous releases include a build/aclocal_include
which has now been moved into the wxWidgets build/aclocal directory.

At the moment, it is intended to be copied and or symlinked into a wxWidgets
source tree.  For example, assume you have wxWidgets checked out in the
/home/myname/wxCVS/wxWidgets/ path.  Then from /home/myname/wxCVS do this:
cvs -d :pserver:anoncvs@cvs.wxwidgets.org:/pack/cvsroots/wxwidgets checkout wxAutohacks

You will now have wxAutohacks in /home/myname/wxCVS/wxAutohacks/.  Now go into
the wxWidgets build (/home/myname/wxCVS/wxWidgets/build/) and do this:
ln -s ../../wxAutohacks/build/autoconf_prepend-include .

From now on from the root of your wxWidgets source tree when you do this:
make -f build/autogen.mk
you will also build an autoconf.m4f file which will ensure the configure
script stays consistent.

Alternatively you can do this:
make -f build/autogen.mk AUTOHACKS_PREPEND_INCLUDE_DIR=../wxAutohacks/build/autoconf_prepend-include
