/////////////////////////////////////////////////////////////////////////////
// Name:        unix/dlunix.cpp
// Purpose:     Unix-specific part of wxDynamicLibrary and related classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-16 (extracted from common/dynlib.cpp)
// RCS-ID:      $Id$
// Copyright:   (c) 2000-2005 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include  "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_DYNLIB_CLASS

#include "wx/dynlib.h"
#include "wx/ffile.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#if defined(__DARWIN__)
    #include <dlfcn.h>
#endif

#if defined(HAVE_DLOPEN) || defined(__DARWIN__)
    #define USE_POSIX_DL_FUNCS
#elif !defined(HAVE_SHL_LOAD)
    #error "Don't know how to load dynamic libraries on this platform!"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// standard shared libraries extensions for different Unix versions
#if defined(__HPUX__)
    const wxChar *wxDynamicLibrary::ms_dllext = _T(".sl");
#elif defined(__DARWIN__)
    const wxChar *wxDynamicLibrary::ms_dllext = _T(".bundle");
#else
    const wxChar *wxDynamicLibrary::ms_dllext = _T(".so");
#endif

// ============================================================================
// wxDynamicLibrary implementation
// ============================================================================

// ----------------------------------------------------------------------------
// dlxxx() emulation for Darwin
// ----------------------------------------------------------------------------

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
    NSObjectFileImage ofile;
    NSModule handle = NULL;

    int dyld_result = NSCreateObjectFileImageFromFile(path, &ofile);
    if ( dyld_result != NSObjectFileImageSuccess )
    {
        handle = NULL;
    }
    else
    {
        handle = NSLinkModule
                 (
                    ofile,
                    path,
                    NSLINKMODULE_OPTION_BINDNOW |
                    NSLINKMODULE_OPTION_RETURN_ON_ERROR
                 );
    }

    if ( !handle )
        TranslateError(path, dyld_result);

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
    wxCharBuffer buf(strlen(symbol) + 1);
    char *p = buf.data();
    p[0] = '_';
    strcpy(p + 1, symbol);

    NSSymbol nsSymbol = NSLookupSymbolInModule( handle, p );
    return nsSymbol ? NSAddressOfSymbol(nsSymbol) : NULL;
}

#endif // defined(__DARWIN__)

// ----------------------------------------------------------------------------
// loading/unloading DLLs
// ----------------------------------------------------------------------------

wxDllType wxDynamicLibrary::GetProgramHandle()
{
#ifdef USE_POSIX_DL_FUNCS
   return dlopen(0, RTLD_LAZY);
#else
   return PROG_HANDLE;
#endif
}

/* static */
wxDllType wxDynamicLibrary::RawLoad(const wxString& libname, int flags)
{
    wxASSERT_MSG( (flags & wxDL_NOW) == 0,
                  _T("wxDL_LAZY and wxDL_NOW are mutually exclusive.") );

#ifdef USE_POSIX_DL_FUNCS
    int rtldFlags = 0;

#ifdef RTLD_LAZY
    if ( flags & wxDL_LAZY )
    {
        rtldFlags |= RTLD_LAZY;
    }
#endif
#ifdef RTLD_NOW
    if ( flags & wxDL_NOW )
    {
        rtldFlags |= RTLD_NOW;
    }
#endif
#ifdef RTLD_GLOBAL
    if ( flags & wxDL_GLOBAL )
    {
        rtldFlags |= RTLD_GLOBAL;
    }
#endif

    return dlopen(libname.fn_str(), rtldFlags);
#else // !USE_POSIX_DL_FUNCS
    int shlFlags = 0;

    if ( flags & wxDL_LAZY )
    {
        shlFlags |= BIND_DEFERRED;
    }
    else if ( flags & wxDL_NOW )
    {
        shlFlags |= BIND_IMMEDIATE;
    }

    return shl_load(libname.fn_str(), shlFlags, 0);
#endif // USE_POSIX_DL_FUNCS/!USE_POSIX_DL_FUNCS
}

/* static */
void wxDynamicLibrary::Unload(wxDllType handle)
{
#ifdef wxHAVE_DYNLIB_ERROR
    int rc =
#endif

#ifdef USE_POSIX_DL_FUNCS
    dlclose(handle);
#else // !USE_POSIX_DL_FUNCS
    shl_unload(handle);
#endif // USE_POSIX_DL_FUNCS/!USE_POSIX_DL_FUNCS

#if defined(USE_POSIX_DL_FUNCS) && defined(wxHAVE_DYNLIB_ERROR)
    if ( rc != 0 )
        Error();
#endif
}

/* static */
void *wxDynamicLibrary::RawGetSymbol(wxDllType handle, const wxString& name)
{
    void *symbol;

#ifdef USE_POSIX_DL_FUNCS
    symbol = dlsym(handle, name.fn_str());
#else // !USE_POSIX_DL_FUNCS
    // note that shl_findsym modifies the handle argument to indicate where the
    // symbol was found, but it's ok to modify the local handle copy here
    if ( shl_findsym(&handle, name.fn_str(), TYPE_UNDEFINED, &symbol) != 0 )
        symbol = 0;
#endif // USE_POSIX_DL_FUNCS/!USE_POSIX_DL_FUNCS

    return symbol;
}

// ----------------------------------------------------------------------------
// error handling
// ----------------------------------------------------------------------------

#ifdef wxHAVE_DYNLIB_ERROR

/* static */
void wxDynamicLibrary::Error()
{
#if wxUSE_UNICODE
    wxWCharBuffer buffer = wxConvLocal.cMB2WC( dlerror() );
    const wxChar *err = buffer;
#else
    const wxChar *err = dlerror();
#endif

    wxLogError(wxT("%s"), err ? err : _("Unknown dynamic library error"));
}

#endif // wxHAVE_DYNLIB_ERROR

// ----------------------------------------------------------------------------
// listing loaded modules
// ----------------------------------------------------------------------------

// wxDynamicLibraryDetails declares this class as its friend, so put the code
// initializing new details objects here
class wxDynamicLibraryDetailsCreator
{
public:
    // create a new wxDynamicLibraryDetails from the given data
    static wxDynamicLibraryDetails *
    New(unsigned long start, unsigned long end, const wxString& path)
    {
        wxDynamicLibraryDetails *details = new wxDynamicLibraryDetails;
        details->m_path = path;
        details->m_name = path.AfterLast(_T('/'));
        details->m_address = wx_reinterpret_cast(void *, start);
        details->m_length = end - start;

        // try to extract the library version from its name
        const size_t posExt = path.rfind(_T(".so"));
        if ( posExt != wxString::npos )
        {
            if ( path.c_str()[posExt + 3] == _T('.') )
            {
                // assume "libfoo.so.x.y.z" case
                details->m_version.assign(path, posExt + 4, wxString::npos);
            }
            else
            {
                size_t posDash = path.find_last_of(_T('-'), posExt);
                if ( posDash != wxString::npos )
                {
                    // assume "libbar-x.y.z.so" case
                    posDash++;
                    details->m_version.assign(path, posDash, posExt - posDash);
                }
            }
        }

        return details;
    }
};

/* static */
wxDynamicLibraryDetailsArray wxDynamicLibrary::ListLoaded()
{
    wxDynamicLibraryDetailsArray dlls;

#ifdef __LINUX__
    // examine /proc/self/maps to find out what is loaded in our address space
    wxFFile file("/proc/self/maps");
    if ( file.IsOpened() )
    {
        // details of the module currently being parsed
        wxString pathCur;
        unsigned long startCur,
                      endCur;

        char path[1024];
        char buf[1024];
        while ( fgets(buf, WXSIZEOF(buf), file.fp()) )
        {
            // format is: start-end perm something? maj:min inode path
            unsigned long start, end;
            switch ( sscanf(buf, "%08lx-%08lx %*4s %*08x %*02d:%*02d %*d %1024s\n",
                            &start, &end, path) )
            {
                case 2:
                    // there may be no path column
                    path[0] = '\0';
                    break;

                case 3:
                    // nothing to do, read everything we wanted
                    break;

                default:
                    // chop '\n'
                    buf[strlen(buf) - 1] = '\0';
                    wxLogDebug(_T("Failed to parse line \"%s\" in /proc/self/maps."),
                               buf);
                    continue;
            }

            wxString pathNew = wxString::FromAscii(path);
            if ( pathCur.empty() )
            {
                // new module start
                pathCur = pathNew;
                startCur = start;
                endCur = end;
            }
            else if ( pathCur == pathNew )
            {
                // continuation of the same module
                wxASSERT_MSG( start == endCur, _T("hole in /proc/self/maps?") );
                endCur = end;
            }
            else // end of the current module
            {
                dlls.Add(wxDynamicLibraryDetailsCreator::New(startCur,
                                                             endCur,
                                                             pathCur));
                pathCur.clear();
            }
        }
    }
#endif // __LINUX__

    return dlls;
}

#endif // wxUSE_DYNLIB_CLASS

