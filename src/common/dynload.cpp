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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
    #include "wx/utils.h"
#endif

#include "wx/strconv.h"

#include "wx/dynload.h"
#include "wx/module.h"


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
        wxPluginLibrary::ms_classes = new wxDLImports;
        wxPluginManager::CreateManifest();
        return true;
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
        UpdateClasses();
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
        RestoreClasses();
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

    if ( m_linkcount == 0 || --m_linkcount == 0 )
    {
        delete this;
        return true;
    }

    return false;
}

// ------------------------
// Private methods
// ------------------------

void wxPluginLibrary::UpdateClasses()
{
    for (wxClassInfo *info = m_after; info != m_before; info = info->m_next)
    {
        if( info->m_className )
        {
            // Hash all the class names into a local table too so
            // we can quickly find the entry they correspond to.
            (*ms_classes)[info->m_className] = this;
        }
    }
}

void wxPluginLibrary::RestoreClasses()
{
    // Check if there is a need to restore classes.
    if (!ms_classes)
        return;

    for(wxClassInfo *info = m_after; info != m_before; info = info->m_next)
    {
        ms_classes->erase(ms_classes->find(info->m_className));
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

            m_wxmodules.push_back(m);
            wxModule::RegisterModule(m);
        }
    }

    // FIXME: Likewise this is (well was) very similar to InitializeModules()

    for ( wxModuleList::iterator it = m_wxmodules.begin();
          it != m_wxmodules.end();
          ++it)
    {
        if( !(*it)->Init() )
        {
            wxLogDebug(_T("wxModule::Init() failed for wxPluginLibrary"));

            // XXX: Watch this, a different hash implementation might break it,
            //      a good hash implementation would let us fix it though.

            // The name of the game is to remove any uninitialised modules and
            // let the dtor Exit the rest on shutdown, (which we'll initiate
            // shortly).

            wxModuleList::iterator oldNode = m_wxmodules.end();
            do {
                ++it;
                if( oldNode != m_wxmodules.end() )
                    m_wxmodules.erase(oldNode);
                wxModule::UnregisterModule( *it );
                oldNode = it;
            } while( it != m_wxmodules.end() );

            --m_linkcount;     // Flag us for deletion
            break;
        }
    }
}

void wxPluginLibrary::UnregisterModules()
{
    wxModuleList::iterator it;

    for ( it = m_wxmodules.begin(); it != m_wxmodules.end(); ++it )
        (*it)->Exit();

    for ( it = m_wxmodules.begin(); it != m_wxmodules.end(); ++it )
        wxModule::UnregisterModule( *it );

    // NB: content of the list was deleted by UnregisterModule calls above:
    m_wxmodules.clear();
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

        return false;
    }

    wxLogTrace(_T("dll"), _T("UnloadLibrary(%s)"), realname.c_str());

    if ( !entry->UnrefLib() )
    {
        // not really unloaded yet
        return false;
    }

    ms_manifest->erase(ms_manifest->find(realname));

    return true;
}

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

// ---------------------------------------------------------------------------
// wxDllLoader   (all these methods are static)
// ---------------------------------------------------------------------------


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
            *success = false;

        return NULL;
    }

    return p->GetSymbol(name, success);
}


// ---------------------------------------------------------------------------
// Global variables
// ---------------------------------------------------------------------------

wxLibraries wxTheLibraries;

// ============================================================================
// implementation
// ============================================================================

// construct the full name from the base shared object name: adds a .dll
// suffix under Windows or .so under Unix
static wxString ConstructLibraryName(const wxString& basename)
{
    wxString fullname;
    fullname << basename << wxDllLoader::GetDllExt();

    return fullname;
}

// ---------------------------------------------------------------------------
// wxLibrary (one instance per dynamic library)
// ---------------------------------------------------------------------------

wxLibrary::wxLibrary(wxDllType handle)
{
    typedef wxClassInfo *(*t_get_first)(void);
    t_get_first get_first;

    m_handle = handle;

    // Some system may use a local heap for library.
    get_first = (t_get_first)GetSymbol(_T("wxGetClassFirst"));
    // It is a wxWidgets DLL.
    if (get_first)
        PrepareClasses(get_first());
}

wxLibrary::~wxLibrary()
{
    if ( m_handle )
    {
        wxDllLoader::UnloadLibrary(m_handle);
    }
}

wxObject *wxLibrary::CreateObject(const wxString& name)
{
    wxClassInfo *info = (wxClassInfo *)classTable.Get(name);

    if (!info)
        return NULL;

    return info->CreateObject();
}

void wxLibrary::PrepareClasses(wxClassInfo *first)
{
    // Index all class infos by their class name
    wxClassInfo *info = first;
    while (info)
    {
        if (info->m_className)
            classTable.Put(info->m_className, (wxObject *)info);
        info = info->m_next;
    }

#if !wxUSE_EXTENDED_RTTI
    // Set base pointers for each wxClassInfo
    info = first;
    while (info)
    {
        if (info->GetBaseClassName1())
            info->m_baseInfo1 = (wxClassInfo *)classTable.Get(info->GetBaseClassName1());
        if (info->GetBaseClassName2())
            info->m_baseInfo2 = (wxClassInfo *)classTable.Get(info->GetBaseClassName2());
        info = info->m_next;
    }
#endif
}

void *wxLibrary::GetSymbol(const wxString& symbname)
{
   return wxDllLoader::GetSymbol(m_handle, symbname);
}


// ---------------------------------------------------------------------------
// wxLibraries (only one instance should normally exist)
// ---------------------------------------------------------------------------

wxLibraries::wxLibraries():m_loaded(wxKEY_STRING)
{
}

wxLibraries::~wxLibraries()
{
    wxNode *node = m_loaded.First();

    while (node) {
        wxLibrary *lib = (wxLibrary *)node->Data();
        delete lib;

        node = node->Next();
    }
}

wxLibrary *wxLibraries::LoadLibrary(const wxString& name)
{
    wxLibrary   *lib;
    wxClassInfo *old_sm_first;
    wxNode      *node = m_loaded.Find(name.GetData());

    if (node != NULL)
        return ((wxLibrary *)node->Data());

    // If DLL shares data, this is necessary.
    old_sm_first = wxClassInfo::sm_first;
    wxClassInfo::sm_first = NULL;

    wxString libname = ConstructLibraryName(name);

    bool success = false;
    wxDllType handle = wxDllLoader::LoadLibrary(libname, &success);
    if(success)
    {
       lib = new wxLibrary(handle);
       wxClassInfo::sm_first = old_sm_first;

       m_loaded.Append(name.GetData(), lib);
    }
    else
       lib = NULL;
    return lib;
}

wxObject *wxLibraries::CreateObject(const wxString& path)
{
    wxNode *node = m_loaded.First();
    wxObject *obj;

    while (node) {
        obj = ((wxLibrary *)node->Data())->CreateObject(path);
        if (obj)
            return obj;

        node = node->Next();
    }
    return NULL;
}

#endif  // WXWIN_COMPATIBILITY_2_2


#endif  // wxUSE_DYNAMIC_LOADER

