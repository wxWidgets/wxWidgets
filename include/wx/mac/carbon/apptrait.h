///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/apptrait.h
// Purpose:     standard implementations of wxAppTraits for MacOS
// Author:      Vadim Zeitlin
// Modified by: Stefan Csomor
// Created:     23.06.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_APPTRAIT_H_
#define _WX_MAC_APPTRAIT_H_

// ----------------------------------------------------------------------------
// wxGUI/ConsoleAppTraits: must derive from wxAppTraits, not wxAppTraitsBase
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxConsoleAppTraits : public wxConsoleAppTraitsBase
{
public:
    // no timer support in wxBase yet
#if wxUSE_TIMER
    virtual wxTimerImpl *CreateTimerImpl(wxTimer *timer) { return NULL; };
#endif
    // other miscellaneous helpers
    // ---------------------------

    virtual wxToolkitInfo& GetToolkitInfo();
};

#if wxUSE_GUI

class WXDLLEXPORT wxGUIAppTraits : public wxGUIAppTraitsBase
{
public:
#if wxUSE_TIMER
    virtual wxTimerImpl *CreateTimerImpl(wxTimer *timer);
#endif
    // other miscellaneous helpers
    // ---------------------------

    virtual wxToolkitInfo& GetToolkitInfo();
};

#endif // wxUSE_GUI

#endif // _WX_MAC_APPTRAIT_H_

