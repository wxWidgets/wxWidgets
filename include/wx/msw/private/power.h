///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/power.h
// Purpose:     Private helpers for MSW power resource blocker
// Author:      merhoumTaki
// Created:     2026-08-18
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_POWER_H_
#define _WX_MSW_PRIVATE_POWER_H_

// Updates shutdown block reason for top-level MSW windows.
bool wxMSWUpdateShutdownBlockReason();

// Returns true if a system power resource block is currently active.
bool wxMSWPowerResourceIsSystemBlockActive();

#endif // _WX_MSW_PRIVATE_POWER_H_
