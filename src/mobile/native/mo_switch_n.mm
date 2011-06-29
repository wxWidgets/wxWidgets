/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_switch_n.mm
// Purpose:     wxMoSwitchCtrl class
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

#include "wx/dcclient.h"
#include "wx/dcbuffer.h"

#include "wx/mobile/native/switch.h"
#include "wx/mobile/native/utils.h"

extern WXDLLEXPORT_DATA(const wxChar) wxSwitchCtrlNameStr[] = wxT("switchctrl");

IMPLEMENT_DYNAMIC_CLASS(wxMoSwitchCtrl, wxControl)

BEGIN_EVENT_TABLE(wxMoSwitchCtrl, wxControl)
    EVT_PAINT(wxMoSwitchCtrl::OnPaint)
    EVT_MOUSE_EVENTS(wxMoSwitchCtrl::OnMouseEvent)
    EVT_ERASE_BACKGROUND(wxMoSwitchCtrl::OnEraseBackground)
END_EVENT_TABLE()

#define EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)    (cy+8)

// this macro tries to adjust the default button height to a reasonable value
// using the char height as the base
#define BUTTON_HEIGHT_FROM_CHAR_HEIGHT(cy) (11*EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)/10)

// Margin from edge to text
#define SWITCH_EDGE_MARGIN 10

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxMoSwitchCtrl::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    return wxCheckBox::Create(parent, id, wxEmptyString, pos, size, style, validator, name);
}

wxMoSwitchCtrl::~wxMoSwitchCtrl()
{
}

void wxMoSwitchCtrl::Init()
{
    
}

#if 0
wxSize wxMoSwitchCtrl::DoGetBestSize() const
{
    // FIXME stub

    wxSize empty(1, 1);
    return empty;
}

bool wxMoSwitchCtrl::SendClickEvent()
{
    // FIXME stub

    return true;
}

bool wxMoSwitchCtrl::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoSwitchCtrl::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoSwitchCtrl::Enable(bool enable)
{
    // FIXME stub

    return true;
}

bool wxMoSwitchCtrl::SetFont(const wxFont& font)
{
    // FIXME stub

    return true;
}

void wxMoSwitchCtrl::SetValue(bool value)
{
    // FIXME stub
}

bool wxMoSwitchCtrl::GetValue() const
{
    // FIXME stub

    return true;
}
#endif  // 0

void wxMoSwitchCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoSwitchCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}


void wxMoSwitchCtrl::OnMouseEvent(wxMouseEvent& event)
{
    // FIXME stub
}
