How to add a new XRC handler
============================

0. Purpose
----------

This note describes what needs to be done to add a new XRC handler, i.e. add
support for loading the objects of some class wxFoo from XRC.


1. Implement the handler
------------------------

By convention, the XRC handler for a class wxFoo declared in wx/foo.h is called
wxFooXmlHandler and is declared in the file wx/xrc/xh_foo.h (this last rule
wasn't always respected in the past, however it's not a reason to not respect
it in the future). The steps for adding a new handler are:

- Add handler declaration in include/wx/xrc/xh_foo.h, it will usually be the
   same as in the other files so you can get inspiration for your brand new
   handler from e.g. wx/xrc/xh_srchctrl.h. Notice the use of `wxUSE_FOO` if wxFoo
   is guarded by this symbol.

- Add implementation in src/xrc/xh_foo.cpp: again, it will be almost always
   very similar to the existing controls. You will need to add support for
   the control-specific styles.


2. Update the other files
-------------------------

There are a few other files to update to make wxWidgets aware of the new
handler:

- Add the new files created above to build/bakefiles/files.bkl: search for
   "xh_srchctrl" to see where you need to add them

- Add #include "wx/xrc/xh_foo.h" to wx/xrc/xh_all.h.

- Register the new handler in wxXmlResource::InitAllHandlers() in
   src/xrc/xmlrsall.cpp


3. Update the sample
--------------------

Demonstrate that the new handler works by adding a control using it to
samples/xrc/rc/controls.xrc.
