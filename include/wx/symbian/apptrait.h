///////////////////////////////////////////////////////////////////////////////
// Name:        wx/symbian/apptrait.h
// Purpose:     class implementing wxAppTraits for Symbian
// Author:      Andrei Matuk
// RCS-ID:      $Id$
// Copyright:   (c) Andrei Matuk
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SYMBIAN_APPTRAIT_H_
#define _WX_SYMBIAN_APPTRAIT_H_

// ----------------------------------------------------------------------------
// wxGUI/ConsoleAppTraits: must derive from wxAppTraits, not wxAppTraitsBase
// ----------------------------------------------------------------------------
//#include "wx/apptrait.h"

class wxConsoleAppTraits: public wxConsoleAppTraitsBase
    {
    #if wxUSE_CONSOLE_EVENTLOOP
        virtual wxEventLoopBase *CreateEventLoop();
    #endif // wxUSE_CONSOLE_EVENTLOOP
    #if wxUSE_TIMER
        virtual wxTimerImpl *CreateTimerImpl(wxTimer *timer);
    #endif
    };

#if wxUSE_GUI

class WXDLLIMPEXP_CORE wxGUIAppTraits : public wxGUIAppTraitsBase
{
public:
    virtual void *BeforeChildWaitLoop();
    virtual void AlwaysYield();
    virtual void AfterChildWaitLoop(void *data);

    virtual bool DoMessageFromThreadWait();
    virtual wxToolkitInfo& GetToolkitInfo();
};

#endif // wxUSE_GUI

#endif // _WX_SYMBIAN_APPTRAIT_H_

