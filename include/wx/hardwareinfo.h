///////////////////////////////////////////////////////////////////////////////
// Name:        wx/hardwareinfo.h
// Purpose:     declaration of the wxSystemHardwareInfo class
// Author:      Blake Madden
// Created:     2025-07-19
// Copyright:   (c) 2025 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SYSTEMHWINFO_H_
#define _WX_SYSTEMHWINFO_H_

#include "wx/power.h"
#include "wx/string.h"
#include "wx/utils.h"

// ----------------------------------------------------------------------------
// wxSystemHardwareInfo
// ----------------------------------------------------------------------------

// Information about the system's hardware
namespace wxSystemHardwareInfo
    {
    inline int GetCPUCount() { return wxThread::GetCPUCount(); }

    inline wxString GetCPUArchitectureName() { return wxGetCpuArchitectureName(); }

    inline wxString GetNativeCPUArchitectureName() { return wxGetNativeCpuArchitectureName(); }

    inline bool IsRunningNatively()
    {
        return wxGetCpuArchitectureName() == wxGetNativeCpuArchitectureName();
    }

    wxMemorySize GetMemory();

    wxMemorySize GetPeakUsedMemory();

    inline wxMemorySize GetFreeMemory() { return wxGetFreeMemory(); }
    }

#endif // _WX_SYSTEMHWINFO_H_
