///////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/apptrait.h
// Purpose:     wxAppTraits for classic Mac OS (Retro68)
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_APPTRAIT_H_
#define _WX_RETRO68_APPTRAIT_H_

// ----------------------------------------------------------------------------
// wxConsoleAppTraits: wxAppTraits implementation for the console apps
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxConsoleAppTraits : public wxConsoleAppTraitsBase
{
public:
#if wxUSE_TIMER
    virtual wxTimerImpl *CreateTimerImpl(wxTimer *timer) override;
#endif // wxUSE_TIMER
};

#if wxUSE_GUI

// ----------------------------------------------------------------------------
// wxGUIAppTraits: wxAppTraits implementation for the GUI apps
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGUIAppTraits : public wxGUIAppTraitsBase
{
public:
    virtual wxEventLoopBase *CreateEventLoop() override;
    virtual wxPortId GetToolkitVersion(int *majVer = nullptr,
                                       int *minVer = nullptr,
                                       int *microVer = nullptr) const override;

#if wxUSE_TIMER
    virtual wxTimerImpl *CreateTimerImpl(wxTimer *timer) override;
#endif // wxUSE_TIMER
};

#endif // wxUSE_GUI

#endif // _WX_RETRO68_APPTRAIT_H_
