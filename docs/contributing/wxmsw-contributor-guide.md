Special notes about writing wxMSW code
======================================

0. Purpose
----------

This is just a collection of various notes which should be useful to anybody
working on wxMSW codebase, please feel free to add more here.

This text assumes familiarity with both Windows programming and wxWidgets so it
doesn't cover any wxWidgets-wide issues not specific to Windows.


1. Windows headers wrappers
---------------------------

In no event should the Windows headers such as `<windows.h>` or `<commctrl.h>` be
included directly. So instead of `#include <...>` use `"wx/msw/wrapwin.h"` or
`wx/msw/wrapcctl.h`.

For convenience it is also possible to replace `#include <commdlg.h>` and
`<shlobj.h>` with `#include "wx/msw/wrapcdlg.h"` and `wrapshl.h` but this is less
vital.

Also notice that many convenient (albeit undocumented) functions and classes
are declared in "wx/msw/private.h", please do become familiar with this file
contents and use the utility classes and functions from it instead of
duplicating their functionality (which can often be done only in exception
unsafe way).


2. Windows features checks
--------------------------

All checks of features not present in all Windows versions must be done both at
compile-time (because, even though we use maximal value for WINVER in our code,
some compilers come with headers too old to declare them) and at run-time
(because wxMSW applications should run everywhere).

The functions wxGetWinVersion() (from wx/msw/private.h) and wxApp::
GetComCtl32Version() should be used to check Windows and comctl32.dll versions
respectively.

Any functions which may not be present in kernel32.dll/user32.dll/... in all
Windows versions should be resolved dynamically, i.e. using wxDynamicLibrary as
otherwise any wx application -- even not needing them at all -- would refuse to
start up on Windows versions not implementing the feature in question. As an
example, look at AlphaBlt()-related code in src/msw/dc.cpp.
