///////////////////////////////////////////////////////////////////////////////
// Name:        hardwareinfo.h
// Purpose:     declaration of the wxSystemHardwareInfo class
// Author:      Blake Madden
// Created:     2025-07-19
// Copyright:   (c) 2025 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "hardwareinfo.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/thread.h"
#endif // WX_PRECOMP

#include <psapi.h>
#include <sysinfoapi.h>
#include <windows.h>

namespace wxSystemHardwareInfo
{
    wxMemorySize GetPeakUsedMemory()
    {
        // https://docs.microsoft.com/en-us/windows/win32/psapi/collecting-memory-usage-information-for-a-process?redirectedfrom=MSDN
        PROCESS_MEMORY_COUNTERS memCounter;
        ::ZeroMemory(&memCounter, sizeof(PROCESS_MEMORY_COUNTERS));
        if (::GetProcessMemoryInfo(::GetCurrentProcess(), &memCounter, sizeof(memCounter)))
        {
            // PeakWorkingSetSize is in bytes
            return memCounter.PeakWorkingSetSize;
        }
        return -1;
    }

    wxMemorySize GetMemory()
    {
        MEMORYSTATUSEX status{};
        status.dwLength = sizeof(status);
        if (::GlobalMemoryStatusEx(&status))
        {
            return status.ullTotalPhys;
        }

        return -1;
    }
}
