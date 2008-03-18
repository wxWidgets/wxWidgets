/////////////////////////////////////////////////////////////////////////////
// Name:        init.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/** @ingroup group_funcmacro_appinitterm */
//@{

/**
    This function can be used to perform the initialization of wxWidgets if you
    can't use the default initialization code for any reason.

    If the function returns true, the initialization was successful and the
    global wxApp object ::wxTheApp has been created. Moreover, wxEntryCleanup()
    must be called afterwards. If the function returns false, a catastrophic
    initialization error occured and (at least the GUI part of) the library
    can't be used at all.

    Notice that parameters @c argc and @c argv may be modified by this
    function.

    @header{wx/init.h}
*/
bool wxEntryStart(int& argc, wxChar** argv);

/**
    See wxEntryStart(int&,wxChar**) for more info about this function.

    This is an additional overload of wxEntryStart() provided under MSW only.
    It is meant to be called with the parameters passed to WinMain().

    @note Under Windows CE platform, and only there, the type of @a pCmdLine is
    @c wchar_t *, otherwise it is @c char *, even in Unicode build.

    @header{wx/init.h}
*/
bool wxEntryStart(HINSTANCE hInstance,
                  HINSTANCE hPrevInstance = NULL,
                  char* pCmdLine = NULL,
                  int nCmdShow = SW_SHOWNORMAL);

/**
    Free resources allocated by a successful call to wxEntryStart().

    @header{wx/init.h}
*/
void wxEntryCleanup();

//@}

