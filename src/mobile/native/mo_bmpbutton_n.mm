/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_bmpbutton_n.mm
// Purpose:     wxMoBitmapButton class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/native/bmpbutton.h"
#include "wx/mobile/native/utils.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/panel.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
    #include "wx/image.h"
#endif

#include "wx/mstream.h"

#include "wx/dcbuffer.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoBitmapButton, wxBitmapButton)

BEGIN_EVENT_TABLE(wxMoBitmapButton, wxBitmapButton)
#if 0
    EVT_PAINT(wxMoBitmapButton::OnPaint)
    EVT_MOUSE_EVENTS(wxMoBitmapButton::OnMouseEvent)
    EVT_ERASE_BACKGROUND(wxMoBitmapButton::OnEraseBackground)
#endif
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

/// Default constructor.
wxMoBitmapButton::wxMoBitmapButton()
{
    Init();
}

/// Constructor taking a bitmap.
wxMoBitmapButton::wxMoBitmapButton(wxWindow *parent,
                                   wxWindowID id,
                                   const wxBitmap& label,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxValidator& validator,
                                   const wxString& name)
{
    Init();
    Create(parent, id, label, pos, size, style, validator, name);
}

bool wxMoBitmapButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxBitmap& bitmap,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    return wxBitmapButton::Create(parent, id, bitmap, pos, size, style, validator, name);
}

wxMoBitmapButton::~wxMoBitmapButton()
{
}

void wxMoBitmapButton::Init()
{
    // FIXME stub
}

#if 0
wxSize wxMoBitmapButton::DoGetBestSize() const
{
    // FIXME stub
    
    wxSize best(10, 10);
    return best;
}

wxSize wxMoBitmapButton::IPGetDefaultSize()
{
    // FIXME stub

    wxSize empty(1, 1);
    return empty;
}

bool wxMoBitmapButton::SendClickEvent()
{
    // FIXME stub

    return true;
}

bool wxMoBitmapButton::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoBitmapButton::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoBitmapButton::Enable(bool enable)
{
    // FIXME stub

    return true;
}

void wxMoBitmapButton::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoBitmapButton::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}


void wxMoBitmapButton::OnMouseEvent(wxMouseEvent& event)
{
    // FIXME stub
}

#endif  // 0
