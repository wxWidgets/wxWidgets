///////////////////////////////////////////////////////////////////////////////
// Name:        wx/hwinfo.h
// Purpose:     declaration of the wxSystemHardwareInfo class
// Author:      Blake Madden
// Created:     07.09.2024
// Copyright:   (c) 2024 Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "hwinfo.h"

#ifndef WX_PRECOMP
    #include "wx/thread.h"
#endif //WX_PRECOMP

#ifdef __WXMSW__
    #include <windows.h>
    #include <sysinfoapi.h>
#elif defined(__APPLE__)
    #include <sys/sysctl.h>
#elif defined(__UNIX__)
    #include <sys/resource.h>
    #include <sys/sysinfo.h>
#endif

int wxSystemHardwareInfo::GetRemainingBatteryLifePercent()
{
#ifdef __WXMSW__
    SYSTEM_POWER_STATUS sps;
    if (::GetSystemPowerStatus(&sps))
    {
        return sps.BatteryLifePercent;
    }
#endif
    // if no battery or unknown
    return -1;
}

wxBatteryState wxSystemHardwareInfo::GetBatteryState()
{
    return wxGetBatteryState();
}

wxPowerType wxSystemHardwareInfo::GetPowerType()
{
    return wxGetPowerType();
}

wxMemorySize wxSystemHardwareInfo::GetPhysicalMemory()
{
    wxMemorySize physicalMemory = -1;

#ifdef __WXMSW__
    MEMORYSTATUSEX status{};
    status.dwLength = sizeof(status);
    if (::GlobalMemoryStatusEx(&status))
    {
        physicalMemory = status.ullTotalPhys;
    }
#elif defined(__APPLE__)
    int64_t memSize{ 0 };
    size_t sizeOfRetVal{ sizeof(memSize) };
    if (sysctlbyname("hw.memsize", &memSize, &sizeOfRetVal, nullptr, 0) != -1)
    {
        physicalMemory = memSize;
    }
#elif defined(__UNIX__)
    struct sysinfo status{};
    if (sysinfo(&status) == 0)
    {
        physicalMemory = status.totalram;
    }
#endif

    return physicalMemory;
}

wxMemorySize wxSystemHardwareInfo::GetFreePhysicalMemory()
{
    return wxGetFreeMemory();
}

int wxSystemHardwareInfo::GetCPUCount()
{
    return 	wxThread::GetCPUCount();
}

wxString wxSystemHardwareInfo::GetCPUArchitectureName()
{
    return wxGetCpuArchitectureName();
}

wxString wxSystemHardwareInfo::GetNativeCPUArchitectureName()
{
    return wxGetNativeCpuArchitectureName();
}

bool wxSystemHardwareInfo::IsRunningNatively()
{
    return wxGetCpuArchitectureName() == wxGetNativeCpuArchitectureName();
}

