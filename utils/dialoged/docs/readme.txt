
Prototype dialog editor and property sheet classes
--------------------------------------------------

Julian Smart, October 4th 1995
------------------------------

Here's what I've done so far on a lightweight dialog editor.  The 16-bit
Windows binaries in the bin directory are dialoged.exe (the dialog
editor) and test.exe (a small property sheet demo). 

Main points:

 - You can create a new dialog box and add items to it.
 - You can move items around, and right-click
   to edit a few properties (very incomplete).
 - Can't write out .wxr files yet. Loading code is in
   wxWindows, but writing code is absent: should be put
   into wxWindows.
 - No attempt at resources other than dialogs yet.
   Should have menu editor too.
 - Should *somehow* have a protocol to allow
   existing resources e.g. in wxCLIPS/wxPython
   to be edited in situ.
   This should be made simpler by the existance of
   the plug-in event handler mechanism, which means you
   can temporarily handle all the events yourself.
 - See dialoged.cc: the main program is tiny because
   it's meant to be embeddable.
 - The wxPropertySheet (set of) classes are very
   general and should be put into wxWin and documented.
   
Comments, chivvying and help all appreciated. Maybe if
I documented what I had, it would be easier for others
to do some work on it.

Regards,

Julian