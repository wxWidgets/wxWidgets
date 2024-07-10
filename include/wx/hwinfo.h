///////////////////////////////////////////////////////////////////////////////
// Name:        wx/hwinfo.h
// Purpose:     declaration of the wxSystemHardwareInfo class
// Author:      Blake Madden
// Created:     07.09.2024
// Copyright:   (c) 2024 Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SYSTEMHWINFO_H_
#define _WX_SYSTEMHWINFO_H_

#include "wx/string.h"
#include "wx/utils.h"
#include "wx/power.h"

// ----------------------------------------------------------------------------
// wxSystemHardwareInfo
// ----------------------------------------------------------------------------

// Information about the system's hardware
class WXDLLIMPEXP_BASE wxSystemHardwareInfo
{
public:
    
    static int GetCPUCount();

    static wxString GetCPUArchitectureName();
    static wxString GetNativeCPUArchitectureName();
    static bool IsRunningNatively();

    static wxMemorySize GetPhysicalMemory();
    static wxMemorySize GetFreePhysicalMemory();

    static wxBatteryState GetBatteryState();
    static int GetRemainingBatteryLifePercent();
    static wxPowerType GetPowerType();
};

#endif // _WX_SYSTEMHWINFO_H_
