///////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/evtloop.h
// Purpose:     declares wxEventLoop class
// Author:      Vaclav Slavik
// Created:     2006-08-16
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_EVTLOOP_H_
#define _WX_DFB_EVTLOOP_H_

#include "wx/dfb/dfbptr.h"
#include "wx/unix/evtloop.h"

wxDFB_DECLARE_INTERFACE(IDirectFBEventBuffer);

// ----------------------------------------------------------------------------
// wxEventLoop
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxConsoleEventLoop
{
public:
    wxGUIEventLoop();

    // returns DirectFB event buffer used by wx
    static wxIDirectFBEventBufferPtr GetDirectFBEventBuffer();

protected:
    virtual void DoYieldFor(long eventsToProcess);

private:
    static void InitBuffer();
    static void CleanUp();

    friend class wxApp; // calls CleanUp()

private:
    static wxIDirectFBEventBufferPtr ms_buffer;
    static int ms_bufferFd;

    wxDECLARE_NO_COPY_CLASS(wxGUIEventLoop);
};

#endif // _WX_DFB_EVTLOOP_H_
