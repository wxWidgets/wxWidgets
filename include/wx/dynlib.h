/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dynlib.h
// Purpose:     Dynamic library loading classes
// Author:      Guilhem Lavaux, Vadim Zeitlin, Vaclav Slavik
// Modified by:
// Created:     20/07/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DYNLIB_H__
#define _WX_DYNLIB_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#   pragma interface "dynlib.h"
#endif

#include "wx/setup.h"

#if wxUSE_DYNAMIC_LOADER

#include "wx/dynload.h"  // Use the new (version of) wxDynamicLibrary instead

#elif wxUSE_DYNLIB_CLASS

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
#elif defined(__DARWIN__)
    typedef void *wxDllType;
#elif defined(__WXMAC__)
    typedef void *wxDllType;
#else
#   error "wxLibrary can't be compiled on this platform, sorry."
#endif // OS

// LoadLibrary is defined in windows.h as LoadLibraryA, but wxDllLoader method
// should be called LoadLibrary, not LoadLibraryA or LoadLibraryW!
#if defined(__WIN32__) && defined(LoadLibrary)
#   include "wx/msw/winundef.h"
#endif

// ----------------------------------------------------------------------------
// wxDllLoader: low level DLL functions, use wxDynamicLibrary in your code
// ----------------------------------------------------------------------------

/*
    wxDllLoader is a class providing an interface similar to unix's dlopen().
    It is used by wxDynamicLibrary wxLibrary and manages the actual loading of
    DLLs and the resolving of symbols in them. There are no instances of this
    class, it simply serves as a namespace for its static member functions.
*/
class WXDLLIMPEXP_BASE wxDllLoader
{
public:
    /*
      This function loads the shared library libname into memory.

      libname may be either the full path to the file or just the filename in
      which case the library is searched for in all standard locations
      (use GetDllExt() to construct the filename)

      if success pointer is not NULL, it will be filled with TRUE if everything
      went ok and FALSE otherwise
     */
    static wxDllType LoadLibrary(const wxString& libname, bool *success = 0);

    /*
      This function unloads the shared library previously loaded with
      LoadLibrary
     */
    static void UnloadLibrary(wxDllType dll);

    /*
       This function returns a valid handle for the main program
       itself or NULL if back linking is not supported by the current platform
       (e.g. Win32).
     */
    static wxDllType GetProgramHandle();

    /*
       This function resolves a symbol in a loaded DLL, such as a
       variable or function name.

       dllHandle Handle of the DLL, as returned by LoadDll().
       name Name of the symbol.

       Returns the pointer to the symbol or NULL on error.
     */
    static void *GetSymbol(wxDllType dllHandle,
                           const wxString &name,
                           bool *success = NULL);

    // return the standard DLL extension (with leading dot) for this platform
    static const wxString &GetDllExt() { return ms_dllext; }

private:
    // forbid construction of objects
    wxDllLoader();
    static const wxString   ms_dllext;
};

// ----------------------------------------------------------------------------
// wxDynamicLibrary - friendly interface to wxDllLoader
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxDynamicLibrary
{
public:
    // ctors
    wxDynamicLibrary() { m_library = 0; }
    wxDynamicLibrary(const wxString& name) { Load(name); }

    // return TRUE if the library was loaded successfully
    bool IsLoaded() const { return m_library != 0; }
    operator bool() const { return IsLoaded(); }

    // load the library with the given name (full or not), return TRUE on
    // success
    bool Load(const wxString& name)
    {
        m_library = wxDllLoader::LoadLibrary(name);

        return IsLoaded();
    }

    // unload the library, also done automatically in dtor
    void Unload()
    {
        if ( IsLoaded() )
            wxDllLoader::UnloadLibrary(m_library);
    }

    // load a symbol from the library, return NULL if an error occured or
    // symbol wasn't found
    void *GetSymbol(const wxString& name) const
    {
        wxCHECK_MSG( IsLoaded(), NULL,
                     _T("can't load symbol from unloaded library") );

        return wxDllLoader::GetSymbol(m_library, name);
    }

    // unload the library
    //
    // NB: dtor is not virtual, don't derive from this class
    ~wxDynamicLibrary() { Unload(); }

private:
    // the handle to DLL or NULL
    wxDllType m_library;

    // no copy ctor/assignment operators (or we'd try to unload the library
    // twice)
    DECLARE_NO_COPY_CLASS(wxDynamicLibrary)
};

// ----------------------------------------------------------------------------
// wxLibrary
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxLibrary : public wxObject
{
public:
    wxLibrary(wxDllType handle);
    virtual ~wxLibrary();

    // Get a symbol from the dynamic library
    void *GetSymbol(const wxString& symbname);

    // Create the object whose classname is "name"
    wxObject *CreateObject(const wxString& name);

protected:
    void PrepareClasses(wxClassInfo *first);

    wxDllType m_handle;

public:
    wxHashTable classTable;
};

// ----------------------------------------------------------------------------
// wxLibraries
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxLibraries
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

extern WXDLLIMPEXP_DATA_BASE(wxLibraries) wxTheLibraries;

// ----------------------------------------------------------------------------
// Interesting defines
// ----------------------------------------------------------------------------

#define WXDLL_ENTRY_FUNCTION() \
extern "C" WXEXPORT const wxClassInfo *wxGetClassFirst(); \
const wxClassInfo *wxGetClassFirst() { \
  return wxClassInfo::GetFirst(); \
}

#endif // wxUSE_DYNLIB_CLASS

#endif // _WX_DYNLIB_H__
