/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_button_n.mm
// Purpose:     wxMoButton class
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

#include "wx/mobile/native/button.h"
#include "wx/mobile/native/utils.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/panel.h"
    #include "wx/bmpbuttn.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
#endif

#include "wx/dcbuffer.h"
#include "wx/stockitem.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoButton, wxControl)

BEGIN_EVENT_TABLE(wxMoButton, wxControl)
    EVT_PAINT(wxMoButton::OnPaint)
    EVT_MOUSE_EVENTS(wxMoButton::OnMouseEvent)
    EVT_ERASE_BACKGROUND(wxMoButton::OnEraseBackground)
END_EVENT_TABLE()

#define EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)    (cy+8)

// this macro tries to adjust the default button height to a reasonable value
// using the char height as the base
#define BUTTON_HEIGHT_FROM_CHAR_HEIGHT(cy) (11*EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)/10)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

/// Default constructor.
wxMoButton::wxMoButton()
{
    Init();
}

/// Constructor taking a text label.
wxMoButton::wxMoButton(wxWindow *parent,
                       wxWindowID id,
                       const wxString& label,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    Init();
    Create(parent, id, label, pos, size, style, validator, name);
}

bool wxMoButton::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& lbl,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    return wxButton::Create(parent, id, lbl, pos, size, style, validator, name);
}

wxMoButton::~wxMoButton()
{
    
}

void wxMoButton::Init()
{
    
}

#if 0
wxSize wxMoButton::DoGetBestSize() const
{
    // FIXME stub

    wxSize empty(1, 1);
    return empty;
}

wxSize wxMoButton::IPGetDefaultSize() const
{
    // FIXME stub

    static wxSize s_sizeBtn(1, 1);
    return s_sizeBtn;
}

bool wxMoButton::SendClickEvent()
{
    // FIXME stub

    return true;
}

bool wxMoButton::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoButton::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoButton::Enable(bool enable)
{
    // FIXME stub

    return true;
}

void wxMoButton::SetLabel( const wxString &label )
{
    // FIXME stub
}

bool wxMoButton::SetFont(const wxFont& font)
{
    // FIXME stub

    return true;
}
#endif

void wxMoButton::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoButton::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}

void wxMoButton::OnMouseEvent(wxMouseEvent& event)
{
    // FIXME stub
}
