/////////////////////////////////////////////////////////////////////////////
// Name:        dynlib.cpp
// Purpose:     Dynamic library management
// Author:      Guilhem Lavaux
// Modified by:
// Created:     20/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DYNLIB_H__
#define _WX_DYNLIB_H__

#ifdef __GNUG__
#   pragma interface
#endif

#include "wx/setup.h"

#if wxUSE_DYNLIB_CLASS

#include "wx/string.h"
#include "wx/list.h"
#include "wx/hash.h"

// this is normally done by configure, but I leave it here for now...
#if defined(__UNIX__) && !(defined(HAVE_DLOPEN) || defined(HAVE_SHL_LOAD))
#   if defined(__LINUX__) || defined(__SOLARIS__) || defined(__SUNOS__) || defined(__FREEBSD__)
#      define HAVE_DLOPEN
#   elif defined(__HPUX__)
#      define HAVE_SHL_LOAD
#   endif // Unix flavour
#endif // !Unix or already have some HAVE_xxx defined

// Note: WXPM/EMX has to be tested first, since we want to use
// native version, even if configure detected presence of DLOPEN.
#if defined(__WXPM__) || defined(__EMX__)
#   define INCL_DOS
#   include <os2.h>
    typedef HMODULE wxDllType;
#elif defined(HAVE_DLOPEN)
#   include <dlfcn.h>
    typedef void *wxDllType;
#elif defined(HAVE_SHL_LOAD)
#   include <dl.h>
    typedef shl_t wxDllType;
#elif defined(__WINDOWS__)
#   include <windows.h>         // needed to get HMODULE
    typedef HMODULE wxDllType;
#elif defined(__WXMAC__)
    typedef CFragConnectionID wxDllType;
#else
#   error "wxLibrary can't be compiled on this platform, sorry."
#endif // OS

// LoadLibrary is defined in windows.h as LoadLibraryA, but wxDllLoader method
// should be called LoadLibrary, not LoadLibraryA or LoadLibraryW!
#if defined(__WIN32__) && defined(LoadLibrary)
#   include "wx/msw/winundef.h"
#endif

// ----------------------------------------------------------------------------
// wxDllLoader
// ----------------------------------------------------------------------------

/** wxDllLoader is a class providing an interface similar to unix's
    dlopen(). It is used by the wxLibrary framework and manages the
    actual loading of DLLs and the resolving of symbols in them.
    There are no instances of this class, it simply serves as a
    namespace for its static member functions.
*/
class WXDLLEXPORT wxDllLoader
{
public:
    /** This function loads a shared library into memory, with libname
      being the basename of the library, without the filename
      extension. No initialisation of the library will be done.
      @param libname Name of the shared object to load.
      @param success Must point to a bool variable which will be set to TRUE or FALSE.
      @return A handle to the loaded DLL. Use success parameter to test if it is valid.
     */
    static wxDllType LoadLibrary(const wxString & libname, bool *success = NULL);
    /** This function unloads the shared library. */
    static void UnloadLibrary(wxDllType dll);
    /** This function returns a valid handle for the main program
      itself. */
    static wxDllType GetProgramHandle(void);
    /** This function resolves a symbol in a loaded DLL, such as a
      variable or function name.
      @param dllHandle Handle of the DLL, as returned by LoadDll().
      @param name Name of the symbol.
      @return A pointer to the symbol.
     */
    static void * GetSymbol(wxDllType dllHandle, const wxString &name);

private:
    /// forbid construction of objects
    wxDllLoader();
};

// ----------------------------------------------------------------------------
// wxLibrary
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxLibrary : public wxObject
{
public:
    wxHashTable classTable;

public:
    wxLibrary(wxDllType handle);
    ~wxLibrary();

    // Get a symbol from the dynamic library
    void *GetSymbol(const wxString& symbname);

    // Create the object whose classname is "name"
    wxObject *CreateObject(const wxString& name);

protected:
    void PrepareClasses(wxClassInfo *first);

    wxDllType m_handle;
};



// ----------------------------------------------------------------------------
// wxLibraries
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxLibraries
{
public:
    wxLibraries();
    ~wxLibraries();

    // caller is responsible for deleting the returned pointer if !NULL
    wxLibrary *LoadLibrary(const wxString& basename);

    wxObject *CreateObject(const wxString& name);

protected:
    wxList m_loaded;
};

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------

extern wxLibraries wxTheLibraries;

// ----------------------------------------------------------------------------
// Interesting defines
// ----------------------------------------------------------------------------

#define WXDLL_ENTRY_FUNCTION() \
extern "C" wxClassInfo *wxGetClassFirst(); \
wxClassInfo *wxGetClassFirst() { \
  return wxClassInfo::GetFirst(); \
}

#endif // wxUSE_DYNLIB_CLASS

#endif // _WX_DYNLIB_H__
