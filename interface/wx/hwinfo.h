/////////////////////////////////////////////////////////////////////////////
// Name:        hwinfo.h
// Purpose:     interface of wxSystemHardwareInfo
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


/**
    @class wxSystemHardwareInfo

    This class holds information about the client's system hardware
    (e.g., physical memory, battery, CPU, etc.).

    All member functions are static and can accessed as such:
    @code
        wxMemorySize amountOfRAM = wxSystemHardwareInfo::GetPhysicalMemory();
        wxLogMessage("Total RAM installed: %s.",
                     wxFileName::GetHumanReadableSize(amountOfRAM.GetValue()));
    @endcode

    @since 3.3.0

    @library{wxbase}
    @category{cfg}

    @see wxPlatformInfo(), @ref group_funcmacro_networkuseros
*/
class wxSystemHardwareInfo
{
public:

    /**
        @name CPU information
    */
    //@{

    /**
        Returns the number of CPU cores.
    */
    static int GetCPUCount();

    /**
        Returns the CPU architecture name.

        This can be, for example, "x86_64", "arm64", or "i86pc".
        The name for the same CPU running on the same hardware
        can vary across operating systems.

        The returned string may be empty if the CPU architecture
        couldn't be recognized.

        @see wxGetCpuArchitectureName()
     */
    static wxString GetCPUArchitectureName();

    /**
        In some situations, the current process and native CPU
        architecture may be different.

        This returns the native CPU architecture regardless of
        the current process CPU architecture.

        Common examples for CPU architecture differences are the following:

        - Win32 process in x64 Windows (WoW)
        - Win32 or x64 process on ARM64 Windows (WoW64)
        - x86_64 process on ARM64 macOS (Rosetta 2)

        The returned string may be empty if the CPU architecture
        couldn't be recognized.

        @see ::wxGetNativeCpuArchitectureName()
     */
    static wxString GetNativeCPUArchitectureName();

    /**
        Returns @c true if the architecture that the program is running
        under is the same as the system's architecture. For example, this
        will be @c if running a 32-bit application under x64 (Windoes) or
        running x86_64 under ARM64 (macOS)
    */
    static bool IsRunningNatively();

    //@}


    /**
        @name Physical memory (i.e., RAM) information
    */
    //@{

    /**
        Returns the amount of installed memory in bytes under environments
        which support it, and -1 if not supported or failed to perform measurement.
    */
    static wxMemorySize GetPhysicalMemory();
    /**
        Returns the amount of free memory in bytes under environments
        which support it, and -1 if not supported or failed to perform measurement.

        @see ::wxGetFreeMemory()
    */
    static wxMemorySize GetFreePhysicalMemory();

    //@}

    /**
        @name Battery information
    */
    //@{

    /**
        Returns battery state as one of `wxBATTERY_NORMAL_STATE`,
        `wxBATTERY_LOW_STATE, wxBATTERY_CRITICAL_STATE`,
        `wxBATTERY_SHUTDOWN_STATE` or `wxBATTERY_UNKNOWN_STATE`.

        Currently only implemented on MS Windows;
        returns `wxBATTERY_UNKNOWN_STATE` elsewhere.
    */
    static wxBatteryState GetBatteryState();
    /**
        Returns the remaining battery life (as a percentage), or -1 if
        unknown (or not applicable).

        Currently only implemented on MS Windows;
        returns -1 elsewhere.
    */
    static int GetRemainingBatteryLifePercent();
    /**
        Returns the type of power source as one of `wxPOWER_SOCKET`,
        `wxPOWER_BATTERY` or `wxPOWER_UNKNOWN`.

        Currently only implemented on MS Windows;
        returns `wxPOWER_UNKNOWN` elsewhere.
    */
    static wxPowerType GetPowerType();

    //@}
};
