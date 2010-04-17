/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/apptrait.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_APPTRAIT_H_
#define _WX_QT_APPTRAIT_H_

WXDLLIMPEXP_FWD_BASE class wxTimerImpl;

class WXDLLIMPEXP_BASE wxConsoleAppTraits : public wxConsoleAppTraitsBase
{
public:
    wxConsoleAppTraits();
    virtual ~wxConsoleAppTraits();

// #if !wxUSE_CONSOLE_EVENTLOOP
    virtual wxEventLoopBase *CreateEventLoop();
// #endif // !wxUSE_CONSOLE_EVENTLOOP

    wxTimerImpl *CreateTimerImpl(wxTimer *timer);
protected:

private:
};

#if wxUSE_GUI

class WXDLLIMPEXP_CORE wxGUIAppTraits : public wxGUIAppTraitsBase
{
public:
    wxGUIAppTraits();
    virtual ~wxGUIAppTraits();

protected:

private:
    
    // From ../unix/apptbase.h:
public:
    virtual int WaitForChild(wxExecuteData& execData);
};

#endif // wxUSE_GUI

#endif // _WX_QT_APPTRAIT_H_

