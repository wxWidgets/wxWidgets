/////////////////////////////////////////////////////////////////////////////
// Name:        dynlib.cpp
// Purpose:     Dynamic library management
// Author:      Guilhem Lavaux
// Modified by:
// Created:     20/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
#   pragma implementation "dynlib.h"
#endif

#include  "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_DYNLIB_CLASS && !wxUSE_DYNAMIC_LOADER

#if defined(__WINDOWS__)
    #include "wx/msw/private.h"
#endif

#include "wx/dynlib.h"
#include "wx/filefn.h"
#include "wx/intl.h"
#include "wx/log.h"

#if defined(__WXMAC__)
    #include "wx/mac/private.h"
#endif

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

#if defined(__WXPM__) || defined(__EMX__)
#  define INCL_DOS
#  include <os2.h>
#  define wxDllOpen(error, lib, handle)     DosLoadModule(error, sizeof(error), lib, &handle)
#  define wxDllGetSymbol(handle, modaddr)   DosQueryProcAddr(handle, 1L, NULL, (PFN*)modaddr)
#  define wxDllClose(handle)                DosFreeModule(handle)
#elif defined(HAVE_DLOPEN)
    // note about dlopen() flags: we use RTLD_NOW to have more Windows-like
    // behaviour (Win won't let you load a library with missing symbols) and
    // RTLD_GLOBAL because it is needed sometimes and probably doesn't hurt
    // otherwise. On True64-Unix RTLD_GLOBAL is not allowed and on VMS the
    // second argument on dlopen is ignored.
#ifdef __VMS
# define wxDllOpen(lib)                dlopen(lib.fn_str(), 0 )
#elif defined( __osf__ )
# define wxDllOpen(lib)                dlopen(lib.fn_str(), RTLD_LAZY )
#else
# define wxDllOpen(lib)                dlopen(lib.fn_str(), RTLD_LAZY | RTLD_GLOBAL)
#endif
#define wxDllGetSymbol(handle, name)  dlsym(handle, name)
#   define wxDllClose                    dlclose
#elif defined(HAVE_SHL_LOAD)
#   define wxDllOpen(lib)                shl_load(lib.fn_str(), BIND_DEFERRED, 0)
#   define wxDllClose                    shl_unload

static inline void *wxDllGetSymbol(shl_t handle, const wxString& name)
{
    void *sym;
    if ( shl_findsym(&handle, name.mb_str(), TYPE_UNDEFINED, &sym) == 0 )
        return sym;
    else
        return 0;
}

#elif defined(__DARWIN__)
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

#   define wxDllOpen(lib)                dlopen(lib.fn_str(), 0)
#   define wxDllGetSymbol(handle, name)  dlsym(handle, name)
#   define wxDllClose                    dlclose
#elif defined(__WINDOWS__)
    // using LoadLibraryEx under Win32 to avoid name clash with LoadLibrary
#   ifdef __WIN32__
#ifdef _UNICODE
#ifdef __WXWINCE__
#      define wxDllOpen(lib)                  ::LoadLibrary(lib)
#else
#      define wxDllOpen(lib)                  ::LoadLibraryExW(lib, 0, 0)
#endif
#else
#      define wxDllOpen(lib)                  ::LoadLibraryExA(lib, 0, 0)
#endif
#   else   // Win16
#      define wxDllOpen(lib)                  ::LoadLibrary(lib)
#   endif  // Win32/16
#   define wxDllGetSymbol(handle, name)    ::GetProcAddress(handle, name)
#   define wxDllClose                      ::FreeLibrary
#elif defined(__WXMAC__)
#   define wxDllClose(handle)               CloseConnection(&((CFragConnectionID)handle))
#else
#   error "Don't know how to load shared libraries on this platform."
#endif // OS

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
    // It is a wxWindows DLL.
    if (get_first)
        PrepareClasses(get_first());
}

wxLibrary::~wxLibrary()
{
    if ( m_handle )
    {
        wxDllClose(m_handle);
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
}

void *wxLibrary::GetSymbol(const wxString& symbname)
{
   return wxDllLoader::GetSymbol(m_handle, symbname);
}

// ---------------------------------------------------------------------------
// wxDllLoader
// ---------------------------------------------------------------------------


#if defined(__WINDOWS__) || defined(__WXPM__) || defined(__EMX__)
const wxString wxDllLoader::ms_dllext( _T(".dll") );
#elif defined(__UNIX__)
#if defined(__HPUX__)
const wxString wxDllLoader::ms_dllext( _T(".sl") );
#else
const wxString wxDllLoader::ms_dllext( _T(".so") );
#endif
#elif defined(__WXMAC__)
const wxString wxDllLoader::ms_dllext( _T("") );
#endif

/* static */
wxDllType wxDllLoader::GetProgramHandle()
{
#if defined( HAVE_DLOPEN ) && !defined(__EMX__)
   // optain handle for main program
   return dlopen(NULL, RTLD_NOW/*RTLD_LAZY*/);
#elif defined (HAVE_SHL_LOAD)
   // shl_findsymbol with NULL handle looks up in main program
   return 0;
#else
   wxFAIL_MSG( wxT("This method is not implemented under Windows or OS/2"));
   return 0;
#endif
}

/* static */
wxDllType wxDllLoader::LoadLibrary(const wxString & libname, bool *success)
{
    wxDllType   handle;
    bool        failed = FALSE;

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
                         &((CFragConnectionID)handle),
                         &myMainAddr,
                         myErrName ) != noErr )
    {
        p2cstr( myErrName );
        wxLogSysError( _("Failed to load shared library '%s' Error '%s'"),
                       libname.c_str(),
                       (char*)myErrName );
        handle = 0;
        failed = TRUE;
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
        const wxChar *err = dlerror();
        if( err )
        {
            failed = TRUE;
            wxLogError( msg, err );
        }
#else
        failed = TRUE;
        wxLogSysError( msg, libname.c_str() );
#endif
    }

    if ( success )
        *success = !failed;

    return handle;
}


/* static */
void wxDllLoader::UnloadLibrary(wxDllType handle)
{
   wxDllClose(handle);
}

/* static */
void *wxDllLoader::GetSymbol(wxDllType dllHandle, const wxString &name, bool *success)
{
    bool    failed = FALSE;
    void    *symbol = 0;

#if defined(__WXMAC__) && !defined(__UNIX__)
    Ptr                 symAddress;
    CFragSymbolClass    symClass;
    Str255              symName;

	wxMacStringToPascal( name.c_str() , symName ) ;

    if( FindSymbol( ((CFragConnectionID)dllHandle), symName, &symAddress, &symClass ) == noErr )
        symbol = (void *)symAddress;

#elif defined(__WXPM__) || defined(__EMX__)
    wxDllGetSymbol(dllHandle, symbol);

#else // Windows or Unix

    // mb_str() is necessary in Unicode build
    //
    // "void *" cast is needed by gcc 3.1 + w32api 1.4, don't ask me why
#ifdef __WXWINCE__
    symbol = (void *) wxDllGetSymbol(dllHandle, name.c_str());
#else
    symbol = (void *) wxDllGetSymbol(dllHandle, name.mb_str());
#endif

#endif // OS

    if ( !symbol )
    {
#ifdef HAVE_DLERROR
        const wxChar *err = dlerror();
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

    bool success = FALSE;
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

#endif // wxUSE_DYNLIB_CLASS && !wxUSE_DYNAMIC_LOADER

#if defined(__DARWIN__) && (wxUSE_DYNLIB_CLASS || wxUSE_DYNAMIC_LOADER)
// ---------------------------------------------------------------------------
// For Darwin/Mac OS X
//   supply the sun style dlopen functions in terms of Darwin NS*
// ---------------------------------------------------------------------------

#include <stdio.h>
#include <mach-o/dyld.h>

static char dl_last_error[1024];

static
void TranslateError(const char *path, int number)
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

    index = number;
    if (index > NUM_OFI_ERRORS - 1) {
        index = NUM_OFI_ERRORS - 1;
    }
    sprintf(dl_last_error, OFIErrorStrings[index], path, number);
}

const char *dlerror()
{
    return dl_last_error;
}

void *dlopen(const char *path, int WXUNUSED(mode) /* mode is ignored */)
{
    int dyld_result;
    NSObjectFileImage ofile;
    NSModule handle = NULL;

    dyld_result = NSCreateObjectFileImageFromFile(path, &ofile);
    if (dyld_result != NSObjectFileImageSuccess)
    {
	TranslateError(path, dyld_result);
    }
    else
    {
	// NSLinkModule will cause the run to abort on any link error's
	// not very friendly but the error recovery functionality is limited.
	handle = NSLinkModule(ofile, path, NSLINKMODULE_OPTION_BINDNOW);
    }

    return handle;
}

int dlclose(void *handle)
{
    NSUnLinkModule( handle, NSUNLINKMODULE_OPTION_NONE);
    return 0;
}

void *dlsym(void *handle, const char *symbol)
{
    void *addr;
    
    NSSymbol nsSymbol = NSLookupSymbolInModule( handle , symbol ) ;

    if ( nsSymbol) 
    {
	    addr = NSAddressOfSymbol(nsSymbol);
    }
    else 
    {
	    addr = NULL;
    }
    return addr;
}

#endif // defined(__DARWIN__) && (wxUSE_DYNLIB_CLASS || wxUSE_DYNAMIC_LOADER)
