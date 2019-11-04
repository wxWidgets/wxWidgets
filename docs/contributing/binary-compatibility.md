Binary Compatibility and wxWidgets
==================================

Purpose
-------

This is a broad technote covering all aspects of binary compatibility with
wxWidgets.

Releases
--------

General overview of releases can be found in [wxWidgets naming conventions](about-platform-toolkit-and-library-names.md),
but for completeness the wxWidgets release version number is as follows:

	2.6.2

Where

	  2      6      2
	Major  Minor Release

(i.e. Major.Minor.Release).

All versions with EVEN minor version component (e.g. 2.8.x, 3.0.x etc.)
are expected to be binary compatible (ODD minors are development versions
and the compatibility constraints don't apply to them). Note that by
preserving binary compatibility we mean BACKWARDS compatibility only,
meaning that applications built with old wxWidgets headers should continue
to work with new wxWidgets (shared/dynamic) libraries without the need to
rebuild. There is no requirement to preserve compatibility in the other
direction (i.e. make new headers compatible with old libraries) as this
would preclude any additions whatsoever to the stable branch. But see
also the section about `wxABI_VERSION`.


What kind of changes are NOT binary compatible
----------------------------------------------

[The KDE guide](https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B)
is a good reference.


The changes that are NOT binary compatible:

- Adding a virtual function
- Changing the name of any function or variable
- Changing the signature of a virtual function (adding a parameter,
  even a default one)
- Changing the order of the virtual functions in a class ("switching" them, etc.)
- Changing access privileges of a function: some compilers (among which MSVC)
  use the function access specifier in its mangled name. Moreover, while
  changing a private function to public should be compatible (as the old
  symbol can't be referenced from outside the library anyhow), changing a
  virtual private function to public is NOT compatible because the old symbol
  is referenced by the virtual tables in the executable code and so an old
  program compiled with MSVC wouldn't start up with a new DLL even if it
  doesn't use the affected symbol at all!
- Adding a member variable
- Changing the order of non-static member variables


Changes which are compatible
----------------------------

- Adding a new class
- Adding a new non-virtual method to an existing class
- Adding a new constructor to an existing class
- Overriding the implementation of an existing virtual function
  (this is considered to be backwards binary compatible until we find a
  counter example; currently it's known to work with Apple gcc at least)
- Anything which doesn't result in ABI change at all, e.g. adding new
  macros, constants and, of course, private changes in the implementation


`wxABI_VERSION` and "forward" binary compatibility
--------------------------------------------------

As mentioned we do not support "forward" binary compatibility, that is the
ability to run applications compiled with new wxWidgets headers on systems
with old wxWidgets libraries.

However, for the developers who want to ensure that their application works
with some fixed old wxWidgets version and doesn't (inadvertently) require
features added in later releases, we provide the macro `wxABI_VERSION` which
can be defined to restrict the API exported by wxWidgets headers to that of
a fixed old release.

For this to work, all new symbols added to binary compatible releases must
be `#if`'ed with `wxABI_VERSION`.

The layout of `wxABI_VERSION` is as follows:

	20602

where

	 2     06     02
	Major Minor Release

I.e. it corresponds to the wxWidgets release in section Releases.

An example of using `wxABI_VERSION` is as follows for symbols
only in a 2.6.2 release:

	```cpp
	#if wxABI_VERSION >= 20602 /* 2.6.2+ only */
	bool Load(const wxURI& location, const wxURI& proxy);

	wxFileOffset GetDownloadProgress();
	wxFileOffset GetDownloadTotal();

	bool    ShowPlayerControls(
	  wxMediaCtrlPlayerControls flags =
	    wxMEDIACTRLPLAYERCONTROLS_DEFAULT);

	//helpers for the wxPython people
	bool LoadURI(const wxString& fileName)
	{   return Load(wxURI(fileName));       }
	bool LoadURIWithProxy(const wxString& fileName, const wxString& proxy)
	{   return Load(wxURI(fileName), wxURI(proxy));       }
	#endif
	```


Workarounds for adding virtual functions
----------------------------------------

Originally the idea for adding virtual functions to binary compatible
releases was to pad out some empty "reserved" functions and then
rename those later when someone needed to add a virtual function.

However, after there was some actual testing of the idea a lot of
controversy erupted.  Eventually we decided against the idea, and
instead devised a new method for doing so called wxShadowObject.

wxShadowObject is a class derived from wxObject that provides a means
of adding functions and/or member variables to a class internally
to wxWidgets.  It does so by storing these in a hash map inside of
it, looking it up when the function etc. is called. wxShadowObject
is generally stored inside a reserved member variable.

wxShadowObject resides in include/wx/clntdata.h.

To use wxShadowObject, you first call AddMethod or AddField with
the first parameter being the name of the field and/or method
you want, and the second parameter being the value of the
field and/or method.

In the case of fields this is a void*, and in the case of method
is a wxShadowObjectMethod which is a typedef:

	typedef int (*wxShadowObjectMethod)(void*, void*);

After you add a field, you can set it via SetField with the same
parameters as AddField, the second parameter being the value to set
the field to.  You can get the field after you call AddField
via GetField, with the parameters as the other two field functions,
only in the case the second parameter is the fallback
value for the field in the case of it not being found in the
hash map.

You can call a method after you add it via InvokeMethod, which
returns a bool indicating whether or not the method was found
in the hash map, and has 4 parameters.  The first parameter is
the name of the method you wish to call, the second is the first
parameter passed to the wxShadowObjectMethod, the third is the
second parameter passed to that wxShadowObjectMethod, and the
fourth is the return value of the wxShadowObjectMethod.


version-script.in
-----------------

For ld/libtool we use sun-style version scripts.  Basically
anything which fits the conditions of being `#if`'ed via `wxABI_VERSION`
needs to go here also.

See 'info ld scripts version' on a GNU system, it's online here:
https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_node/ld_25.html

Or see chapter 5 of the 'Linker and Libraries Guide' for Solaris, available
online here:
https://docs.oracle.com/cd/E19120-01/open.solaris/819-0690/chapter5-84101/index.html

The file has the layout as follows:

	@WX_VERSION_TAG@.X

Where X is the current Release as mentioned earlier, i.e. 2.  This
is following by an opening bracket "{", followed by "global:",
followed by patterns matching added symbols, then followed by "}", and then
the file is either followed by earlier Releases or ended by
a @WX_VERSION_TAG@ block without the period or Release.

The patterns used to specify added symbols are globbing patters and can
contain wildcards such as '*'.

For example for a new class member such as:

    wxFont wxGenericListCtrl::GetItemFont( long item ) const;

the mangled symbol might be:

    _ZNK17wxGenericListCtrl11GetItemFontEl

so a line like this could be added to version-script.in:

    \*wxGenericListCtrl*GetItemFont*;

Allow for the fact that the name mangling is going to vary from compiler to
complier.

When adding a class you can match all the symbols it adds with a single
pattern, so long as that pattern is not likely to also match other symbols.
For example for wxLogBuffer a line like this:

    \*wxLogBuffer*;


Checking the version information in libraries and programs
----------------------------------------------------------

On Sun there is a tool for this, see pvs(1). On GNU you can use objdump, below
are some examples.

To see what versions of each library a program (or library) depends on:

	$ objdump -p widgets | sed -ne '/Version References/,/^$/p'
	Version References:
	  required from libgcc_s.so.1:
	    0x0b792650 0x00 10 GCC_3.0
	  required from libwx_based-2.6.so.0:
	    0x0cca2546 0x00 07 WXD_2.6
	  required from libstdc++.so.6:
	    0x056bafd3 0x00 09 CXXABI_1.3
	    0x08922974 0x00 06 GLIBCXX_3.4
	  required from libwx_gtk2d_core-2.6.so.0:
	    0x0a2545d2 0x00 08 WXD_2.6.2
	    0x0cca2546 0x00 05 WXD_2.6
	  required from libc.so.6:
	    0x09691a75 0x00 04 GLIBC_2.2.5

To see what WXD_2.6.2 symbols a program uses:

	$ objdump -T widgets | grep 'WXD_2\.6\.2'
	0000000000000000 g    DO *ABS*  0000000000000000  WXD_2.6.2   WXD_2.6.2
	00000000004126d8      DF *UND*  0000000000000177  WXD_2.6.2   _ZN19wxTopLevelWindowGTK20RequestUserAttentionEi

To see what WXD_2.6.2 symbols a library defines:

	$ objdump -T libwx_based-2.6.so | grep 'WXD_2\.6\.2' | grep -v 'UND\|ABS'
	0000000000259a10  w   DO .data  0000000000000018  WXD_2.6.2   _ZTI19wxMessageOutputBest
	00000000002599e0  w   DO .data  0000000000000028  WXD_2.6.2   _ZTV19wxMessageOutputBest
	000000000010a98e  w   DF .text  000000000000003e  WXD_2.6.2   _ZN19wxMessageOutputBestD0Ev
	0000000000114efb  w   DO .rodata        000000000000000e  WXD_2.6.2   _ZTS11wxLogBuffer
	0000000000255590  w   DO .data  0000000000000018  WXD_2.6.2   _ZTI11wxLogBuffer
	000000000011b550  w   DO .rodata        0000000000000016  WXD_2.6.2   _ZTS19wxMessageOutputBest
	00000000000bfcc8 g    DF .text  00000000000000dd  WXD_2.6.2   _ZN11wxLogBuffer5DoLogEmPKcl
	000000000010a3a6 g    DF .text  0000000000000153  WXD_2.6.2   _ZN19wxMessageOutputBest6PrintfEPKcz
	00000000000c0b22  w   DF .text  000000000000004b  WXD_2.6.2   _ZN11wxLogBufferD0Ev
	00000000000bfc3e g    DF .text  0000000000000089  WXD_2.6.2   _ZN11wxLogBuffer5FlushEv
	00000000000c0ad6  w   DF .text  000000000000004b  WXD_2.6.2   _ZN11wxLogBufferD1Ev
	00000000000b1130  w   DF .text  0000000000000036  WXD_2.6.2   _ZN11wxLogBufferC1Ev
	00000000000c095c  w   DF .text  0000000000000029  WXD_2.6.2   _ZN19wxMessageOutputBestC1Ev
	00000000000c08e8  w   DF .text  000000000000003e  WXD_2.6.2   _ZN19wxMessageOutputBestD1Ev
	00000000002554c0  w   DO .data  0000000000000038  WXD_2.6.2   _ZTV11wxLogBuffer
	00000000000bfda6 g    DF .text  0000000000000036  WXD_2.6.2   _ZN11wxLogBuffer11DoLogStringEPKcl
	00000000000abe10 g    DF .text  0000000000000088  WXD_2.6.2   _ZN14wxZipFSHandler7CleanupEv


Testing binary compatibility between releases
---------------------------------------------

An easy way of testing binary compatibility is just to build wxWidgets
in dll/dynamic library mode and then switch out the current library
in question with an earlier stable version of the library, then running
the application in question again.  If it runs OK then there is usually
binary compatibility between those releases.

You can also break into your debugger or whatever program you want
to use and check the memory layout of the class.  If it is the same
then it is binary compatible.
(In GDB the command x/d will show addresses as pointers to functions if
possible so you can see if the order of the functions in vtbl doesn't change.)

Another way to check for binary compatibility is to build wxWidgets in shared mode
and use the 'abicheck.sh --generate' script before doing your changes to generate
the current ABI (if the 'expected_abi' file is not already in the repo).
Then rebuild wxWidgets with your changes and use 'abicheck.sh' to compare the
resulting ABI with the expected one.
Note that the abicheck.sh script is in the "lib" folder.
