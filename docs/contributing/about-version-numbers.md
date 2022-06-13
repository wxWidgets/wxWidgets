All about wxWidgets Version Numbers
===================================

## Where to update the version numbers

There are several places in the wxWidgets source tree that
define the version number for the library.

The script `misc/scripts/inc_release` can be used for incrementing the release
field of the version, i.e. changing 2.8.x to 2.8.x+1 but it does not take
care of version.bkl and can't be used for changing the other version
components, this needs to be done manually. It also doesn't update
version.bkl file which always needs to be updated manually, follow the
instructions there.

Here is the list of files that need to be updated:

- build/bakefiles/version.bkl {C:R:A}          [NOT UPDATED AUTOMATICALLY]
- configure.in
- build/osx/wxvers.xcconfig
- docs/changes.txt
- docs/readme.txt (date needs manual editing)  [NOT UPDATED AUTOMATICALLY]
- docs/doxygen/Doxyfile (PROJECT_NUMBER and DOCSET_FEEDNAME)
- docs/doxygen/mainpages/manual.h (just date)  [NOT UPDATED AUTOMATICALLY]
- include/wx/version.h
- include/wx/osx/config_xcode.h
- samples/minimal/Info_cocoa.plist
- samples/minimal/CMakeListst.txt [major/minor only]

Do not forget to rebake everything after updating version.bkl!


## When to update the version numbers

Version should be updated immediately after releasing the previous version
so that the sources in the repository always correspond to the next release
and not the past one.

See also [binary compatibility notes](binary-compatibility.md)
