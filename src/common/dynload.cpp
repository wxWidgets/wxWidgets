/////////////////////////////////////////////////////////////////////////////
// Name:         dynload.cpp
// Purpose:      Dynamic loading framework
// Author:       Ron Lee, David Falkinder, Vadim Zeitlin and a cast of 1000's
//               (derived in part from dynlib.cpp (c) 1998 Guilhem Lavaux)
// Modified by:
// Created:      03/12/01
// RCS-ID:       $Id$
// Copyright:    (c) 2001 Ron Lee <ron@debian.org>
// Licence:      wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dynload.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DYNAMIC_LOADER

#ifdef __WINDOWS__
    #include "wx/msw/private.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/hash.h"
#endif

#include "wx/filename.h"        // for SplitPath()
#include "wx/strconv.h"

#include "wx/dynload.h"
#include "wx/module.h"

#if defined(__DARWIN__)
/* Porting notes:
 *   The dlopen port is a port from dl_next.xs by Anno Siegel.
 *   dl_next.xs is itself a port from dl_dlopen.xs by Paul Marquess.
 *   The method used here is just to supply the sun style dlopen etc.
 *   functions in terms of Darwin NS*.
 */
void *dlopen(const char *path, int mode /* mode is ignored */);
void *dlsym(void *handle, const char *symbol);
int   dlclose(void *handle);
const char *dlerror(void);
#endif

// ============================================================================
// implementation
// ============================================================================

// ---------------------------------------------------------------------------
// wxDynamicLibrary
// ---------------------------------------------------------------------------

//FIXME:  This class isn't really common at all, it should be moved into
//        platform dependent files.

#if defined(__WINDOWS__) || defined(__WXPM__) || defined(__EMX__)
    const wxChar *wxDynamicLibrary::ms_dllext = _T(".dll");
#elif defined(__UNIX__)
    #if defined(__HPUX__)
        const wxChar *wxDynamicLibrary::ms_dllext = _T(".sl");
    #else
        const wxChar *wxDynamicLibrary::ms_dllext = _T(".so");
    #endif
#endif

wxDllType wxDynamicLibrary::GetProgramHandle()
{
#if defined( HAVE_DLOPEN ) && !defined(__EMX__)
   return dlopen(0, RTLD_LAZY);
#elif defined (HAVE_SHL_LOAD)
   return PROG_HANDLE;
#else
   wxFAIL_MSG( wxT("This method is not implemented under Windows or OS/2"));
   return 0;
#endif
}

bool wxDynamicLibrary::Load(wxString libname, int flags)
{
    wxASSERT_MSG(m_handle == 0, _T("Library already loaded."));

    // add the proper extension for the DLL ourselves unless told not to
    if ( !(flags & wxDL_VERBATIM) )
    {
        // and also check that the libname doesn't already have it
        wxString ext;
        wxFileName::SplitPath(libname, NULL, NULL, &ext);
        if ( ext.empty() )
        {
            libname += GetDllExt();
        }
    }

    // different ways to load a shared library
    //
    // FIXME: should go to the platform-specific files!
#if defined(__WXMAC__) && !defined(__DARWIN__)
    FSSpec      myFSSpec;
    Ptr         myMainAddr;
    Str255      myErrName;

    wxMacFilename2FSSpec( libname , &myFSSpec );

    if( GetDiskFragment( &myFSSpec,
                         0,
                         kCFragGoesToEOF,
                         "\p",
                         kPrivateCFragCopy,
                         &m_handle,
                         &myMainAddr,
                         myErrName ) != noErr )
    {
        p2cstr( myErrName );
        wxLogSysError( _("Failed to load shared library '%s' Error '%s'"),
                       libname.c_str(),
                       (char*)myErrName );
        m_handle = 0;
    }

#elif defined(__WXPM__) || defined(__EMX__)
    char    err[256] = "";
    DosLoadModule(err, sizeof(err), libname.c_str(), &m_handle);

#elif defined(HAVE_DLOPEN) || defined(__DARWIN__)

#if defined(__VMS) || defined(__DARWIN__)
    m_handle = dlopen(libname.c_str(), 0);  // The second parameter is ignored
#else // !__VMS  && !__DARWIN__
    int rtldFlags = 0;

    if ( flags & wxDL_LAZY )
    {
        wxASSERT_MSG( (flags & wxDL_NOW) == 0,
                      _T("wxDL_LAZY and wxDL_NOW are mutually exclusive.") );
#ifdef RTLD_LAZY
        rtldFlags |= RTLD_LAZY;
#else
        wxLogDebug(_T("wxDL_LAZY is not supported on this platform"));
#endif
    }
    else if ( flags & wxDL_NOW )
    {
#ifdef RTLD_NOW
        rtldFlags |= RTLD_NOW;
#else
        wxLogDebug(_T("wxDL_NOW is not supported on this platform"));
#endif
    }

    if ( flags & wxDL_GLOBAL )
    {
#ifdef RTLD_GLOBAL
        rtldFlags |= RTLD_GLOBAL;
#else
        wxLogDebug(_T("RTLD_GLOBAL is not supported on this platform."));
#endif
    }

    m_handle = dlopen(libname.fn_str(), rtldFlags);
#endif  // __VMS || __DARWIN__ ?

#elif defined(HAVE_SHL_LOAD)
    int shlFlags = 0;

    if( flags & wxDL_LAZY )
    {
        wxASSERT_MSG( (flags & wxDL_NOW) == 0,
                      _T("wxDL_LAZY and wxDL_NOW are mutually exclusive.") );
        shlFlags |= BIND_DEFERRED;
    }
    else if( flags & wxDL_NOW )
    {
        shlFlags |= BIND_IMMEDIATE;
    }
    m_handle = shl_load(libname.fn_str(), BIND_DEFERRED, 0);

#elif defined(__WINDOWS__)
    m_handle = ::LoadLibrary(libname.c_str());
#else
    #error  "runtime shared lib support not implemented on this platform"
#endif

    if ( m_handle == 0 )
    {
        wxString msg(_("Failed to load shared library '%s'"));
#if defined(HAVE_DLERROR) && !defined(__EMX__)

#if wxUSE_UNICODE
        wxWCharBuffer buffer = wxConvLocal.cMB2WC( dlerror() );
        const wxChar *err = buffer;
#else
        const wxChar *err = dlerror();
#endif

        if( err )
            wxLogError( msg, err );
#else
        wxLogSysError( msg, libname.c_str() );
#endif
    }

    return IsLoaded();
}

void wxDynamicLibrary::Unload()
{
    if( IsLoaded() )
    {
#if defined(__WXPM__) || defined(__EMX__)
        DosFreeModule( m_handle );
#elif defined(HAVE_DLOPEN) || defined(__DARWIN__)
        dlclose( m_handle );
#elif defined(HAVE_SHL_LOAD)
        shl_unload( m_handle );
#elif defined(__WINDOWS__)
        ::FreeLibrary( m_handle );
#elif defined(__WXMAC__) && !defined(__DARWIN__)
        CloseConnection( (CFragConnectionID*) &m_handle );
#else
#error  "runtime shared lib support not implemented"
#endif
        m_handle = 0;
    }
}

void *wxDynamicLibrary::GetSymbol(const wxString &name, bool *success) const
{
    wxCHECK_MSG( IsLoaded(), NULL,
                 _T("Can't load symbol from unloaded library") );

    bool     failed = FALSE;
    void    *symbol = 0;

#if defined(__WXMAC__) && !defined(__DARWIN__)
    Ptr                 symAddress;
    CFragSymbolClass    symClass;
    Str255              symName;
#if TARGET_CARBON
    c2pstrcpy( (StringPtr) symName, name );
#else
    strcpy( (char *)symName, name );
    c2pstr( (char *)symName );
#endif
    if( FindSymbol( dllHandle, symName, &symAddress, &symClass ) == noErr )
        symbol = (void *)symAddress;

#elif defined(__WXPM__) || defined(__EMX__)
    DosQueryProcAddr( m_handle, 1L, name.c_str(), (PFN*)symbol );

#elif defined(HAVE_DLOPEN) || defined(__DARWIN__)
    symbol = dlsym( m_handle, name.fn_str() );

#elif defined(HAVE_SHL_LOAD)
    // use local variable since shl_findsym modifies the handle argument
    // to indicate where the symbol was found (GD)
    wxDllType the_handle = m_handle;
    if( shl_findsym( &the_handle, name.fn_str(), TYPE_UNDEFINED, &symbol ) != 0 )
        symbol = 0;

#elif defined(__WINDOWS__)
    symbol = (void*) ::GetProcAddress( m_handle, name.mb_str() );

#else
#error  "runtime shared lib support not implemented"
#endif

    if ( !symbol )
    {
#if defined(HAVE_DLERROR) && !defined(__EMX__)

#if wxUSE_UNICODE
        wxWCharBuffer buffer = wxConvLocal.cMB2WC( dlerror() );
        const wxChar *err = buffer;
#else
        const wxChar *err = dlerror();
#endif

        if( err )
        {
            wxLogError(wxT("%s"), err);
        }
#else
        failed = TRUE;
        wxLogSysError(_("Couldn't find symbol '%s' in a dynamic library"),
                      name.c_str());
#endif
    }
    if( success )
        *success = !failed;

    return symbol;
}


// ---------------------------------------------------------------------------
// wxPluginLibrary
// ---------------------------------------------------------------------------


wxDLImports*  wxPluginLibrary::ms_classes = NULL;

class wxPluginLibraryModule : public wxModule
{
public:
    wxPluginLibraryModule() { }

    // TODO: create ms_classes on demand, why always preallocate it?
    virtual bool OnInit()
    {
        wxPluginLibrary::ms_classes = new wxDLImports(wxKEY_STRING);
        wxPluginManager::CreateManifest();
        return TRUE;
    }

    virtual void OnExit()
    {
        delete wxPluginLibrary::ms_classes;
        wxPluginLibrary::ms_classes = NULL;
        wxPluginManager::ClearManifest();
    }

private:
    DECLARE_DYNAMIC_CLASS(wxPluginLibraryModule )
};

IMPLEMENT_DYNAMIC_CLASS(wxPluginLibraryModule, wxModule)


wxPluginLibrary::wxPluginLibrary(const wxString &libname, int flags)
        : m_linkcount(1)
        , m_objcount(0)
{
    m_before = wxClassInfo::sm_first;
    Load( libname, flags );
    m_after = wxClassInfo::sm_first;

    if( m_handle != 0 )
    {
        UpdateClassInfo();
        RegisterModules();
    }
    else
    {
        // Flag us for deletion
        --m_linkcount;
    }
}

wxPluginLibrary::~wxPluginLibrary()
{
    if( m_handle != 0 )
    {
        UnregisterModules();
        RestoreClassInfo();
    }
}

wxPluginLibrary *wxPluginLibrary::RefLib()
{
    wxCHECK_MSG( m_linkcount > 0, NULL,
                 _T("Library had been already deleted!") );

    ++m_linkcount;
    return this;
}

bool wxPluginLibrary::UnrefLib()
{
    wxASSERT_MSG( m_objcount == 0,
                  _T("Library unloaded before all objects were destroyed") );

    if ( --m_linkcount == 0 )
    {
        delete this;
        return TRUE;
    }

    return FALSE;
}

// ------------------------
// Private methods
// ------------------------

void wxPluginLibrary::UpdateClassInfo()
{
    wxClassInfo     *info;
    wxHashTable     *t = wxClassInfo::sm_classTable;

        // FIXME: Below is simply a cut and paste specialisation of
        //        wxClassInfo::InitializeClasses.  Once this stabilises,
        //        the two should probably be merged.
        //
        //        Actually it's becoming questionable whether we should merge
        //        this info with the main ClassInfo tables since we can nearly
        //        handle this completely internally now and it does expose
        //        certain (minimal % user_stupidy) risks.

    for(info = m_after; info != m_before; info = info->m_next)
    {
        if( info->m_className )
        {
            if( t->Get(info->m_className) == 0 )
                t->Put(info->m_className, (wxObject *)info);

            // Hash all the class names into a local table too so
            // we can quickly find the entry they correspond to.
            (*ms_classes)[info->m_className] = this;
        }
    }

    for(info = m_after; info != m_before; info = info->m_next)
    {
        if( info->m_baseClassName1 )
            info->m_baseInfo1 = (wxClassInfo *)t->Get(info->m_baseClassName1);
        if( info->m_baseClassName2 )
            info->m_baseInfo2 = (wxClassInfo *)t->Get(info->m_baseClassName2);
    }
}

void wxPluginLibrary::RestoreClassInfo()
{
    wxClassInfo *info;

    for(info = m_after; info != m_before; info = info->m_next)
    {
        wxClassInfo::sm_classTable->Delete(info->m_className);
        ms_classes->erase(ms_classes->find(info->m_className));
    }

    if( wxClassInfo::sm_first == m_after )
        wxClassInfo::sm_first = m_before;
    else
    {
        info = wxClassInfo::sm_first;
        while( info->m_next && info->m_next != m_after ) info = info->m_next;

        wxASSERT_MSG( info, _T("ClassInfo from wxPluginLibrary not found on purge"));

        info->m_next = m_before;
    }
}

void wxPluginLibrary::RegisterModules()
{
    // Plugin libraries might have wxModules, Register and initialise them if
    // they do.
    //
    // Note that these classes are NOT included in the reference counting since
    // it's implicit that they will be unloaded if and when the last handle to
    // the library is.  We do have to keep a copy of the module's pointer
    // though, as there is currently no way to Unregister it without it.

    wxASSERT_MSG( m_linkcount == 1,
                  _T("RegisterModules should only be called for the first load") );

    for ( wxClassInfo *info = m_after; info != m_before; info = info->m_next)
    {
        if( info->IsKindOf(CLASSINFO(wxModule)) )
        {
            wxModule *m = wxDynamicCast(info->CreateObject(), wxModule);

            wxASSERT_MSG( m, _T("wxDynamicCast of wxModule failed") );

            m_wxmodules.Append(m);
            wxModule::RegisterModule(m);
        }
    }

    // FIXME: Likewise this is (well was) very similar to InitializeModules()

    for ( wxModuleList::Node *node = m_wxmodules.GetFirst();
          node;
          node = node->GetNext())
    {
        if( !node->GetData()->Init() )
        {
            wxLogDebug(_T("wxModule::Init() failed for wxPluginLibrary"));

            // XXX: Watch this, a different hash implementation might break it,
            //      a good hash implementation would let us fix it though.

            // The name of the game is to remove any uninitialised modules and
            // let the dtor Exit the rest on shutdown, (which we'll initiate
            // shortly).

            wxModuleList::Node *oldNode = 0;
            do {
                node = node->GetNext();
                delete oldNode;
                wxModule::UnregisterModule( node->GetData() );
                oldNode = node;
            } while( node );

            --m_linkcount;     // Flag us for deletion
            break;
        }
    }
}

void wxPluginLibrary::UnregisterModules()
{
    wxModuleList::Node  *node;

    for ( node = m_wxmodules.GetFirst(); node; node = node->GetNext() )
        node->GetData()->Exit();

    for ( node = m_wxmodules.GetFirst(); node; node = node->GetNext() )
        wxModule::UnregisterModule( node->GetData() );

    m_wxmodules.DeleteContents(TRUE);
}


// ---------------------------------------------------------------------------
// wxPluginManager
// ---------------------------------------------------------------------------

wxDLManifest*   wxPluginManager::ms_manifest = NULL;

// ------------------------
// Static accessors
// ------------------------

wxPluginLibrary *
wxPluginManager::LoadLibrary(const wxString &libname, int flags)
{
    wxString realname(libname);

    if( !(flags & wxDL_VERBATIM) )
        realname += wxDynamicLibrary::GetDllExt();

    wxPluginLibrary *entry;

    if ( flags & wxDL_NOSHARE )
    {
        entry = NULL;
    }
    else
    {
        entry = FindByName(realname);
    }

    if ( entry )
    {
        wxLogTrace(_T("dll"),
                   _T("LoadLibrary(%s): already loaded."), realname.c_str());

        entry->RefLib();
    }
    else
    {
        entry = new wxPluginLibrary( libname, flags );

        if ( entry->IsLoaded() )
        {
            (*ms_manifest)[realname] = entry;

            wxLogTrace(_T("dll"),
                       _T("LoadLibrary(%s): loaded ok."), realname.c_str());

        }
        else
        {
            wxLogTrace(_T("dll"),
                       _T("LoadLibrary(%s): failed to load."), realname.c_str());

            // we have created entry just above
            if ( !entry->UnrefLib() )
            {
                // ... so UnrefLib() is supposed to delete it
                wxFAIL_MSG( _T("Currently linked library is not loaded?") );
            }

            entry = NULL;
        }
    }

    return entry;
}

bool wxPluginManager::UnloadLibrary(const wxString& libname)
{
    wxString realname = libname;

    wxPluginLibrary *entry = FindByName(realname);

    if ( !entry )
    {
        realname += wxDynamicLibrary::GetDllExt();

        entry = FindByName(realname);
    }

    if ( !entry )
    {
        wxLogDebug(_T("Attempt to unload library '%s' which is not loaded."),
                   libname.c_str());

        return FALSE;
    }

    wxLogTrace(_T("dll"), _T("UnloadLibrary(%s)"), realname.c_str());

    if ( !entry->UnrefLib() )
    {
        // not really unloaded yet
        return FALSE;
    }

    ms_manifest->erase(ms_manifest->find(realname));

    return TRUE;
}

#if WXWIN_COMPATIBILITY_2_2
wxPluginLibrary *wxPluginManager::GetObjectFromHandle(wxDllType handle)
{
    for ( wxDLManifest::iterator i = ms_manifest->begin();
          i != ms_manifest->end();
          ++i )
    {
        wxPluginLibrary * const lib = i->second;

        if ( lib->GetLibHandle() == handle )
            return lib;
    }

    return NULL;
}
#endif // WXWIN_COMPATIBILITY_2_2

// ------------------------
// Class implementation
// ------------------------

bool wxPluginManager::Load(const wxString &libname, int flags)
{
    m_entry = wxPluginManager::LoadLibrary(libname, flags);

    return IsLoaded();
}

void wxPluginManager::Unload()
{
    wxCHECK_RET( m_entry, _T("unloading an invalid wxPluginManager?") );

    for ( wxDLManifest::iterator i = ms_manifest->begin();
          i != ms_manifest->end();
          ++i )
    {
        if ( i->second == m_entry )
        {
            ms_manifest->erase(i);
            break;
        }
    }

    m_entry->UnrefLib();

    m_entry = NULL;
}

// ---------------------------------------------------------------------------
// wxDllLoader   (all these methods are static)
// ---------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_2_2

wxDllType wxDllLoader::LoadLibrary(const wxString &name, bool *success)
{
    wxPluginLibrary *p = wxPluginManager::LoadLibrary
                         (
                            name,
                            wxDL_DEFAULT | wxDL_VERBATIM | wxDL_NOSHARE
                         );

    if ( success )
        *success = p != NULL;

    return p ? p->GetLibHandle() : 0;
}

void wxDllLoader::UnloadLibrary(wxDllType handle)
{
    wxPluginLibrary *p = wxPluginManager::GetObjectFromHandle(handle);

    wxCHECK_RET( p, _T("Unloading a library not loaded with wxDllLoader?") );

    p->UnrefLib();
}

void *
wxDllLoader::GetSymbol(wxDllType dllHandle, const wxString &name, bool *success)
{
    wxPluginLibrary *p = wxPluginManager::GetObjectFromHandle(dllHandle);

    if ( !p )
    {
        wxFAIL_MSG( _T("Using a library not loaded with wxDllLoader?") );

        if ( success )
            *success = FALSE;

        return NULL;
    }

    return p->GetSymbol(name, success);
}

#endif  // WXWIN_COMPATIBILITY_2_2

#endif  // wxUSE_DYNAMIC_LOADER

