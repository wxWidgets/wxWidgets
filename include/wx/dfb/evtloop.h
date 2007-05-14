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

#include "wx/dfb/dfbptr.h"

wxDFB_DECLARE_INTERFACE(IDirectFBEventBuffer);
struct wxDFBEvent;

// ----------------------------------------------------------------------------
// wxEventLoop
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxEventLoopManual
{
public:
    wxGUIEventLoop();

    virtual bool Pending() const;
    virtual bool Dispatch();

    // returns DirectFB event buffer used by wx
    static wxIDirectFBEventBufferPtr GetDirectFBEventBuffer();

    // wxYield implementation: iterate the loop as long as there are any
    // pending events
    void Yield();

protected:
    virtual void WakeUp();
    virtual void OnNextIteration();

    virtual void HandleDFBEvent(const wxDFBEvent& event);

private:
    static void InitBuffer();
    static void CleanUp();

    friend class wxApp; // calls CleanUp() and WakeUp()

private:
    static wxIDirectFBEventBufferPtr ms_buffer;

    DECLARE_NO_COPY_CLASS(wxGUIEventLoop)
};

#endif // _WX_DFB_EVTLOOP_H_
