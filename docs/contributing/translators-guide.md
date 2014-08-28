wxWidgets translator guide
==========================

This note is addressed to wxWidgets translators.

First of all, here is what you will need:

1. GNU gettext package version 0.10.35 or later (NB: earlier versions were
   known to have serious bugs)

   a) for Unix systems you can download gettext-0.10.tar.gz from any GNU
      mirror (RPMs and DEBs are also available from the usual places)

   b) for Windows you can download the precompiled binaries from
      www.wxwidgets.org or install PoEdit (poedit.sourceforge.net) and
      add <installdir>/poEdit/bin to your path (so it can find xgettext).

2. A way to run a program recursively on an entire directory from the command
   line:

   a) for Unix systems, this is done in locale/Makefile using the standard find
      command and xargs which is installed on almost all modern Unices.  If you
      are unlucky enough to not have xargs, you can use the -exec option of find
      instead.

   b) for Win32 systems you can use either Cygwin or MinGW. If you don't have
      those it is less trivial: if you have 4DOS/4NT/bash, that's
      fine, but you'd have to use some kind of "for /s" loop with the
      command.com/cmd.exe.

3. (at least read) access to the subversion is not necessary strictly speaking, but
   will make things a lot easier for you and others.


Now a brief overview of the process of translations (please refer to GNU
gettext documentation for more details). It happens in several steps:

1. the strings to translate are extracted from the C++ sources using xgettext
   program into a wxstd.pot file which is a "text message catalog"

2. this new wxstd.pot file (recreated each time some new text messages are added
   to wxWidgets) is merged with existing translations in another .po file (for
   example, de.po) and replaces this file (this is done using the program
   msgmerge)

3. the resulting .po file must be translated

4. finally, msgformat must be run to produce a .mo file: "binary message catalog"


How does it happen in practice? There is a Makefile in the "locale"
directory which will do almost everything (except translations) for you. i.e.
just type "make lang.po" to create or update the message catalog for 'lang'.
Then edit the resulting lang.po and make sure that there are no empty or fuzzy
translations left (empty translations are the ones with msgstr "", fuzzy
translations are those which have the word "fuzzy" in a comment just above
them). Then type "make lang.mo" which will create the binary message catalog.

Under Windows (If you don't have Cygwin or MinGW), you should execute the
commands manually, please have a look at Makefile to see what must be done.

For platform specific translations, .po files such as 'locale/msw/it.po' can be
used to provide translations that override the usual ones in 'locale/it.po'.
The generated .mo files are then installed under the names such as 'wxmsw.mo'
alongside the generic 'wxstd.mo'.

A new platform specific translation should be added to a section such as this
in wx.bkl:

    <gettext-catalogs id="locale_msw">
        <srcdir>$(SRCDIR)/locale/msw</srcdir>
        <catalog-name>wxmsw</catalog-name>
        <linguas>it</linguas>
        <install-to>$(LOCALEDIR)</install-to>
    </gettext-catalogs>
