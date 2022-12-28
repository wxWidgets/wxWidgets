How to add a new font encoding to wxWidgets
===========================================

Introduction
------------

wxWidgets has built in support for a certain number of font encodings (which
is synonymous with code sets and character sets for us here even though it is
not exactly the same thing), look at include/wx/fontenc.h for the full list.
This list is far from being exhaustive though and if you have enough knowledge
about an encoding to add support for it to wxWidgets, this tech note is for
you!

A word of warning though: this is written out of my head and is surely
incomplete. Please correct the text here, especially if you detect problems
when you try following it.

Also note that I completely ignore all the difficult issues of support for
non European languages in the GUI (i.e. BiDi and text orientation support).


The recipe
----------

Suppose you want to add support for Klingon to wxWidgets. This is what you'd
have to do:

1. include/wx/fontenc.h: add a new `wxFONTENCODING_KLINGON` enum element, if
   possible without changing the values of the existing elements of the enum
   and be careful to now make it equal to some other elements -- this means
   that you have to put it before `wxFONTENCODING_MAX`

2. `wxFONTENCODING_MAX` must be the same as the number of elements in 3
   (hopefully) self explanatory arrays in src/common/fmapbase.cpp:

   - gs_encodings
   - gs_encodingDescs
   - gs_encodingNames

   You must update all of them, e.g. you'd add `wxFONTENCODING_KLINGON`,
   "Klingon (Star Trek)" and "klingon" to them in this example. The latter
   name should ideally be understandable to both Win32 and iconv as it is used
   to convert to/from this encoding under Windows and Unix respectively.
   Typically any reasonable name will be supported by iconv, if unsure run
   "iconv -l" on your favourite Unix system. For the list of charsets
   supported under Win32, look under HKEY_CLASSES_ROOT\MIME\Database\Charset
   in regedit. Of course, being consistent with the existing encoding names
   wouldn't hurt either.

3. Normally you don't have to do anything else if you've got support for this
   encoding under both Win32 and Unix. If you haven't, you should modify
   wxEncodingConverter to support it (this could be useful anyhow as a
   fallback for systems where iconv is unavailable). To do it you must:
   a) add a new table to src/common/unictabl.inc: note that this file is auto
      generated so you have to modify misc/unictabl script instead (probably)
   b) possibly update EquivalentEncodings table in src/common/encconv.cpp
      if `wxFONTENCODING_KLINGON` can be converted into another one
      (losslessly only or not?)

4. Add a [unit test](how-to-write-unit-tests.md) for support of your new encoding (with
   time we should have a wxCSConv unit test so you would just add a case to
   it for `wxFONTENCODING_KLINGON`) and test everything on as many different
   platforms as you can.
