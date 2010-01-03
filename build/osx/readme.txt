Updating Library Version Info
-----------------------------

for a new release the wxvers.xcconfig has to be updated accordingly

Building Projects
-----------------

makeprojects is an AppleScript that uses the XML Tools from 
http://www.latenightsw.com/freeware/XMLTools2/

the reason for this script is to support a single place of definition for the files needed
for a certain platform by building XCode projects from the bakefiles files.bkl file list.

it creates new projects from the ..._in.xcodeproj templates in this folder and then
reads in the files lists from the files.bkl in the build/bakefiles directory, evaluates the
conditions in these definitions and then adds the correct files to the newly created XCode
projects

Stefan Csomor
