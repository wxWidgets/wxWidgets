///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/apptrait.h
// Purpose:     class implementing wxAppTraits for MSW
// Author:      Vadim Zeitlin
// Created:     21.06.2003
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_APPTRAIT_H_
#define _WX_MSW_APPTRAIT_H_

// ----------------------------------------------------------------------------
// wxGUI/ConsoleAppTraits: must derive from wxAppTraits, not wxAppTraitsBase
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxConsoleAppTraits : public wxConsoleAppTraitsBase
{
public:
    virtual wxEventLoopBase *CreateEventLoop() override;
    virtual void *BeforeChildWaitLoop() override;
    virtual void AfterChildWaitLoop(void *data) override;
#if wxUSE_TIMER
    virtual wxTimerImpl *CreateTimerImpl(wxTimer *timer) override;
#endif // wxUSE_TIMER
#if wxUSE_THREADS
    virtual bool DoMessageFromThreadWait() override;
    virtual WXDWORD WaitForThread(WXHANDLE hThread, int flags) override;
#endif // wxUSE_THREADS
    virtual bool CanUseStderr() override { return true; }
    virtual bool WriteToStderr(const wxString& text) override;
    virtual WXHWND GetMainHWND() const override { return nullptr; }
};

#if wxUSE_GUI

#if defined(__WXMSW__)

class WXDLLIMPEXP_CORE wxGUIAppTraits : public wxGUIAppTraitsBase
{
public:
    virtual wxEventLoopBase *CreateEventLoop() override;
    virtual void *BeforeChildWaitLoop() override;
    virtual void AfterChildWaitLoop(void *data) override;
#if wxUSE_TIMER
    virtual wxTimerImpl *CreateTimerImpl(wxTimer *timer) override;
#endif // wxUSE_TIMER
#if wxUSE_THREADS
    virtual bool DoMessageFromThreadWait() override;
    virtual WXDWORD WaitForThread(WXHANDLE hThread, int flags) override;
#endif // wxUSE_THREADS
    wxPortId GetToolkitVersion(int *majVer = nullptr,
                               int *minVer = nullptr,
                               int *microVer = nullptr) const override;

    virtual bool CanUseStderr() override;
    virtual bool WriteToStderr(const wxString& text) override;
    virtual WXHWND GetMainHWND() const override;
};

#elif defined(__WXGTK__)

class WXDLLIMPEXP_CORE wxGUIAppTraits : public wxGUIAppTraitsBase
{
public:
    virtual wxEventLoopBase *CreateEventLoop() override;
    virtual void *BeforeChildWaitLoop() override { return nullptr; }
    virtual void AfterChildWaitLoop(void *WXUNUSED(data)) override { }
#if wxUSE_TIMER
    virtual wxTimerImpl *CreateTimerImpl(wxTimer *timer) override;
#endif

#if wxUSE_THREADS
    virtual void MutexGuiEnter() override;
    virtual void MutexGuiLeave() override;
#endif

#if wxUSE_THREADS
    virtual bool DoMessageFromThreadWait() override { return true; }
    virtual WXDWORD WaitForThread(WXHANDLE hThread, int WXUNUSED(flags)) override
        { return DoSimpleWaitForThread(hThread); }
#endif // wxUSE_THREADS
    virtual wxPortId GetToolkitVersion(int *majVer = nullptr,
                                       int *minVer = nullptr,
                                       int *microVer = nullptr) const override;

    virtual bool CanUseStderr() override { return false; }
    virtual bool WriteToStderr(const wxString& WXUNUSED(text)) override
        { return false; }
    virtual WXHWND GetMainHWND() const override { return nullptr; }
};

#elif defined(__WXQT__)

class WXDLLIMPEXP_CORE wxGUIAppTraits : public wxGUIAppTraitsBase
{
public:
    virtual wxEventLoopBase *CreateEventLoop() override;
    virtual void *BeforeChildWaitLoop() override { return nullptr; }
    virtual void AfterChildWaitLoop(void*) override { }
#if wxUSE_TIMER
    virtual wxTimerImpl *CreateTimerImpl(wxTimer *timer) override;
#endif

#if wxUSE_THREADS
    virtual bool DoMessageFromThreadWait() override { return true; }
    virtual WXDWORD WaitForThread(WXHANDLE hThread, int WXUNUSED(flags)) override
        { return DoSimpleWaitForThread(hThread); }
#endif // wxUSE_THREADS
    virtual wxPortId GetToolkitVersion(int *majVer = nullptr,
                                       int *minVer = nullptr,
                                       int *microVer = nullptr) const override;

    virtual bool CanUseStderr() override { return false; }
    virtual bool WriteToStderr(const wxString&) override { return false; }
    virtual WXHWND GetMainHWND() const override { return nullptr; }
};

#endif

#endif // wxUSE_GUI

#endif // _WX_MSW_APPTRAIT_H_
