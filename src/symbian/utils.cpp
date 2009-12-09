/////////////////////////////////////////////////////////////////////////////
// Name:        src/symbian/utils.cpp
// Purpose:     Various utilities
// Author:      Andrei Matuk
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Andrei Matuk <Veon.UA@gmail.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif  //WX_PRECOMP

#include "wx/power.h"
#include "wx/apptrait.h"
#include "wx/confbase.h"
#include "wx/timer.h"
#include "wx/apptrait.h"
#include "wx/filename.h"
#include <f32file.h>
#include <e32std.h>
#include <stdlib.h>
#include <sys/unistd.h>

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// get host name and related
// ----------------------------------------------------------------------------

// Get hostname only (without domain name)
bool wxGetHostName(wxChar *buf, int maxSize)
{
    return false;
}

// get full hostname (with domain name if possible)
bool wxGetFullHostName(wxChar *buf, int maxSize)
{
    return false;
}

// Get user ID e.g. jacs
bool wxGetUserId(wxChar *buf, int maxSize)
{
    return wxGetUserName(buf, maxSize);
}

// Get user name e.g. Julian Smart
bool wxGetUserName(wxChar *buf, int maxSize)
{
    return false;
}

const wxChar* wxGetHomeDir(wxString *pstr)
{
    RFs fsSession;
    fsSession.Connect();
    User::LeaveIfError(fsSession.CreatePrivatePath(EDriveC));
    TFileName name;
    User::LeaveIfError(fsSession.PrivatePath(name));

    wxString str = wxString(name);
    str.erase(str.Length()-1,1);
    pstr->assign(str);

    fsSession.Close();

    return pstr->c_str();
}

wxString wxGetUserHome(const wxString& user)
{
    return wxT("");
}

bool wxGetDiskSpace(const wxString& path, wxLongLong *pTotal, wxLongLong *pFree)
{
    wxFileName filename(path);
    wxString   volume = filename.GetVolume();

    if (volume != wxT(""))
    {
        volume = volume.MakeLower();
        int drive = int(volume[0]) - int(_T('a'));

        RFs fsSession;
        fsSession.Connect();

        TVolumeInfo volumeInfo;
        if (fsSession.Volume(volumeInfo, EDriveA+drive) == KErrNone)
        {
#if defined(__SERIES60_1X__) || defined(__SERIES60_2X__)
            *pTotal = wxLongLong(volumeInfo.iSize.High(), volumeInfo.iSize.Low());
            *pFree  = wxLongLong(volumeInfo.iFree.High(), volumeInfo.iFree.Low());
#else
            *pTotal = wxLongLong(I64HIGH(volumeInfo.iSize), I64LOW(volumeInfo.iSize));
            *pFree  = wxLongLong(I64HIGH(volumeInfo.iFree), I64LOW(volumeInfo.iFree));
#endif
            return true;
        }
    }

    return false;
}

// ----------------------------------------------------------------------------
// env vars
// ----------------------------------------------------------------------------

bool wxGetEnv(const wxString& WXUNUSED_IN_WINCE(var),
              wxString *WXUNUSED_IN_WINCE(value))
{
    // no environment variables in Symbian
    return false;
}

// ----------------------------------------------------------------------------
// process management
// ----------------------------------------------------------------------------
// see http://wiki.forum.nokia.com/index.php/How_to_start_and_stop_exe for implement
int wxKill(long pid, wxSignal sig, wxKillError *krc, int flags)
{
    return 0;
}

// Execute a program in an Interactive Shell
bool wxShell(const wxString& command)
{
    return false;
}

// Shutdown or reboot the PC
bool wxShutdown(wxShutdownFlags wFlags)
{
    return false;
}

#include <etel3rdparty.h>
CTelephony::TBatteryInfoV1 wxGetBatteryInfo()
{
    CTelephony* iTelephony;
    TRequestStatus iStatus;
    CTelephony::TBatteryInfoV1      iBatteryInfoV1;
    CTelephony::TBatteryInfoV1Pckg  iBatteryInfoV1Pckg(iBatteryInfoV1);
    iTelephony->GetBatteryInfo(iStatus, iBatteryInfoV1Pckg); // Create asynchronous request to Telephony server
    User::WaitForRequest(iStatus);
    return iBatteryInfoV1;
}

wxPowerType wxGetPowerType()
{
    switch (wxGetBatteryInfo().iStatus)    {
        case CTelephony::EPoweredByBattery:
            return wxPOWER_BATTERY;
        case CTelephony::ENoBatteryConnected:
        case CTelephony::EBatteryConnectedButExternallyPowered:
            return wxPOWER_SOCKET;
        default : return wxPOWER_UNKNOWN;
    }
}


wxBatteryState wxGetBatteryState()
{
    TUint percent = wxGetBatteryInfo().iChargeLevel;
    // Put your values
    if (percent<6)  return wxBATTERY_SHUTDOWN_STATE;
    if (percent<16) return wxBATTERY_CRITICAL_STATE;
    if (percent<36) return wxBATTERY_LOW_STATE;
    return wxBATTERY_NORMAL_STATE;
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
wxMemorySize wxGetFreeMemory()
{
    int largestblock;
    return User::Available(largestblock);    // Returns total free memory, as opposed to largest available block.
}

unsigned long wxGetProcessId()
{
    return getpid();
}

// Emit a beeeeeep
void wxBell()
{
    User::Beep(100, 50000);
}

wxString wxGetOsDescription()
{
    TVersion ver = User::Version();

    wxString strOS = wxString::Format(_T("SymbianOS %d.%d (Build %d)"), ver.iMajor, ver.iMinor, ver.iBuild);
    return strOS;
}

// see http://wiki.forum.nokia.com/index.php/CS000933_-_Detecting_S60_platform_version_at_run_time
#include <f32file.h>

wxOperatingSystemId wxGetOsVersion(int *verMaj, int *verMin)
{
    RFs aFs;
    aFs.Connect();
    TFindFile fileFinder(aFs);
    CDir* result;

    _LIT(KS60ProductIDFile, "Series60v*.sis");
    _LIT(KROMInstallDir, "z:\\system\\install\\");

    User::LeaveIfError(fileFinder.FindWildByDir(KS60ProductIDFile,
            KROMInstallDir, result));
    CleanupStack::PushL(result);
    User::LeaveIfError(result->Sort(ESortByName | EDescending));

    TUint aMajor = (*result)[0].iName[9] - '0';
    TUint aMinor = (*result)[0].iName[11] - '0';
    if (verMaj) *verMaj = aMajor;
    if (verMin) *verMin = aMinor;
    CleanupStack::PopAndDestroy(result);
    aFs.Close();
    return wxOS_SYMBIAN_S60;
    }


bool wxIsPlatform64Bit()
{
 return false;
}

// ----------------------------------------------------------------------------
// sleep functions
// ----------------------------------------------------------------------------

void wxMilliSleep(unsigned long milliseconds)
{
    User::After(milliseconds * 1000);
}

void wxMicroSleep(unsigned long microseconds)
{
    User::After(microseconds);
}

void wxSleep(int nSecs)
{
    wxMilliSleep(1000*nSecs);
}
