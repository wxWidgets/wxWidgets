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
    #pragma interface
#endif

#include <wx/string.h>
#include <wx/list.h>
#include <wx/hash.h>

#if defined(HAVE_DLOPEN)
    #include <dlfcn.h>

    typedef void *wxDllType;
#elif defined(HAVE_SHLLOAD)
    #include <dl.h>

    typedef void *wxDllType;
#elif defined(__WINDOWS__)
    #include <windows.h>

    typedef HMODULE wxDllType;
#elif defined(__WXMAC__)
    typedef CFragConnectionID wxDllType;
#else
    #error "wxLibrary can't be compiled on this platform, sorry."
#endif // OS

// defined in windows.h
#ifdef LoadLibrary
    #undef LoadLibrary
#endif

// ----------------------------------------------------------------------------
// wxLibrary
// ----------------------------------------------------------------------------

class wxLibrary : public wxObject
{
public:
    wxHashTable classTable;

public:
    wxLibrary(void *handle);
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

class wxLibraries
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

#endif // _WX_DYNLIB_H__
