/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dlmsw.cpp
// Purpose:     Win32-specific part of wxDynamicLibrary and related classes
// Author:      Vadim Zeitlin
// Modified by: Suzumizaki-kimitaka 2015-06-14
// Created:     2005-01-10 (partly extracted from common/dynlib.cpp)
// Copyright:   (c) 1998-2005 Vadim Zeitlin <vadim@wxwindows.org>
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

#include "wx/msw/private.h"
#include "wx/msw/debughlp.h"
#include "wx/filename.h"

// defined for TDM's GCC/mingw32
#ifndef PCTSTR
#define PCTSTR LPCTSTR
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// wrap some functions from version.dll: load them dynamically and provide a
// clean interface
class wxVersionDLL
{
public:
    // load version.dll and bind to its functions
    wxVersionDLL();

    // return the file version as string, e.g. "x.y.z.w"
    wxString GetFileVersion(const wxString& filename) const;

private:
    typedef DWORD (APIENTRY *GetFileVersionInfoSize_t)(PTSTR, PDWORD);
    typedef BOOL (APIENTRY *GetFileVersionInfo_t)(PTSTR, DWORD, DWORD, PVOID);
    typedef BOOL (APIENTRY *VerQueryValue_t)(const PVOID, PTSTR, PVOID *, PUINT);

    #define DO_FOR_ALL_VER_FUNCS(what)                                        \
        what(GetFileVersionInfoSize);                                         \
        what(GetFileVersionInfo);                                             \
        what(VerQueryValue)

    #define DECLARE_VER_FUNCTION(func) func ## _t m_pfn ## func

    DO_FOR_ALL_VER_FUNCS(DECLARE_VER_FUNCTION);

    #undef DECLARE_VER_FUNCTION


    wxDynamicLibrary m_dll;


    wxDECLARE_NO_COPY_CLASS(wxVersionDLL);
};

#if API_VERSION_NUMBER < 11
typedef PTSTR ModName_t;
#else
typedef PCTSTR ModName_t;
#endif

// class used to create wxDynamicLibraryDetails objects
class WXDLLIMPEXP_BASE wxDynamicLibraryDetailsCreator
{
public:
    // type of parameters being passed to EnumModulesProc
    struct EnumModulesProcParams
    {
        wxDynamicLibraryDetailsArray *dlls;
        wxVersionDLL *verDLL;
    };

    static BOOL CALLBACK
    EnumModulesProc(ModName_t name, DWORD64 base, ULONG size, PVOID data);
};

// ============================================================================
// wxVersionDLL implementation
// ============================================================================

// ----------------------------------------------------------------------------
// loading
// ----------------------------------------------------------------------------

wxVersionDLL::wxVersionDLL()
{
    // don't give errors if DLL can't be loaded or used, we're prepared to
    // handle it
    wxLogNull noLog;

    if ( m_dll.Load(wxT("version.dll"), wxDL_VERBATIM) )
    {
        // the functions we load have either 'A' or 'W' suffix depending on
        // whether we're in ANSI or Unicode build
        #ifdef UNICODE
            #define SUFFIX L"W"
        #else // ANSI
            #define SUFFIX "A"
        #endif // UNICODE/ANSI

        #define LOAD_VER_FUNCTION(name)                                       \
            m_pfn ## name = (name ## _t)m_dll.GetSymbol(wxT(#name SUFFIX));   \
        if ( !m_pfn ## name )                                                 \
        {                                                                     \
            m_dll.Unload();                                                   \
            return;                                                           \
        }

        DO_FOR_ALL_VER_FUNCS(LOAD_VER_FUNCTION);

        #undef LOAD_VER_FUNCTION
    }
}

// ----------------------------------------------------------------------------
// wxVersionDLL operations
// ----------------------------------------------------------------------------

wxString wxVersionDLL::GetFileVersion(const wxString& filename) const
{
    wxString ver;
    if ( m_dll.IsLoaded() )
    {
        wxChar *pc = const_cast<wxChar *>((const wxChar*) filename.t_str());

        DWORD dummy;
        DWORD sizeVerInfo = m_pfnGetFileVersionInfoSize(pc, &dummy);
        if ( sizeVerInfo )
        {
            wxCharBuffer buf(sizeVerInfo);
            if ( m_pfnGetFileVersionInfo(pc, 0, sizeVerInfo, buf.data()) )
            {
                void *pVer;
                UINT sizeInfo;
                if ( m_pfnVerQueryValue(buf.data(),
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
    }
    //else: we failed to load DLL, can't retrieve version info

    return ver;
}

// ============================================================================
// wxDynamicLibraryDetailsCreator implementation
// ============================================================================

/* static */
BOOL CALLBACK
wxDynamicLibraryDetailsCreator::EnumModulesProc(ModName_t name,
                                                DWORD64 base,
                                                ULONG size,
                                                PVOID data)
{
    EnumModulesProcParams *params = (EnumModulesProcParams *)data;

    wxDynamicLibraryDetails *details = new wxDynamicLibraryDetails;

    // fill in simple properties
#ifdef UNICODE
    details->m_name = name;
#else
    details->m_name = wxString(name, wxConvLocal);
#endif
    details->m_address = wxUIntToPtr(base);
    details->m_length = size;

    // to get the version, we first need the full path
    const HMODULE hmod = wxDynamicLibrary::MSWGetModuleHandle
                         (
                            details->m_name,
                            details->m_address
                         );
    if ( hmod )
    {
        wxString fullname = wxGetFullModuleName(hmod);
        if ( !fullname.empty() )
        {
            details->m_path = fullname;
            details->m_version = params->verDLL->GetFileVersion(fullname);
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
    return (wxDllType)::GetModuleHandle(NULL);
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

    // Explicitly look in the same path as where the main wx HINSTANCE module
    // is located (usually the executable or the DLL that uses wx).  Normally
    // this is automatically part of the default search path but in some cases
    // it may not be, such as when the wxPython extension modules need to load
    // a DLL, but the intperpreter executable is located elsewhere.  Doing
    // this allows us to always be able to dynamically load a DLL that is
    // located at the same place as the wx modules.
    wxString modpath, path;
    ::GetModuleFileName(wxGetInstance(),
                        wxStringBuffer(modpath, MAX_PATH+1),
                        MAX_PATH);
    
    wxFileName::SplitPath(modpath, &path, NULL, NULL);

    typedef BOOL (WINAPI *SetDllDirectory_t)(LPCTSTR lpPathName);

    static SetDllDirectory_t s_pfnSetDllDirectory = (SetDllDirectory_t) -1;

    if ( s_pfnSetDllDirectory == (SetDllDirectory_t) -1 )
    {
        /*
        Should wxLoadedDLL ever not be used here (or rather, the
        wxDL_GET_LOADED flag isn't used), infinite recursion will take
        place (unless s_pfnSetDllDirectory is set to NULL here right
        before loading the DLL).
        */
        wxLoadedDLL dllKernel("kernel32.dll");

        wxDL_INIT_FUNC_AW(s_pfn, SetDllDirectory, dllKernel);
    }

    if (s_pfnSetDllDirectory)
    {
        s_pfnSetDllDirectory(path.t_str());
    }

    wxDllType handle = ::LoadLibrary(libname.t_str());

    // reset the search path
    if (s_pfnSetDllDirectory)
    {
        s_pfnSetDllDirectory(NULL);
    }

    return handle;
}

/* static */
void wxDynamicLibrary::Unload(wxDllType handle)
{
    ::FreeLibrary(handle);
}

/* static */
void *wxDynamicLibrary::RawGetSymbol(wxDllType handle, const wxString& name)
{
    return (void *)::GetProcAddress(handle,
#ifdef __WXWINCE__
                                            name.c_str()
#else
                                            name.ToAscii()
#endif // __WXWINCE__
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
        // prepare to use functions for version info extraction
        wxVersionDLL verDLL;

        wxDynamicLibraryDetailsCreator::EnumModulesProcParams params;
        params.dlls = &dlls;
        params.verDLL = &verDLL;

        if ( !wxDbgHelpDLL::EnumerateLoadedModulesT
                            (
                                ::GetCurrentProcess(),
                                wxDynamicLibraryDetailsCreator::EnumModulesProc,
                                &params
                            ) )
        {
            wxLogLastError(wxT("EnumerateLoadedModulesT"));
        }
    }
#endif // wxUSE_DBGHELP

    return dlls;
}

/* static */
WXHMODULE wxDynamicLibrary::MSWGetModuleHandle(const wxString& name, void *addr)
{
    // we want to use GetModuleHandleEx() instead of usual GetModuleHandle()
    // because the former works correctly for comctl32.dll while the latter
    // returns NULL when comctl32.dll version 6 is used under XP (note that
    // GetModuleHandleEx() is only available under XP and later, coincidence?)

    // check if we can use GetModuleHandleEx
    typedef BOOL (WINAPI *GetModuleHandleEx_t)(DWORD, LPCTSTR, HMODULE *);

    static const GetModuleHandleEx_t INVALID_FUNC_PTR = (GetModuleHandleEx_t)-1;

    static GetModuleHandleEx_t s_pfnGetModuleHandleEx = INVALID_FUNC_PTR;
    if ( s_pfnGetModuleHandleEx == INVALID_FUNC_PTR )
    {
        wxDynamicLibrary dll(wxT("kernel32.dll"), wxDL_VERBATIM);
        s_pfnGetModuleHandleEx =
            (GetModuleHandleEx_t)dll.GetSymbolAorW(wxT("GetModuleHandleEx"));

        // dll object can be destroyed, kernel32.dll won't be unloaded anyhow
    }

    // get module handle from its address
    if ( s_pfnGetModuleHandleEx )
    {
        // flags are GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT |
        //           GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
        HMODULE hmod;
        if ( s_pfnGetModuleHandleEx(6, (LPCTSTR)addr, &hmod) && hmod )
            return hmod;
    }

    return ::GetModuleHandle(name.t_str());
}

#endif // wxUSE_DYNLIB_CLASS

