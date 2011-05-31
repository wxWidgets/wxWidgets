How to build the Debian wx packages
===================================

This file is currently just a brain dump of my experiences with
building the Debian wx packages, based on various experimentations,
and Googling around.  Please don't take anything said here as
authoritative or written in stone.  Although I've been able to get
things to work fairly reliably, I still feel pretty clueless about
some things.


Overview
--------

Contrary to how RPM and other packaging systems work, building Debian
packages is done with an expanded source tree instead of using a
tarball.  Inside the toplevel of the source tree you'll find a subdir
named debian, and within this dir are various files used by the build.
The most important of these are the control file and the rules file.
The control file specifies the metadata about each package, such as
name, description, dependencies, etc.  Interestingly, the version
number of the current build is not in the control file as might be
expected, but is instead taken from the changelog file.  I guess this
is a way for debian to make sure that there is always an updated
changelog for every release, but it is very non-intuitive to say the
least.  The rules file is a Makefile, and is executable with a #! that
runs make on itself.  This lets you execute commands from the toplevel
source dir like:

       ./debian/rules build

While theoretically you could build the wx packages directly from your
CVS workspace this won't work (currently) for a couple reasons.
First, the debian/rules file is currently looking at the name of the
toplevel source dir and extracting from it the flavour name (if
present) for use in passing to --with-flavor configure flag.  (I'm
considering changing how it finds this value for 2.7 so, for example,
the debian packages could also be built from the wxPython source
tarball...)  Second, since the build tools use the current source dir
for creating the source package, you'll probably want to start with a
clean source tree that has had unnecessary things removed from it.
There is a Makefile target that will create a minimized and clean
source tree for you, and will name it as debian/rules expects.  To use
it go to a build dir where you've already run configure, and then run:

       make debian-dist

This will result in a new source tree with a name like
wxwidgets2.7-2.7.0.0 that is located as a sibling to the toplevel of
the current source tree.


Environment
-----------

The various dpkg helper tools will use some environment settings to
provide default values.  I have these set:

       DEBFULLNAME='Robin Dunn'
       DEBEMAIL=robin@alldunn.com
       DEBSIGN_KEYID='Robin Dunn <robin@alldunn.com>'
       DEBUILD_DPKG_BUILDPACKAGE_OPTS='-i -ICVS -I.svn'

Notice the DEBSIGN_KEYID value.  If this is set (and you have a
matching gnupg key) then the packages will be digitally signed when
they are built.

If you are building packages that you intend to be installable on
machines other than your own, then I recommend that you either have a
separate machine with a minimal OS install, or set up a chroot
environment and do the builds there.  The reason for this is to
minimize unexpected extra dependencies that the built packages will
have because of extra things you have installed on your desktop
system, for example OpenGL libs installed by your video card drivers.
Using a chroot will also allow you to build packages for different
versions of Debian (or Ubuntu) if desired.  There is a good
description of setting up a chroot environment here:

       https://wiki.ubuntu.com/DebootstrapChroot

In addition to the base system packages, you'll need to install in
the chroot environment any packages needed for building wxWidgets
(compilers, make, autoconf, gtk and image libs, lib-dev's, python,
python-dev, etc.)  as well as the packages listed in the next section.


Build Packages
--------------

There are a number of helper packages that are used when building
debian packages.  Here are some that I have in my chroot, there may be
some others that I am not seeing at the moment:

       debhelper
       devscripts
       dh-make
       dpkg-dev
       fakeroot
       lintian
       diff
       patch


Doing the Build
---------------

Ok, if you are not totally confused by this point you should have a
minimal source tree produced by "make debian-dist" that is accessible
by your chroot or other minimal install system. (Unless you are only
making packages for yourself, then doing the build on in your main
desktop environment would  be okay.)  The first step is to chdir to
the top level of this source tree.

If you haven't already you'll want to edit debian/changelog to make an
entry for the current build.  If the version number is changing since
the last build then you'll need a whole new section.  If you are just
updating something in the same version then you can just get by with
editing the current changelog entry.  The 'dch' tool can be used to
open the file in an editor with the cursor positioned for you.  If you
use 'dch -i' then it will create a new changelog entry for you with
the proper syntax.  Don't forget to copy this file back to your CVS
workspace if appropriate.

Our debian/control file is generated from debian/control.in, so you
can force it to be created now by running the following.  You may want
to do this to verify its contents before proceeding with the build.

       ./debian/rules debian/control

To run just the compile/link/etc. portions of the build you can do
this:

       ./debian/rules build

To also make a set of test binaries you can do this:

       fakeroot ./debian/rules binary

To clean up from prior builds you can do this:

       fakeroot ./debian/rules clean

And to automate the entire process (build, binaries, source packages,
digital signing, etc.) you can do this:

       dpkg-buildpackage -rfakeroot

When finished the binary and source packages will be left in the
parent dir of the source tree.


Automating the process
----------------------

The script debian/build_all automates the above stepds and builds both
wxWidgets and wxPython packages in both ANSI and Unicode modes. You should do
the build manually at least the first time however to make sure you have all
the prerequisites and the build doesn't fail.


Other Sources of Information
----------------------------

http://liw.iki.fi/liw/talks/debian-packaging-tutorial.pdf
http://women.alioth.debian.org/wiki/index.php/English/BuildingTutorial
http://women.alioth.debian.org/wiki/index.php/English/AdvancedBuildingTips
http://www.wiggy.net/presentations/2001/DebianWalkThrough/handouts/handouts.html
http://www.debian.org/doc/manuals/maint-guide/index.en.html
http://www.isotton.com/debian/docs/repository-howto/repository-howto.html

