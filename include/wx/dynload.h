/////////////////////////////////////////////////////////////////////////////
// Name:         dynload.h
// Purpose:      Dynamic loading framework
// Author:       Ron Lee, David Falkinder, Vadim Zeitlin and a cast of 1000's
//               (derived in part from dynlib.cpp (c) 1998 Guilhem Lavaux)
// Modified by:
// Created:      03/12/01
// RCS-ID:       $Id$
// Copyright:    (c) 2001 Ron Lee <ron@debian.org>
// Licence:      wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DYNAMICLOADER_H__
#define _WX_DYNAMICLOADER_H__

#ifdef __GNUG__
#pragma interface "dynload.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_DYNAMIC_LOADER

#include "wx/hash.h"
#include "wx/module.h"


// Ugh, I'd much rather this was typesafe, but no time
// to rewrite wxHashTable right now..

typedef wxHashTable wxDLManifest;
typedef wxHashTable wxDLImports;

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

    // Note: WXPM/EMX has to be tested first, since we want to use
    // native version, even if configure detected presence of DLOPEN.

#if defined(__WXPM__) || defined(__EMX__)
#define INCL_DOS
#include <os2.h>
    typedef HMODULE wxDllType;
#elif defined(HAVE_DLOPEN)
#include <dlfcn.h>
    typedef void *wxDllType;
#elif defined(HAVE_SHL_LOAD)
#include <dl.h>
    typedef shl_t wxDllType;
#elif defined(__WINDOWS__)
#include <windows.h>         // needed to get HMODULE
    typedef HMODULE wxDllType;
#elif defined(__DARWIN__)
    typedef void *wxDllType;
#elif defined(__WXMAC__)
    typedef CFragConnectionID wxDllType;
#else
#error "wxLibrary can't be compiled on this platform, sorry."
#endif

    // LoadLibrary is defined in windows.h as LoadLibraryA, but wxDllLoader
    // method should be called LoadLibrary, not LoadLibraryA or LoadLibraryW!

#if defined(__WIN32__) && defined(LoadLibrary)
#   include "wx/msw/winundef.h"
#endif


// ---------------------------------------------------------------------------
// wxDllLoader
// ---------------------------------------------------------------------------

    //  Cross platform wrapper for dlopen and friends.
    //  There are no instances of this class, it simply
    //  serves as a namespace for its static member functions.

class WXDLLEXPORT wxDllLoader
{
public:

        // libname may be either the full path to the file or just the filename
        // in which case the library is searched for in all standard locations.
        // The platform specific library extension is automatically appended.

    static wxDllType    Load(const wxString& name);

        // The same as Load, except 'name' is searched for without modification.

    static wxDllType    LoadLibrary(const wxString& name);
    static void         UnloadLibrary(wxDllType dll);

        // return a valid handle for the main program itself or NULL if
        // back linking is not supported by the current platform (e.g. Win32)

    static wxDllType GetProgramHandle();

        // resolve a symbol in a loaded DLL, such as a variable or function
        // name.  dllHandle is a handle previously returned by LoadLibrary(),
        // symbol is the (possibly mangled) name of the symbol.
        // (use extern "C" to export unmangled names)
        //
        // Since it is perfectly valid for the returned symbol to actually be
        // NULL, that is not always indication of an error.  Pass and test the
        // parameter 'success' for a true indication of success or failure to
        // load the symbol.
        //
        // Returns a pointer to the symbol on success.

    static void *GetSymbol(wxDllType dllHandle, const wxString &name, bool *success = 0);

        // return the platform standard DLL extension (with leading dot)

    static const wxString &GetDllExt() { return ms_dllext; }

private:

    wxDllLoader();                    // forbid construction of objects
    static const wxString ms_dllext;  // Platform specific shared lib suffix.
};


// ---------------------------------------------------------------------------
// wxDynamicLibrary
// ---------------------------------------------------------------------------

class wxDLManifestEntry
{
public:

    static wxDLImports ms_classes;  // Static hash of all imported classes.

    wxDLManifestEntry( const wxString &libname );
    ~wxDLManifestEntry();

    wxDLManifestEntry  *RefLib() { ++m_linkcount; return this; }
    bool                UnrefLib();

        // These two are called by the PluginSentinel on (PLUGGABLE) object
        // creation/destruction.  There is usually no reason for the user to
        // call them directly.  We have to separate this from the link count,
        // since the two are not interchangeable.

        // FIXME: for even better debugging PluginSentinel should register
        //        the name of the class created too, then we can state
        //        exactly which object was not destroyed which may be
        //        difficult to find otherwise.  Also this code should
        //        probably only be active in DEBUG mode, but let's just
        //        get it right first.

    void        RefObj() { ++m_objcount; }
    void        UnrefObj()
    {
        wxASSERT_MSG( m_objcount > 0, _T("Too many objects deleted??") );
        --m_objcount;
    }

    bool        IsLoaded() const { return m_linkcount > 0; }

    wxDllType   GetLinkHandle() const { return m_handle; }
    wxDllType   GetProgramHandle() const { return wxDllLoader::GetProgramHandle(); }
    void       *GetSymbol(const wxString &symbol, bool *success = 0)
    {
        return wxDllLoader::GetSymbol( m_handle, symbol, success );
    }

private:

        // Order of these three *is* important, do not change it

    wxClassInfo    *m_before;       // sm_first before loading this lib
    wxDllType       m_handle;       // Handle from dlopen.
    wxClassInfo    *m_after;        // ..and after.

    size_t          m_linkcount;    // Ref count of library link calls
    size_t          m_objcount;     // ..and (pluggable) object instantiations.
    wxModuleList    m_wxmodules;    // any wxModules that we initialised.

    void    UpdateClassInfo();      // Update the wxClassInfo table
    void    RestoreClassInfo();     // Restore the original wxClassInfo state.
    void    RegisterModules();      // Init any wxModules in the lib.
    void    UnregisterModules();    // Cleanup any wxModules we installed.

DECLARE_NO_COPY_CLASS(wxDLManifestEntry)
};


class WXDLLEXPORT wxDynamicLibrary
{
public:

        // Static accessors.

    static wxDLManifestEntry    *Link(const wxString &libname);
    static bool                  Unlink(const wxString &libname);

        // Instance methods.

    wxDynamicLibrary(const wxString &libname);
    ~wxDynamicLibrary();

    bool   IsLoaded() const { return m_entry && m_entry->IsLoaded(); }
    void  *GetSymbol(const wxString &symbol, bool *success = 0)
    {
        return m_entry->GetSymbol( symbol, success );
    }

private:

    static wxDLManifest  ms_manifest;  // Static hash of loaded libs.
    wxDLManifestEntry   *m_entry;      // Cache our entry in the manifest.

    // We could allow this class to be copied if we really
    // wanted to, but not without modification.

DECLARE_NO_COPY_CLASS(wxDynamicLibrary)
};


#endif  // wxUSE_DYNAMIC_LOADER
#endif  // _WX_DYNAMICLOADER_H__

// vi:sts=4:sw=4:et
