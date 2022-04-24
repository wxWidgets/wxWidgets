Updating Library Version Info
-----------------------------

for a new release the wxvers.xcconfig has to be updated accordingly

Building Projects
-----------------

makeprojects.py is a Python 3 script that builds upon pbxproj. It replaces the old
AppleScript that did not work with the current Xcode versions anymore.

The reason for this script is to support a single place of definition for the files needed
for a certain platform by building Xcode projects from the bakefiles files.bkl file list.

It creates new projects from the ..._in.xcodeproj templates in this folder and then
reads in the files lists from the files.bkl in the build/bakefiles directory, evaluates the
conditions in these definitions and then adds the correct files to the newly created Xcode
projects

If you only need a specific target and not all of them (cocoa, iphone) then you can
comment the unneeded makeProject calls.

Prerequisites
-------------

pbxproj (https://github.com/kronenthaler/mod-pbxproj)

Stefan Csomor
