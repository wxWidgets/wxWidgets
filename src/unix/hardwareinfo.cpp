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

#ifdef __APPLE__
    #include <sys/sysctl.h>
#elif defined(__UNIX__)
    #include <sys/resource.h>
    #include <sys/sysinfo.h>
#endif

namespace wxSystemHardwareInfo
{
    wxMemorySize GetPeakUsedMemory()
    {
        rusage usage{};
        memset(&usage, 0, sizeof(rusage));
        if (getrusage(RUSAGE_SELF, &usage) == 0)
        {
            // ru_maxrss is in kilobytes
            return usage.ru_maxrss * 1024;
        }
        return -1;
    }

    wxMemorySize GetMemory()
    {
#ifdef __APPLE__
        int64_t memSize = 0;
        size_t sizeOfRetVal = sizeof(memSize);
        if (sysctlbyname("hw.memsize", &memSize, &sizeOfRetVal, nullptr, 0) != -1)
        {
            return memSize;
        }
#elif defined(__UNIX__)
        struct sysinfo status{};
        if (sysinfo(&status) == 0)
        {
            return status.totalram;
        }
#endif

        return -1;
    }
}
