/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/dlunix.cpp
// Purpose:     Unix-specific part of wxDynamicLibrary and related classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-16 (extracted from common/dynlib.cpp)
// Copyright:   (c) 2000-2005 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include  "wx/wxprec.h"


#if wxUSE_DYNLIB_CLASS

#include "wx/dynlib.h"
#include "wx/ffile.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#ifdef HAVE_DLOPEN
    #include <dlfcn.h>
#endif

#ifdef HAVE_DL_ITERATE_PHDR
    #include <link.h>
#endif

// if some flags are not supported, just ignore them
#ifndef RTLD_LAZY
    #define RTLD_LAZY 0
#endif

#ifndef RTLD_NOW
    #define RTLD_NOW 0
#endif

#ifndef RTLD_GLOBAL
    #define RTLD_GLOBAL 0
#endif


#ifndef HAVE_DLOPEN
    #error "Don't know how to load dynamic libraries on this platform!"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// loading/unloading DLLs
// ----------------------------------------------------------------------------

wxDllType wxDynamicLibrary::GetProgramHandle()
{
   return dlopen(nullptr, RTLD_LAZY);
}

/* static */
wxDllType wxDynamicLibrary::RawLoad(const wxString& libname, int flags)
{
    wxASSERT_MSG( !(flags & wxDL_NOW) || !(flags & wxDL_LAZY),
                  wxT("wxDL_LAZY and wxDL_NOW are mutually exclusive.") );

    // we need to use either RTLD_NOW or RTLD_LAZY because if we call dlopen()
    // with flags == 0 recent versions of glibc just fail the call, so use
    // RTLD_NOW even if wxDL_NOW was not specified
    int rtldFlags = flags & wxDL_LAZY ? RTLD_LAZY : RTLD_NOW;

    if ( flags & wxDL_GLOBAL )
        rtldFlags |= RTLD_GLOBAL;

    return dlopen(libname.fn_str(), rtldFlags);
}

/* static */
void wxDynamicLibrary::Unload(wxDllType handle)
{
    int rc = dlclose(handle);

    if ( rc != 0 )
        ReportError(_("Failed to unload shared library"));
}

/* static */
void *wxDynamicLibrary::RawGetSymbol(wxDllType handle, const wxString& name)
{
    void *symbol = dlsym(handle, name.fn_str());

    return symbol;
}

// ----------------------------------------------------------------------------
// error handling
// ----------------------------------------------------------------------------

/* static */
void wxDynamicLibrary::ReportError(const wxString& message,
                                   const wxString& name)
{
    wxString msg(message);
    if ( name.IsEmpty() && msg.Find("%s") == wxNOT_FOUND )
        msg += "%s";
    // msg needs a %s for the name
    wxASSERT(msg.Find("%s") != wxNOT_FOUND);

    wxString err(dlerror());

    if ( err.empty() )
        err = _("Unknown dynamic library error");

    wxLogError(msg + wxT(": %s"), name, err);
}


// ----------------------------------------------------------------------------
// listing loaded modules
// ----------------------------------------------------------------------------

#ifdef HAVE_DL_ITERATE_PHDR

// wxDynamicLibraryDetails declares this class as its friend, so put the code
// initializing new details objects here
class wxDynamicLibraryDetailsCreator
{
public:
    static int Callback(dl_phdr_info* info, size_t /* size */, void* data)
    {
        const wxString path = wxString::FromUTF8(info->dlpi_name);

        wxDynamicLibraryDetails details;
        details.m_path = path;
        details.m_name = path.AfterLast(wxT('/'));

        // Find the first and last address belonging to this module.
        decltype(info->dlpi_addr) start = 0, end = 0;
        for ( decltype(info->dlpi_phnum) n = 0; n < info->dlpi_phnum; n++ )
        {
            const auto& segment = info->dlpi_phdr[n];
            if ( !segment.p_vaddr || !segment.p_memsz )
                continue;

            if ( !start || segment.p_vaddr < start )
                start = segment.p_vaddr;

            if ( !end || segment.p_vaddr + segment.p_memsz > end )
                end = segment.p_vaddr + segment.p_memsz;
        }

        details.m_address = wxUIntToPtr(info->dlpi_addr + start);
        details.m_length = end - start;

        // try to extract the library version from its name
        const size_t posExt = path.rfind(wxT(".so"));
        if ( posExt != wxString::npos )
        {
            if ( path.c_str()[posExt + 3] == wxT('.') )
            {
                // assume "libfoo.so.x.y.z" case
                details.m_version.assign(path, posExt + 4, wxString::npos);
            }
            else
            {
                size_t posDash = path.find_last_of(wxT('-'), posExt);
                if ( posDash != wxString::npos )
                {
                    // assume "libbar-x.y.z.so" case
                    posDash++;
                    details.m_version.assign(path, posDash, posExt - posDash);
                }
            }
        }

        auto dlls = static_cast<wxDynamicLibraryDetailsArray*>(data);
        dlls->push_back(std::move(details));

        // Return 0 to keep iterating.
        return 0;
    }
};

#endif // HAVE_DL_ITERATE_PHDR

/* static */
wxDynamicLibraryDetailsArray wxDynamicLibrary::ListLoaded()
{
    wxDynamicLibraryDetailsArray dlls;

#ifdef HAVE_DL_ITERATE_PHDR
    dl_iterate_phdr(wxDynamicLibraryDetailsCreator::Callback, &dlls);
#endif // HAVE_DL_ITERATE_PHDR

    return dlls;
}


/* static */
void* wxDynamicLibrary::GetModuleFromAddress(const void* addr, wxString* path)
{
#ifdef HAVE_DLADDR
    Dl_info di = { }; // 0 initialize whatever fields the struct has

    // At least under Solaris dladdr() takes non-const void*.
    if ( dladdr(const_cast<void*>(addr), &di) == 0 )
        return nullptr;

    if ( path )
        *path = di.dli_fname;

    return di.dli_fbase;
#else
    wxUnusedVar(addr);
    wxUnusedVar(path);
#endif // HAVE_DLADDR

    return nullptr;
}


#endif // wxUSE_DYNLIB_CLASS

