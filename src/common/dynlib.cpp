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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#   pragma implementation "dynlib.h"
#endif

#include  "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_DYNLIB_CLASS

#if defined(__WINDOWS__)
    #include "wx/msw/wrapwin.h"
#endif

#include "wx/dynlib.h"
#include "wx/filefn.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/utils.h"
#include "wx/filename.h"        // for SplitPath()
#include "wx/app.h"
#include "wx/apptrait.h"

#if defined(__WXMAC__)
    #include "wx/mac/private.h"
#endif


// ============================================================================
// implementation
// ============================================================================

#if defined(__DARWIN__)
// ---------------------------------------------------------------------------
// For Darwin/Mac OS X
//   supply the sun style dlopen functions in terms of Darwin NS*
// ---------------------------------------------------------------------------

/* Porting notes:
 *   The dlopen port is a port from dl_next.xs by Anno Siegel.
 *   dl_next.xs is itself a port from dl_dlopen.xs by Paul Marquess.
 *   The method used here is just to supply the sun style dlopen etc.
 *   functions in terms of Darwin NS*.
 */

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
    // as on many other systems, C symbols have prepended underscores under
    // Darwin but unlike the normal dlopen(), NSLookupSymbolInModule() is not
    // aware of this
    NSSymbol nsSymbol = NSLookupSymbolInModule( handle,
                                                wxString(_T('_')) + symbol );
    return nsSymbol ? NSAddressOfSymbol(nsSymbol) : NULL;
}

#endif // defined(__DARWIN__)


// ---------------------------------------------------------------------------
// wxDynamicLibrary
// ---------------------------------------------------------------------------

//FIXME:  This class isn't really common at all, it should be moved into
//        platform dependent files.

#if defined(__WINDOWS__) || defined(__WXPM__) || defined(__EMX__)
    const wxChar *wxDynamicLibrary::ms_dllext = _T(".dll");
#elif defined(__WXMAC__) && !defined(__DARWIN__)
    const wxChar *wxDynamicLibrary::ms_dllext = _T("");
#elif defined(__UNIX__)
    #if defined(__HPUX__)
        const wxChar *wxDynamicLibrary::ms_dllext = _T(".sl");
    #elif defined(__DARWIN__)
        const wxChar *wxDynamicLibrary::ms_dllext = _T(".bundle");
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
        wxLogSysError( _("Failed to load shared library '%s' Error '%s'"),
                       libname.c_str(),
                       wxMacMakeStringFromPascal( myErrName ).c_str() );
        m_handle = 0;
    }

#elif defined(__WXPM__) || defined(__EMX__)
    char    err[256] = "";
    DosLoadModule(err, sizeof(err), libname.c_str(), &m_handle);

#elif defined(HAVE_DLOPEN) || defined(__DARWIN__)

#if defined(__VMS) || defined(__DARWIN__)
    m_handle = dlopen(libname.fn_str(), 0);  // The second parameter is ignored
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

/* static */
void wxDynamicLibrary::Unload(wxDllType handle)
{
#if defined(__WXPM__) || defined(__EMX__)
    DosFreeModule( handle );
#elif defined(HAVE_DLOPEN) || defined(__DARWIN__)
    dlclose( handle );
#elif defined(HAVE_SHL_LOAD)
    shl_unload( handle );
#elif defined(__WINDOWS__)
    ::FreeLibrary( handle );
#elif defined(__WXMAC__) && !defined(__DARWIN__)
    CloseConnection( (CFragConnectionID*) &handle );
#else
    #error  "runtime shared lib support not implemented"
#endif
}

void *wxDynamicLibrary::DoGetSymbol(const wxString &name, bool *success) const
{
    wxCHECK_MSG( IsLoaded(), NULL,
                 _T("Can't load symbol from unloaded library") );

    void    *symbol = 0;

    wxUnusedVar(symbol);
#if defined(__WXMAC__) && !defined(__DARWIN__)
    Ptr                 symAddress;
    CFragSymbolClass    symClass;
    Str255              symName;
#if TARGET_CARBON
    c2pstrcpy( (StringPtr) symName, name.fn_str() );
#else
    strcpy( (char *)symName, name.fn_str() );
    c2pstr( (char *)symName );
#endif
    if( FindSymbol( m_handle, symName, &symAddress, &symClass ) == noErr )
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
#ifdef __WXWINCE__
    symbol = (void*) ::GetProcAddress( m_handle, name );
#else
    symbol = (void*) ::GetProcAddress( m_handle, name.mb_str() );
#endif

#else
#error  "runtime shared lib support not implemented"
#endif

    if ( success )
        *success = symbol != NULL;

    return symbol;
}

void *wxDynamicLibrary::GetSymbol(const wxString& name, bool *success) const
{
    void *symbol = DoGetSymbol(name, success);
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
        wxLogSysError(_("Couldn't find symbol '%s' in a dynamic library"),
                      name.c_str());
#endif
    }

    return symbol;
}

/*static*/
wxString
wxDynamicLibrary::CanonicalizeName(const wxString& name,
                                   wxDynamicLibraryCategory
#ifdef __UNIX__
                                        cat
#else // !__UNIX__
                                        WXUNUSED(cat)
#endif // __UNIX__/!__UNIX__
                                   )
{
    wxString nameCanonic;

    // under Unix the library names usually start with "lib" prefix, add it
#ifdef __UNIX__
    switch ( cat )
    {
        default:
            wxFAIL_MSG( _T("unknown wxDynamicLibraryCategory value") );
            // fall through

        case wxDL_MODULE:
            // don't do anything for modules, their names are arbitrary
            break;

        case wxDL_LIBRARY:
            // library names should start with "lib" under Unix
            nameCanonic = _T("lib");
            break;
    }
#endif // __UNIX__

    nameCanonic << name << GetDllExt();
    return nameCanonic;
}

/*static*/
wxString wxDynamicLibrary::CanonicalizePluginName(const wxString& name,
                                                  wxPluginCategory cat)
{
    wxString suffix;
    if ( cat == wxDL_PLUGIN_GUI )
    {
        wxAppTraits *traits = wxAppConsole::GetInstance() ?
                              wxAppConsole::GetInstance()->GetTraits() : NULL;
        wxASSERT_MSG( traits,
               _("can't query for GUI plugins name in console applications") );
        suffix = traits->GetToolkitInfo().shortName;
    }
#if wxUSE_UNICODE
    suffix << _T('u');
#endif
#ifdef __WXDEBUG__
    suffix << _T('d');
#endif

    if ( !suffix.empty() )
        suffix = wxString(_T("_")) + suffix;

#define WXSTRINGIZE(x)  #x
#ifdef __UNIX__
    #if (wxMINOR_VERSION % 2) == 0
        #define wxDLLVER(x,y,z) "-" WXSTRINGIZE(x) "." WXSTRINGIZE(y)
    #else
        #define wxDLLVER(x,y,z) "-" WXSTRINGIZE(x) "." WXSTRINGIZE(y) "." WXSTRINGIZE(z)
    #endif
#else
    #if (wxMINOR_VERSION % 2) == 0
        #define wxDLLVER(x,y,z) WXSTRINGIZE(x) WXSTRINGIZE(y)
    #else
        #define wxDLLVER(x,y,z) WXSTRINGIZE(x) WXSTRINGIZE(y) WXSTRINGIZE(z)
    #endif
#endif

    suffix << wxString::FromAscii(wxDLLVER(wxMAJOR_VERSION, wxMINOR_VERSION,
                                           wxRELEASE_NUMBER));
#undef wxDLLVER
#undef WXSTRINGIZE

#ifdef __WINDOWS__
    // Add compiler identification:
    #if defined(__GNUG__)
        suffix << _T("_gcc");
    #elif defined(__VISUALC__)
        suffix << _T("_vc");
    #elif defined(__WATCOMC__)
        suffix << _T("_wat");
    #elif defined(__BORLANDC__)
        suffix << _T("_bcc");
    #endif
#endif

    return CanonicalizeName(name + suffix, wxDL_MODULE);
}

/*static*/
wxString wxDynamicLibrary::GetPluginsDirectory()
{
#ifdef __UNIX__
    wxString format = wxGetInstallPrefix();
    wxString dir;
    format << wxFILE_SEP_PATH
           << wxT("lib") << wxFILE_SEP_PATH
           << wxT("wx") << wxFILE_SEP_PATH
#if (wxMINOR_VERSION % 2) == 0
           << wxT("%i.%i");
    dir.Printf(format.c_str(), wxMAJOR_VERSION, wxMINOR_VERSION);
#else
           << wxT("%i.%i.%i");
    dir.Printf(format.c_str(),
               wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER);
#endif
    return dir;

#else // ! __UNIX__
    return wxEmptyString;
#endif
}


#endif // wxUSE_DYNLIB_CLASS
