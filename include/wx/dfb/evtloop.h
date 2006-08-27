///////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/evtloop.h
// Purpose:     declares wxEventLoop class
// Author:      Vaclav Slavik
// Created:     2006-08-16
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_EVTLOOP_H_
#define _WX_DFB_EVTLOOP_H_

#include "wx/dfb/ifacehelpers.h"

wxDFB_DECLARE_INTERFACE(IDirectFBEventBuffer);
struct wxDFBEvent;

// ----------------------------------------------------------------------------
// wxEventLoop
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxEventLoop : public wxEventLoopManual
{
public:
    wxEventLoop();

    virtual bool Pending() const;
    virtual bool Dispatch();

    // returns DirectFB event buffer used by wx
    static IDirectFBEventBufferPtr GetDirectFBEventBuffer();

protected:
    virtual void WakeUp();
    virtual void OnNextIteration();

    virtual void HandleDFBEvent(const wxDFBEvent& event);

private:
    static void InitBuffer();

private:
    static IDirectFBEventBufferPtr ms_buffer;

    friend class wxApp; // calls WakeUp()

    DECLARE_NO_COPY_CLASS(wxEventLoop)
};

#endif // _WX_DFB_EVTLOOP_H_
