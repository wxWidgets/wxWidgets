///////////////////////////////////////////////////////////////////////////////
// Name:        wx/hardwareinfo.h
// Purpose:     declaration of the wxSystemHardwareInfo class
// Author:      Blake Madden
// Created:     2025-07-19
// Copyright:   (c) 2025 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    @namespace wxSystemHardwareInfo

    Retrieval functions for the system's hardware and hardware usage.

    @since 3.3.1

    @library{wxbase}
 */
namespace wxSystemHardwareInfo
{
    /**
        Returns the number of CPUs.
     */
    inline int GetCPUCount();

    /**
        Returns the CPU architecture name, if available.
     */
    inline wxString GetCPUArchitectureName();

    /**
        Returns the native CPU architecture name, if available.
     */
    inline wxString GetNativeCPUArchitectureName();

    /**
        Returns @c true if the system's CPU architecture and the
        CPU architecture of the program are the same.
     */
    inline bool IsRunningNatively()
    {
        return wxGetCpuArchitectureName() == wxGetNativeCpuArchitectureName();
    }

    /**
        Returns the system's total physical memory (in bytes), or -1 upon failure.
     */
    wxMemorySize GetMemory();

    /**
        Returns the most amount of physical memory used at one time by the program
        (during its current run), or -1 upon failure.

        This can be useful for performance testing and debugging.
     */
    wxMemorySize GetPeakUsedMemory();

    /**
        Returns the amount of available physical memory (in bytes),
        or -1 upon failure.
     */
    inline wxMemorySize GetFreeMemory() { return wxGetFreeMemory(); }
}
