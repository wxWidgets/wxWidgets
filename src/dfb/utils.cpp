/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/utils.cpp
// Purpose:     Miscellaneous utility functions and classes
// Author:      Vaclav Slavik
// Created:     2006-08-08
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/utils.h"
#include "wx/evtloop.h"
#include "wx/apptrait.h"
#include "wx/private/display.h"
#include "wx/unix/private/timer.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "wx/dfb/private.h"
#include <directfb_version.h>

// ----------------------------------------------------------------------------
// toolkit info
// ----------------------------------------------------------------------------

wxPortId wxGUIAppTraits::GetToolkitVersion(int *verMaj,
                                           int *verMin,
                                           int *verMicro) const
{
    if ( verMaj ) *verMaj = DIRECTFB_MAJOR_VERSION;
    if ( verMin ) *verMaj = DIRECTFB_MINOR_VERSION;
    if ( verMicro ) *verMicro = DIRECTFB_MICRO_VERSION;

    return wxPORT_DFB;
}


wxEventLoopBase* wxGUIAppTraits::CreateEventLoop()
{
    return new wxEventLoop;
}

wxTimerImpl *wxGUIAppTraits::CreateTimerImpl(wxTimer *timer)
{
    return new wxUnixTimerImpl(timer);
}

// ----------------------------------------------------------------------------
// display characteristics
// ----------------------------------------------------------------------------

// TODO: move into a separate src/dfb/display.cpp

class wxDisplayImplSingleDFB : public wxDisplayImplSingle
{
public:
    virtual wxRect GetGeometry() const override
    {
        const wxVideoMode mode(wxTheApp->GetDisplayMode());

        return wxRect(0, 0, mode.w, mode.h);
    }

    virtual int GetDepth() const override
    {
        return wxTheApp->GetDisplayMode().bpp;
    }

    virtual wxSize GetPPI() const override
    {
        // FIXME: there's no way to get physical resolution using the DirectDB
        //        API, we hardcode a commonly used value of 72dpi
        return wxSize(72, 72);
    }
};

class wxDisplayFactorySingleDFB : public wxDisplayFactorySingle
{
protected:
    virtual wxDisplayImpl *CreateSingleDisplay()
    {
        return new wxDisplayImplSingleDFB;
    }
};

wxDisplayFactory* wxDisplay::CreateFactory()
{
    return new wxDisplayFactorySingleDFB;
}

//-----------------------------------------------------------------------------
// mouse
//-----------------------------------------------------------------------------

void wxGetMousePosition(int *x, int *y)
{
    wxIDirectFBDisplayLayerPtr layer(wxIDirectFB::Get()->GetDisplayLayer());
    if ( layer )
        layer->GetCursorPosition(x, y);
}

wxPoint wxGetMousePosition()
{
    wxPoint pt;
    wxGetMousePosition(&pt.x, &pt.y);
    return pt;
}

//-----------------------------------------------------------------------------
// keyboard
//-----------------------------------------------------------------------------

bool wxGetKeyState(wxKeyCode key)
{
    wxASSERT_MSG(key != WXK_LBUTTON && key != WXK_RBUTTON && key != WXK_MBUTTON,
                 "can't use wxGetKeyState() for mouse buttons");

    return false; // FIXME
}

//----------------------------------------------------------------------------
// misc.
//----------------------------------------------------------------------------

void wxBell()
{
}
