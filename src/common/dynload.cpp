/////////////////////////////////////////////////////////////////////////////
// Name:         dynload.cpp
// Purpose:      Dynamic loading framework
// Author:       Ron Lee, David Falkinder, Vadim Zeitlin and a cast of 1000's
//               (derived in part from dynlib.cpp (c) 1998 Guilhem Lavaux)
// Modified by:
// Created:      03/12/01
// RCS-ID:       $Id$
// Copyright:    (c) 2001 Ron Lee <ron@debian.org>
// Licence:      wxWindows license
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
#endif

#include "wx/dynload.h"

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

#if defined(__WXPM__) || defined(__EMX__)
#define INCL_DOS
#include <os2.h>
#define wxDllOpen(error, lib, handle)   DosLoadModule(error, sizeof(error), lib, &handle)
#define wxDllGetSymbol(handle, modaddr) DosQueryProcAddr(handle, 1L, NULL, (PFN*)modaddr)
#define wxDllClose(handle)              DosFreeModule(handle)

#elif defined(HAVE_DLOPEN)
    // note about dlopen() flags: we use RTLD_NOW to have more Windows-like
    // behaviour (Win won't let you load a library with missing symbols) and
    // RTLD_GLOBAL because it is needed sometimes and probably doesn't hurt
    // otherwise. On True64-Unix RTLD_GLOBAL is not allowed and on VMS the
    // second argument on dlopen is ignored.

#ifdef __VMS
#define wxDllOpen(lib)                dlopen(lib.fn_str(), 0)

#elif defined( __osf__ )
#define wxDllOpen(lib)                dlopen(lib.fn_str(), RTLD_LAZY)

#else
#define wxDllOpen(lib)                dlopen(lib.fn_str(), RTLD_LAZY | RTLD_GLOBAL)
#endif  // __VMS

#define wxDllGetSymbol(handle, name)  dlsym(handle, name)
#define wxDllClose                    dlclose

#elif defined(HAVE_SHL_LOAD)
#define wxDllOpen(lib)                shl_load(lib.fn_str(), BIND_DEFERRED, 0)
#define wxDllClose                    shl_unload

static inline void *wxDllGetSymbol(shl_t handle, const wxString& name)
{
    void *sym;
    return ( shl_findsym(&handle, name.mb_str(), TYPE_UNDEFINED, &sym) == 0 )
           ? sym : 0;
}

#elif defined(__DARWIN__)

    // Porting notes:
    //   The dlopen port is a port from dl_next.xs by Anno Siegel.
    //   dl_next.xs is itself a port from dl_dlopen.xs by Paul Marquess.
    //   The method used here is just to supply the sun style dlopen etc.
    //   functions in terms of Darwin NS*.

void        *dlopen(const char *path, int mode);        // mode is ignored
void        *dlsym(void *handle, const char *symbol);
int          dlclose(void *handle);
const char  *dlerror(void);

#define wxDllOpen(lib)                  dlopen(lib.fn_str(), 0)
#define wxDllGetSymbol(handle, name)    dlsym(handle, name)
#define wxDllClose                      dlclose

#elif defined(__WINDOWS__)

    // using LoadLibraryEx under Win32 to avoid name clash with LoadLibrary

#ifdef __WIN32__

#ifdef _UNICODE
#define wxDllOpen(lib)                  ::LoadLibraryExW(lib, 0, 0)
#else
#define wxDllOpen(lib)                  ::LoadLibraryExA(lib, 0, 0)
#endif

#else   // Win16
#define wxDllOpen(lib)                  ::LoadLibrary(lib)
#endif  // Win32/16
#define wxDllGetSymbol(handle, name)    ::GetProcAddress(handle, name)
#define wxDllClose                      ::FreeLibrary

#elif defined(__WXMAC__)
#define wxDllClose(handle)              CloseConnection(&handle)
#else
#error "Don't know how to load shared libraries on this platform."
#endif


// ============================================================================
// implementation
// ============================================================================

// ---------------------------------------------------------------------------
// wxDllLoader   (all these methods are static)
// ---------------------------------------------------------------------------


#if defined(__WINDOWS__) || defined(__WXPM__) || defined(__EMX__)
const wxString wxDllLoader::ms_dllext( _T(".dll") );
#elif defined(__UNIX__)
#if defined(__HPUX__)
const wxString wxDllLoader::ms_dllext( _T(".sl") );
#else
const wxString wxDllLoader::ms_dllext( _T(".so") );
#endif
#endif

wxDllType wxDllLoader::GetProgramHandle()
{
#if defined( HAVE_DLOPEN ) && !defined(__EMX__)
   // obtain handle for main program
   return dlopen(NULL, RTLD_NOW/*RTLD_LAZY*/);
#elif defined (HAVE_SHL_LOAD)
   // shl_findsymbol with NULL handle looks up in main program
   return 0;
#else
   wxFAIL_MSG( wxT("This method is not implemented under Windows or OS/2"));
   return 0;
#endif
}

wxDllType wxDllLoader::LoadLibrary(const wxString &name)
{
    wxString    libname( name + wxDllLoader::GetDllExt() );
    wxDllType   handle;

#if defined(__WXMAC__) && !defined(__UNIX__)
    FSSpec      myFSSpec;
    Ptr         myMainAddr;
    Str255      myErrName;

    wxMacFilename2FSSpec( libname , &myFSSpec );

    if( GetDiskFragment( &myFSSpec,
                         0,
                         kCFragGoesToEOF,
                         "\p",
                         kPrivateCFragCopy,
                         &handle,
                         &myMainAddr,
                         myErrName ) != noErr )
    {
        p2cstr( myErrName );
        wxLogSysError( _("Failed to load shared library '%s' Error '%s'"),
                       libname.c_str(),
                       (char*)myErrName );
        handle = 0;
    }

#elif defined(__WXPM__) || defined(__EMX__)
    char        zError[256] = "";
    wxDllOpen(zError, libname, handle);
#else
    handle = wxDllOpen(libname);
#endif
    if ( !handle )
    {
        wxString msg(_("Failed to load shared library '%s'"));
#ifdef HAVE_DLERROR
        wxChar  *err = dlerror();
        if( err )
            wxLogError( msg, err );
#else
        wxLogSysError( msg, libname.c_str() );
#endif
    }
    return handle;
}

void wxDllLoader::UnloadLibrary(wxDllType handle)
{
   wxDllClose(handle);
}

void *wxDllLoader::GetSymbol(wxDllType dllHandle, const wxString &name, bool *success)
{
    bool     failed = FALSE;
    void    *symbol = 0;

#if defined(__WXMAC__) && !defined(__UNIX__)
    Ptr                 symAddress;
    CFragSymbolClass    symClass;
    Str255              symName;
#if TARGET_CARBON
    c2pstrcpy( (StringPtr) symName, name );
#else
    strcpy( (char *) symName, name );
    c2pstr( (char *) symName );
#endif
    if( FindSymbol( dllHandle, symName, &symAddress, &symClass ) == noErr )
        symbol = (void *)symAddress;

#elif defined(__WXPM__) || defined(__EMX__)
    wxDllGetSymbol(dllHandle, symbol);
#else

    // mb_str() is necessary in Unicode build
    symbol = wxDllGetSymbol(dllHandle, name.mb_str());
#endif

    if ( !symbol )
    {
        wxString msg(_("wxDllLoader failed to GetSymbol '%s'"));
#ifdef HAVE_DLERROR
        wxChar  *err = dlerror();
        if( err )
        {
            failed = TRUE;
            wxLogError( msg, err );
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
// wxDLManifestEntry
// ---------------------------------------------------------------------------


wxDLImports  wxDLManifestEntry::ms_classes(wxKEY_STRING);

wxDLManifestEntry::wxDLManifestEntry( const wxString &libname )
        : m_before(wxClassInfo::sm_first)
        , m_handle(wxDllLoader::LoadLibrary( libname ))
        , m_after(wxClassInfo::sm_first)
        , m_linkcount(1)
        , m_objcount(0)
{
    if( m_handle != 0 )
    {
        UpdateClassInfo();
        RegisterModules();
    }
    else
        --m_linkcount;      // Flag us for deletion
}

wxDLManifestEntry::~wxDLManifestEntry()
{
    UnregisterModules();
    RestoreClassInfo();

    wxDllLoader::UnloadLibrary(m_handle);
}

bool wxDLManifestEntry::UnrefLib()
{
    wxASSERT_MSG( m_objcount == 0, _T("Library unloaded before all objects were destroyed") );
    if( m_linkcount == 0 || --m_linkcount == 0 )
    {
        delete this;
        return TRUE;
    }
    return FALSE;
}

// ------------------------
// Private methods
// ------------------------

void wxDLManifestEntry::UpdateClassInfo()
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

            if( ms_classes.Get(info->m_className) == 0 )
                ms_classes.Put(info->m_className, (wxObject *) this);
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

void wxDLManifestEntry::RestoreClassInfo()
{
    wxClassInfo *info;

    for(info = m_after; info != m_before; info = info->m_next)
    {
        wxClassInfo::sm_classTable->Delete(info->m_className);
        ms_classes.Delete(info->m_className);
    }

    if( wxClassInfo::sm_first == m_after )
        wxClassInfo::sm_first = m_before;
    else
    {
        info = wxClassInfo::sm_first;
        while( info->m_next && info->m_next != m_after ) info = info->m_next;

        wxASSERT_MSG( info, _T("ClassInfo from wxDynamicLibrary not found on purge"))

        info->m_next = m_before;
    }
}

void wxDLManifestEntry::RegisterModules()
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

    for(wxClassInfo *info = m_after; info != m_before; info = info->m_next)
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

    for(wxModuleList::Node *node = m_wxmodules.GetFirst(); node; node->GetNext())
    {
        if( !node->GetData()->Init() )
        {
            wxLogDebug(_T("wxModule::Init() failed for wxDynamicLibrary"));

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

void wxDLManifestEntry::UnregisterModules()
{
    wxModuleList::Node  *node;

    for(node = m_wxmodules.GetFirst(); node; node->GetNext())
        node->GetData()->Exit();

    for(node = m_wxmodules.GetFirst(); node; node->GetNext())
        wxModule::UnregisterModule( node->GetData() );

    m_wxmodules.DeleteContents(TRUE);
}


// ---------------------------------------------------------------------------
// wxDynamicLibrary
// ---------------------------------------------------------------------------

wxDLManifest   wxDynamicLibrary::ms_manifest(wxKEY_STRING);

// ------------------------
// Static accessors
// ------------------------

wxDLManifestEntry *wxDynamicLibrary::Link(const wxString &libname)
{
    wxDLManifestEntry *entry = (wxDLManifestEntry*) ms_manifest.Get(libname);

    if( entry )
    {
        entry->RefLib();
    }
    else
    {
        entry = new wxDLManifestEntry( libname );

        if( entry->IsLoaded() )
        {
            ms_manifest.Put(libname, (wxObject*) entry);
        }
        else
        {
            wxCHECK_MSG( !entry->UnrefLib(), 0,
                         _T("Currently linked library is, ..not loaded??") );
            entry = 0;
        }
    }
    return entry;
}

bool wxDynamicLibrary::Unlink(const wxString &libname)
{
    wxDLManifestEntry *entry = (wxDLManifestEntry*) ms_manifest.Get(libname);

    if( entry )
        return entry->UnrefLib();

    wxLogDebug(_T("Attempt to Unlink library '%s' (which is not linked)."), libname.c_str());
    return 0;
}

// ------------------------
// Class implementation
// ------------------------

wxDynamicLibrary::wxDynamicLibrary(const wxString &libname)
{
    m_entry = (wxDLManifestEntry*) ms_manifest.Get(libname);

    if( m_entry != 0 )
    {
        m_entry->RefLib();
    }
    else
    {
        m_entry = new wxDLManifestEntry( libname );
        ms_manifest.Put(libname, (wxObject*) m_entry);

        wxASSERT_MSG( m_entry != 0, _T("Failed to create manifest entry") );
    }
}

wxDynamicLibrary::~wxDynamicLibrary()
{
    wxNode  *node;
    ms_manifest.BeginFind();

    // It's either this or store the name of the lib just to do this.

    for(node = ms_manifest.Next(); node; node = ms_manifest.Next())
        if( (wxDLManifestEntry*)node->GetData() == m_entry )
            break;

    if( m_entry && m_entry->UnrefLib() )
        delete node;
}


#ifdef __DARWIN__
// ---------------------------------------------------------------------------
// For Darwin/Mac OS X
//   supply the sun style dlopen functions in terms of Darwin NS*
// ---------------------------------------------------------------------------

extern "C" {
#import <mach-o/dyld.h>
};

enum dyldErrorSource
{
    OFImage,
};

static char dl_last_error[1024];

static
void TranslateError(const char *path, enum dyldErrorSource type, int number)
{
    unsigned int index;
    static char *OFIErrorStrings[] =
    {
	"%s(%d): Object Image Load Failure\n",
	"%s(%d): Object Image Load Success\n",
	"%s(%d): Not an recognisable object file\n",
	"%s(%d): No valid architecture\n",
	"%s(%d): Object image has an invalid format\n",
	"%s(%d): Invalid access (permissions?)\n",
	"%s(%d): Unknown error code from NSCreateObjectFileImageFromFile\n",
    };
#define NUM_OFI_ERRORS (sizeof(OFIErrorStrings) / sizeof(OFIErrorStrings[0]))

    switch (type)
    {
     case OFImage:
	 index = number;
	 if (index > NUM_OFI_ERRORS - 1) {
	     index = NUM_OFI_ERRORS - 1;
	 }
	 sprintf(dl_last_error, OFIErrorStrings[index], path, number);
	 break;
	 
     default:
	 sprintf(dl_last_error, "%s(%d): Totally unknown error type %d\n",
		 path, number, type);
	 break;
    }
}

const char *dlerror()
{
    return dl_last_error;
}
void *dlopen(const char *path, int mode /* mode is ignored */)
{
    int                 dyld_result;
    NSObjectFileImage   ofile;
    NSModule            handle = 0;

    dyld_result = NSCreateObjectFileImageFromFile(path, &ofile);
    if(dyld_result != NSObjectFileImageSuccess)
    {
	TranslateError(path, OFImage, dyld_result);
    }
    else
    {
	// NSLinkModule will cause the run to abort on any link error's
	// not very friendly but the error recovery functionality is limited.
	handle = NSLinkModule(ofile, path, TRUE);
    }

    return handle;
}

int dlclose(void *handle) /* stub only */
{
    return 0;
}

void *dlsym(void *handle, const char *symbol)
{
    return NSIsSymbolNameDefined(symbol)
            ? NSAddressOfSymbol( NSLookupAndBindSymbol(symbol) )
            : 0 ;
}

#endif // __DARWIN__
#endif  // wxUSE_DYNAMIC_LOADER

// vi:sts=4:sw=4:et
