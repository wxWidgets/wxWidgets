///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/gui_resources.h
// Purpose:     Functions for getting GUI resources usage on Windows.
// Author:      Vadim Zeitlin
// Created:     2024-07-15
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_GUI_RESOURCES_H_
#define _WX_MSW_PRIVATE_GUI_RESOURCES_H_

// ----------------------------------------------------------------------------
// Get current or peak resources usage
// ----------------------------------------------------------------------------

// Simple struct contains the number of GDI and USER objects.
struct wxGUIObjectUsage
{
    unsigned long numGDI = 0;
    unsigned long numUSER = 0;

    bool operator==(const wxGUIObjectUsage& other) const
    {
        return numGDI == other.numGDI && numUSER == other.numUSER;
    }

    bool operator!=(const wxGUIObjectUsage& other) const
    {
        return !(*this == other);
    }
};

// Returns the number of GUI resources currently in use by this process.
WXDLLIMPEXP_CORE wxGUIObjectUsage wxGetCurrentlyUsedResources();

// Returns the highest number of GUI resources ever used by this process.
WXDLLIMPEXP_CORE wxGUIObjectUsage wxGetMaxUsedResources();

#endif // _WX_MSW_PRIVATE_GUI_RESOURCES_H_
