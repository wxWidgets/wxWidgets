/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dlmsw.cpp
// Purpose:     Win32-specific part of wxDynamicLibrary and related classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-10 (partly extracted from common/dynlib.cpp)
// Copyright:   (c) 1998-2005 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include  "wx/wxprec.h"

#include "wx/dynlib.h"

#include "wx/msw/private.h"
#include "wx/msw/debughlp.h"
#include "wx/filename.h"

// For MSVC we can link in the required library explicitly, for the other
// compilers (e.g. MinGW) this needs to be done at makefiles level.
#ifdef __VISUALC__
    #pragma comment(lib, "version")
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// class used to create wxDynamicLibraryDetails objects
class WXDLLIMPEXP_BASE wxDynamicLibraryDetailsCreator
{
public:
    // type of parameters being passed to EnumModulesProc
    struct EnumModulesProcParams
    {
        wxDynamicLibraryDetailsArray *dlls;
    };

    static BOOL CALLBACK
    EnumModulesProc(const wxChar* name, DWORD64 base, ULONG size, PVOID data);
};

// ----------------------------------------------------------------------------
// DLL version operations
// ----------------------------------------------------------------------------

static wxString GetFileVersion(const wxString& filename)
{
    wxString ver;
    wxChar *pc = const_cast<wxChar *>((const wxChar*) filename.t_str());

    DWORD dummy;
    DWORD sizeVerInfo = ::GetFileVersionInfoSize(pc, &dummy);
    if ( sizeVerInfo )
    {
        wxCharBuffer buf(sizeVerInfo);
        if ( ::GetFileVersionInfo(pc, 0, sizeVerInfo, buf.data()) )
        {
            void *pVer;
            UINT sizeInfo;
            if ( ::VerQueryValue(buf.data(),
                                    const_cast<wxChar *>(wxT("\\")),
                                    &pVer,
                                    &sizeInfo) )
            {
                VS_FIXEDFILEINFO *info = (VS_FIXEDFILEINFO *)pVer;
                ver.Printf(wxT("%d.%d.%d.%d"),
                            HIWORD(info->dwFileVersionMS),
                            LOWORD(info->dwFileVersionMS),
                            HIWORD(info->dwFileVersionLS),
                            LOWORD(info->dwFileVersionLS));
            }
        }
    }

    return ver;
}

// ============================================================================
// wxDynamicLibraryDetailsCreator implementation
// ============================================================================

/* static */
BOOL CALLBACK
wxDynamicLibraryDetailsCreator::EnumModulesProc(const wxChar* name,
                                                DWORD64 base,
                                                ULONG size,
                                                void *data)
{
    EnumModulesProcParams *params = (EnumModulesProcParams *)data;

    wxDynamicLibraryDetails details;

    // fill in simple properties
    details.m_name = name;
    details.m_address = wxUIntToPtr(base);
    details.m_length = size;

    // to get the version, we first need the full path
    const HMODULE hmod = wxDynamicLibrary::MSWGetModuleHandle
                         (
                            details.m_name,
                            details.m_address
                         );
    if ( hmod )
    {
        wxString fullname = wxGetFullModuleName(hmod);
        if ( !fullname.empty() )
        {
            details.m_path = fullname;
            details.m_version = GetFileVersion(fullname);
        }
    }

    params->dlls->Add(details);

    // continue enumeration (returning FALSE would have stopped it)
    return TRUE;
}

// ============================================================================
// wxDynamicLibrary implementation
// ============================================================================

// ----------------------------------------------------------------------------
// misc functions
// ----------------------------------------------------------------------------

wxDllType wxDynamicLibrary::GetProgramHandle()
{
    return (wxDllType)::GetModuleHandle(nullptr);
}

// ----------------------------------------------------------------------------
// error handling
// ----------------------------------------------------------------------------

/* static */
void wxDynamicLibrary::ReportError(const wxString& message, const wxString& name)
{
    wxString msg(message);
    if ( name.IsEmpty() && msg.Find("%s") == wxNOT_FOUND )
        msg += "%s";
    // msg needs a %s for the name
    wxASSERT(msg.Find("%s") != wxNOT_FOUND);

    const unsigned long code = wxSysErrorCode();
    wxString errMsg = wxSysErrorMsgStr(code);

    // The error message (specifically code==193) may contain a
    // placeholder '%1' which stands for the filename.
    errMsg.Replace("%1", name, false);

    // Mimic the output of wxLogSysError(), but use our pre-processed
    // errMsg.
    wxLogError(msg + " " + _("(error %d: %s)"), name, code, errMsg);
}

// ----------------------------------------------------------------------------
// loading/unloading DLLs
// ----------------------------------------------------------------------------

#ifndef MAX_PATH
    #define MAX_PATH 260        // from VC++ headers
#endif

/* static */
wxDllType
wxDynamicLibrary::RawLoad(const wxString& libname, int flags)
{
    if (flags & wxDL_GET_LOADED)
        return ::GetModuleHandle(libname.t_str());

    return ::LoadLibrary(libname.t_str());
}

/* static */
void wxDynamicLibrary::Unload(wxDllType handle)
{
    if ( !::FreeLibrary(handle) )
    {
        wxLogLastError(wxT("FreeLibrary"));
    }
}

/* static */
void *wxDynamicLibrary::RawGetSymbol(wxDllType handle, const wxString& name)
{
    return (void *)::GetProcAddress(handle,
                                            name.ToAscii()
                                   );
}

// ----------------------------------------------------------------------------
// enumerating loaded DLLs
// ----------------------------------------------------------------------------

/* static */
wxDynamicLibraryDetailsArray wxDynamicLibrary::ListLoaded()
{
    wxDynamicLibraryDetailsArray dlls;

#if wxUSE_DBGHELP
    if ( wxDbgHelpDLL::Init() )
    {
        wxDynamicLibraryDetailsCreator::EnumModulesProcParams params;
        params.dlls = &dlls;

        if ( !wxDbgHelpDLL::CallEnumerateLoadedModules
                            (
                                ::GetCurrentProcess(),
                                wxDynamicLibraryDetailsCreator::EnumModulesProc,
                                &params
                            ) )
        {
            wxLogLastError(wxT("EnumerateLoadedModules"));
        }
    }
#endif // wxUSE_DBGHELP

    return dlls;
}

// ----------------------------------------------------------------------------
// Getting the module from an address inside it
// ----------------------------------------------------------------------------

/* static */
void* wxDynamicLibrary::GetModuleFromAddress(const void* addr, wxString* path)
{
    HMODULE hmod;
    if ( !::GetModuleHandleEx(0, (LPCTSTR)addr, &hmod) || !hmod )
    {
        wxLogLastError(wxT("GetModuleHandleEx"));
        return nullptr;
    }

    if ( path )
    {
        TCHAR libname[MAX_PATH];
        if ( !::GetModuleFileName(hmod, libname, MAX_PATH) )
        {
            // GetModuleFileName could also return extended-length paths (paths
            // prepended with "//?/", maximum length is 32767 charachters) so,
            // in principle, MAX_PATH could be unsufficient and we should try
            // increasing the buffer size here.
            wxLogLastError(wxT("GetModuleFromAddress"));
            return nullptr;
        }

        libname[MAX_PATH-1] = wxT('\0');

        *path = libname;
    }

    // In Windows HMODULE is actually the base address of the module so we
    // can just cast it to the address.
    return reinterpret_cast<void *>(hmod);
}

/* static */
WXHMODULE wxDynamicLibrary::MSWGetModuleHandle(const wxString& name, void *addr)
{
    // we want to use GetModuleHandleEx() instead of usual GetModuleHandle()
    // because the former works correctly for comctl32.dll while the latter
    // returns nullptr when comctl32.dll version 6 is used under XP (note that
    // GetModuleHandleEx() is only available under XP and later, coincidence?)
    HMODULE hmod;
    if ( !addr || !::GetModuleHandleEx(0, (LPCTSTR)addr, &hmod) || !hmod )
        hmod = ::GetModuleHandle(name.t_str());

    return hmod;
}
