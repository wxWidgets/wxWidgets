/////////////////////////////////////////////////////////////////////////////
// Name:         dynload.h
// Purpose:      Dynamic loading framework
// Author:       Ron Lee, David Falkinder, Vadim Zeitlin and a cast of 1000's
//               (derived in part from dynlib.cpp (c) 1998 Guilhem Lavaux)
// Modified by:
// Created:      03/12/01
// RCS-ID:       $Id$
// Copyright:    (c) 2001 Ron Lee <ron@debian.org>
// Licence:      wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DYNAMICLOADER_H__
#define _WX_DYNAMICLOADER_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "dynload.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_DYNAMIC_LOADER

#include "wx/hashmap.h"
#include "wx/module.h"

// FIXME: can this go in private.h or something too??
#if defined(__WXPM__) || defined(__EMX__)
#define INCL_DOS
#include <os2.h>
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

class WXDLLEXPORT_BASE wxPluginLibrary;

WX_DECLARE_EXPORTED_STRING_HASH_MAP(wxPluginLibrary *, wxDLManifest);
typedef wxDLManifest wxDLImports;

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

    // Note: WXPM/EMX has to be tested first, since we want to use
    // native version, even if configure detected presence of DLOPEN.

#if defined(__WXPM__) || defined(__EMX__) || defined(__WINDOWS__)
typedef HMODULE             wxDllType;
#elif defined(HAVE_DLOPEN)
#include <dlfcn.h>
typedef void               *wxDllType;
#elif defined(HAVE_SHL_LOAD)
#include <dl.h>
typedef shl_t               wxDllType;
#elif defined(__DARWIN__)
typedef void               *wxDllType;
#elif defined(__WXMAC__)
typedef CFragConnectionID   wxDllType;
#else
#error "Dynamic Loading classes can't be compiled on this platform, sorry."
#endif


// ---------------------------------------------------------------------------
// wxDynamicLibrary
// ---------------------------------------------------------------------------

//FIXME:  This class isn't really common at all, it should be moved
//        into platform dependent files.

// NOTE: this class is (deliberately) not virtual, do not attempt
//       to use it polymorphically.

enum wxDLFlags
{
    wxDL_LAZY       = 0x00000001,   // resolve undefined symbols at first use
    wxDL_NOW        = 0x00000002,   // resolve undefined symbols on load
    wxDL_GLOBAL     = 0x00000004,   // export extern symbols to subsequently
                                    // loaded libs.
    wxDL_VERBATIM   = 0x00000008,   // Attempt to load the supplied library
                                    // name without appending the usual dll
                                    // filename extension.

    wxDL_NOSHARE    = 0x00000010,   // load new DLL, don't reuse already loaded

    // FIXME: why? (VZ)
#ifdef __osf__
    wxDL_DEFAULT    = wxDL_LAZY
#else
    wxDL_DEFAULT    = wxDL_LAZY | wxDL_GLOBAL
#endif
};


class WXDLLEXPORT_BASE wxDynamicLibrary
{
public:

        // return a valid handle for the main program itself or NULL if
        // back linking is not supported by the current platform (e.g. Win32)

    static wxDllType         GetProgramHandle();

        // return the platform standard DLL extension (with leading dot)

    static const wxChar *GetDllExt() { return ms_dllext; }

    wxDynamicLibrary() : m_handle(0) {}
    wxDynamicLibrary(wxString libname, int flags = wxDL_DEFAULT)
        : m_handle(0)
    {
        Load(libname, flags);
    }
    ~wxDynamicLibrary() { Unload(); }

        // return TRUE if the library was loaded successfully

    bool IsLoaded() const { return m_handle != 0; }

        // load the library with the given name
        // (full or not), return TRUE on success

    bool Load(wxString libname, int flags = wxDL_DEFAULT);

        // detach the library object from its handle, i.e. prevent the object
        // from unloading the library in its dtor -- the caller is now
        // responsible for doing this
    wxDllType Detach() { wxDllType h = m_handle; m_handle = 0; return h; }

        // unload the library, also done automatically in dtor

    void Unload();

        // Return the raw handle from dlopen and friends.

    wxDllType GetLibHandle() const { return m_handle; }

        // resolve a symbol in a loaded DLL, such as a variable or function
        // name.  'name' is the (possibly mangled) name of the symbol.
        // (use extern "C" to export unmangled names)
        //
        // Since it is perfectly valid for the returned symbol to actually be
        // NULL, that is not always indication of an error.  Pass and test the
        // parameter 'success' for a true indication of success or failure to
        // load the symbol.
        //
        // Returns a pointer to the symbol on success, or NULL if an error
        // occurred or the symbol wasn't found.

    void *GetSymbol(const wxString& name, bool *success = 0) const;

#if WXWIN_COMPATIBILITY_2_2
    operator bool() const { return IsLoaded(); }
#endif

protected:

        // Platform specific shared lib suffix.

    static const wxChar *ms_dllext;

        // the handle to DLL or NULL

    wxDllType m_handle;

        // no copy ctor/assignment operators
        // or we'd try to unload the library twice

    DECLARE_NO_COPY_CLASS(wxDynamicLibrary)
};


// ---------------------------------------------------------------------------
// wxPluginLibrary
// ---------------------------------------------------------------------------

// NOTE: Do not attempt to use a base class pointer to this class.
//       wxDL is not virtual and we deliberately hide some of it's
//       methods here.
//
//       Unless you know exacty why you need to, you probably shouldn't
//       instantiate this class directly anyway, use wxPluginManager
//       instead.

class WXDLLEXPORT_BASE wxPluginLibrary : public wxDynamicLibrary
{
public:

    static wxDLImports* ms_classes;  // Static hash of all imported classes.

    wxPluginLibrary( const wxString &libname, int flags = wxDL_DEFAULT );
    ~wxPluginLibrary();

    wxPluginLibrary  *RefLib();
    bool              UnrefLib();

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

    void  RefObj() { ++m_objcount; }
    void  UnrefObj()
    {
        wxASSERT_MSG( m_objcount > 0, _T("Too many objects deleted??") );
        --m_objcount;
    }

        // Override/hide some base class methods

    bool  IsLoaded() const { return m_linkcount > 0; }
    void  Unload() { UnrefLib(); }

private:

    wxClassInfo    *m_before;       // sm_first before loading this lib
    wxClassInfo    *m_after;        // ..and after.

    size_t          m_linkcount;    // Ref count of library link calls
    size_t          m_objcount;     // ..and (pluggable) object instantiations.
    wxModuleList    m_wxmodules;    // any wxModules that we initialised.

    void    UpdateClassInfo();      // Update the wxClassInfo table
    void    RestoreClassInfo();     // Restore the original wxClassInfo state.
    void    RegisterModules();      // Init any wxModules in the lib.
    void    UnregisterModules();    // Cleanup any wxModules we installed.

    DECLARE_NO_COPY_CLASS(wxPluginLibrary)
};


class WXDLLEXPORT_BASE wxPluginManager
{
public:

        // Static accessors.

    static wxPluginLibrary    *LoadLibrary( const wxString &libname,
                                            int flags = wxDL_DEFAULT );
    static bool                UnloadLibrary(const wxString &libname);

        // This is used by wxDllLoader.  It's wrapped in the compatibility
        // macro because it's of arguable use outside of that.

#if WXWIN_COMPATIBILITY_2_2
    static wxPluginLibrary *GetObjectFromHandle(wxDllType handle);
#endif

        // Instance methods.

    wxPluginManager() : m_entry(NULL) {};
    wxPluginManager(const wxString &libname, int flags = wxDL_DEFAULT)
    {
        Load(libname, flags);
    }
    ~wxPluginManager() { Unload(); }

    bool   Load(const wxString &libname, int flags = wxDL_DEFAULT);
    void   Unload();

    bool   IsLoaded() const { return m_entry && m_entry->IsLoaded(); }
    void  *GetSymbol(const wxString &symbol, bool *success = 0)
    {
        return m_entry->GetSymbol( symbol, success );
    }

    static void CreateManifest() { ms_manifest = new wxDLManifest(wxKEY_STRING); }
    static void ClearManifest() { delete ms_manifest; ms_manifest = NULL; }

private:
    // return the pointer to the entry for the library with given name in
    // ms_manifest or NULL if none
    static wxPluginLibrary *FindByName(const wxString& name)
    {
        const wxDLManifest::iterator i = ms_manifest->find(name);

        return i == ms_manifest->end() ? NULL : i->second;
    }

    static wxDLManifest* ms_manifest;  // Static hash of loaded libs.
    wxPluginLibrary*     m_entry;      // Cache our entry in the manifest.

    // We could allow this class to be copied if we really
    // wanted to, but not without modification.
    DECLARE_NO_COPY_CLASS(wxPluginManager)
};


// ---------------------------------------------------------------------------
// wxDllLoader
// ---------------------------------------------------------------------------

    //  Cross platform wrapper for dlopen and friends.
    //  There are no instances of this class, it simply
    //  serves as a namespace for its static member functions.

#if WXWIN_COMPATIBILITY_2_2
class WXDLLEXPORT_BASE wxDllLoader
{
public:

    static wxDllType    LoadLibrary(const wxString& name, bool *success = NULL);
    static void         UnloadLibrary(wxDllType dll);

    static wxDllType GetProgramHandle() { return wxDynamicLibrary::GetProgramHandle(); }

    static void *GetSymbol(wxDllType dllHandle, const wxString &name, bool *success = 0);

    static wxString GetDllExt() { return wxDynamicLibrary::GetDllExt(); }

private:

    wxDllLoader();                    // forbid construction of objects
};
#endif

#endif  // wxUSE_DYNAMIC_LOADER
#endif  // _WX_DYNAMICLOADER_H__

