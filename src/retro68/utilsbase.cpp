///////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/utilsbase.cpp
// Purpose:     Non-GUI utility functions for classic Mac OS (Retro68)
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/app.h"
#endif

#include "wx/apptrait.h"
#include "wx/filename.h"
#include "wx/platinfo.h"

#include "wx/retro68/private.h"

#include <stdlib.h>
#include <string.h>

// ============================================================================
// sleeping
// ============================================================================

void wxMilliSleep(unsigned long milliseconds)
{
    // Delay() uses ticks, i.e. 1/60th of a second.
    LONGINT finalTicks;
    Delay(static_cast<LONGINT>((milliseconds * 60 + 999) / 1000), &finalTicks);
}

void wxMicroSleep(unsigned long microseconds)
{
    wxMilliSleep((microseconds + 999) / 1000);
}

void wxSleep(int nSecs)
{
    wxMilliSleep(nSecs * 1000UL);
}

void wxSecureZeroMemory(void* v, size_t n)
{
    volatile char* p = static_cast<volatile char*>(v);
    while ( n-- )
        *p++ = 0;
}

// ============================================================================
// process management: mostly not supported
// ============================================================================

int wxKill(long WXUNUSED(pid), wxSignal WXUNUSED(sig), wxKillError *rc,
           int WXUNUSED(flags))
{
    if ( rc )
        *rc = wxKILL_ERROR;

    return -1;
}

bool wxShutdown(int flags)
{
    if ( flags & wxSHUTDOWN_REBOOT )
        ShutDwnStart();
    else
        ShutDwnPower();

    // We're not supposed to get here.
    return false;
}

bool wxShell(const wxString& WXUNUSED(command))
{
    return false;
}

bool wxShell(const wxString& WXUNUSED(command), wxArrayString& WXUNUSED(output))
{
    return false;
}

namespace
{

// Launch the application with the given path, there is no support for passing
// it any arguments.
long LaunchApp(const wxString& path, int flags)
{
    FSSpec spec;
    if ( wxMacPathToFSSpec(path, &spec) != noErr )
        return 0;

    LaunchParamBlockRec params;
    memset(&params, 0, sizeof(params));
    params.launchBlockID = extendedBlock;
    params.launchEPBLength = extendedBlockLen;
    // Some of the flags are not defined in Multiverse.h, so use their values
    // directly.
    const unsigned short wxMAC_LAUNCH_NO_FILE_FLAGS = 0x0800;
    const unsigned short wxMAC_LAUNCH_DONT_SWITCH = 0x0200;

    params.launchControlFlags = launchContinue | wxMAC_LAUNCH_NO_FILE_FLAGS;
    if ( !(flags & wxEXEC_SHOW_CONSOLE) && (flags & wxEXEC_HIDE_CONSOLE) )
        params.launchControlFlags |= wxMAC_LAUNCH_DONT_SWITCH;
    params.launchAppSpec = &spec;

    if ( LaunchApplication(&params) != noErr )
        return 0;

    // There is no way to wait for the process to terminate, so return
    // something non-zero to indicate success in both sync and async cases.
    return params.launchProcessSN.lowLongOfPSN
                ? static_cast<long>(params.launchProcessSN.lowLongOfPSN)
                : 1;
}

} // anonymous namespace

long wxExecute(const wxString& command, int flags, wxProcess* WXUNUSED(process),
               const wxExecuteEnv* WXUNUSED(env))
{
    // We don't have any shell to parse the command, so just take it as the
    // full path to the application to launch.
    return LaunchApp(command, flags);
}

long wxExecute(const char* const* argv, int flags, wxProcess* WXUNUSED(process),
               const wxExecuteEnv* WXUNUSED(env))
{
    wxCHECK_MSG( argv && argv[0], 0, "can't execute empty command" );

    return LaunchApp(wxString(argv[0], wxConvFile), flags);
}

long wxExecute(const wchar_t* const* argv, int flags, wxProcess* WXUNUSED(process),
               const wxExecuteEnv* WXUNUSED(env))
{
    wxCHECK_MSG( argv && argv[0], 0, "can't execute empty command" );

    return LaunchApp(wxString(argv[0]), flags);
}

unsigned long wxGetProcessId()
{
    ProcessSerialNumber psn;
    if ( GetCurrentProcess(&psn) != noErr )
        return 0;

    return psn.lowLongOfPSN;
}

// ============================================================================
// user and host information
// ============================================================================

namespace
{

// Resource IDs of the 'STR ' resources in the System file containing the
// information configured in the "Sharing Setup" control panel.
const short wxMAC_STR_OWNER_NAME = -16096;
const short wxMAC_STR_COMPUTER_NAME = -16413;

wxString GetSystemString(short id)
{
    StringHandle h = GetString(id);
    if ( !h || !*h )
        return wxString();

    return wxMacMakeStringFromPascal(*h);
}

bool CopyToBuffer(const wxString& str, wxChar* buf, int sz)
{
    if ( str.empty() || sz <= 0 )
        return false;

    wxStrlcpy(buf, str.t_str(), sz);

    return true;
}

} // anonymous namespace

bool wxGetHostName(wxChar *buf, int sz)
{
    wxString name = GetSystemString(wxMAC_STR_COMPUTER_NAME);
    if ( name.empty() )
        name = "Macintosh";

    return CopyToBuffer(name, buf, sz);
}

bool wxGetFullHostName(wxChar *buf, int sz)
{
    return wxGetHostName(buf, sz);
}

bool wxGetUserId(wxChar *buf, int sz)
{
    return wxGetUserName(buf, sz);
}

bool wxGetUserName(wxChar *buf, int sz)
{
    wxString name = GetSystemString(wxMAC_STR_OWNER_NAME);
    if ( name.empty() )
        name = "user";

    return CopyToBuffer(name, buf, sz);
}

const wxChar* wxGetHomeDir(wxString *home)
{
    // There is no notion of home directory under classic Mac OS, use the
    // documents folder if it exists (Mac OS 8.5+) or the root of the system
    // disk otherwise.
    *home = wxMacFindFolder('docs', false);
    if ( home->empty() )
        *home = wxMacFindFolder(kSystemFolderType, false).BeforeFirst(':') + ':';

    return home->c_str();
}

wxString wxGetUserHome(const wxString& WXUNUSED(user))
{
    wxString home;
    wxGetHomeDir(&home);

    return home;
}

// ============================================================================
// OS and hardware information
// ============================================================================

bool wxIsPlatform64Bit()
{
    return false;
}

wxString wxGetCpuArchitectureName()
{
#ifdef __POWERPC__
    return wxS("ppc");
#else
    return wxS("m68k");
#endif
}

wxString wxGetNativeCpuArchitectureName()
{
    // gestaltSysArchitecture is not available in the very old systems, but
    // these can only run on 68K machines anyhow.
    switch ( wxMacGestalt(gestaltSysArchitecture) )
    {
        case gestaltPowerPC:
            return wxS("ppc");

        case gestalt68k:
        default:
            return wxS("m68k");
    }
}

wxOperatingSystemId wxGetOsVersion(int *verMaj, int *verMin, int *verMicro)
{
    // The version is returned in BCD, e.g. 0x0761 for 7.6.1
    const long ver = wxMacGestalt(gestaltSystemVersion);

    if ( verMaj )
        *verMaj = ((ver >> 12) & 0xf) * 10 + ((ver >> 8) & 0xf);
    if ( verMin )
        *verMin = (ver >> 4) & 0xf;
    if ( verMicro )
        *verMicro = ver & 0xf;

    return wxOS_MAC_OS;
}

bool wxCheckOsVersion(int majorVsn, int minorVsn, int microVsn)
{
    int majorCur, minorCur, microCur;
    wxGetOsVersion(&majorCur, &minorCur, &microCur);

    return majorCur > majorVsn
        || (majorCur == majorVsn && minorCur > minorVsn)
        || (majorCur == majorVsn && minorCur == minorVsn && microCur >= microVsn);
}

wxString wxGetOsDescription()
{
    int major, minor, micro;
    wxGetOsVersion(&major, &minor, &micro);

    // Apple called it "System" before 7.5 and "Mac OS" since 7.6.
    wxString desc = major < 7 || (major == 7 && minor < 6) ? wxS("System")
                                                             : wxS("Mac OS");
    desc << ' ' << major << '.' << minor;
    if ( micro )
        desc << '.' << micro;

    desc << " (" << wxGetNativeCpuArchitectureName() << ")";

    return desc;
}

wxMemorySize wxGetFreeMemory()
{
    // Return the amount of memory available to this application, as there
    // is no virtual memory and each application has its own partition, this
    // is the most useful value.
    return FreeMem();
}

bool wxGetDiskSpace(const wxString& path,
                    wxDiskspaceSize_t *pTotal,
                    wxDiskspaceSize_t *pFree)
{
    FSSpec spec;
    const OSErr err = wxMacPathToFSSpec(path, &spec);
    if ( err != noErr && err != fnfErr )
        return false;

    HParamBlockRec pb;
    memset(&pb, 0, sizeof(pb));
    pb.volumeParam.ioVRefNum = spec.vRefNum;
    if ( PBHGetVInfoSync(&pb) != noErr )
        return false;

    // Notice that this API can't return sizes greater than 2GB, but this is
    // still better than nothing.
    const wxDiskspaceSize_t blockSize = pb.volumeParam.ioVAlBlkSiz;

    if ( pTotal )
        *pTotal = blockSize * static_cast<unsigned short>(pb.volumeParam.ioVNmAlBlks);
    if ( pFree )
        *pFree = blockSize * static_cast<unsigned short>(pb.volumeParam.ioVFrBlk);

    return true;
}

// ============================================================================
// environment
// ============================================================================

// There is no environment under classic Mac OS, but newlib still supports
// these functions allowing to set and use the variables inside the program.
bool wxGetEnv(const wxString& var, wxString *value)
{
    const char* p = getenv(var.mb_str());
    if ( !p )
        return false;

    if ( value )
        *value = wxString(p, wxConvLibc);

    return true;
}

bool wxSetEnv(const wxString& variable, const wxString& value)
{
    return setenv(variable.mb_str(), value.mb_str(), 1) == 0;
}

bool wxUnsetEnv(const wxString& variable)
{
    return unsetenv(variable.mb_str()) == 0;
}
