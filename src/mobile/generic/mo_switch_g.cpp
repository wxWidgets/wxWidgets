/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_switch_g.cpp
// Purpose:     wxMoSwitchCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcclient.h"
#include "wx/dcbuffer.h"

#include "wx/mobile/generic/switch.h"
#include "wx/mobile/generic/utils.h"

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
    if ( !wxControl::Create(parent, id, pos, size, style|wxBORDER_NONE, validator, name) )
        return false;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_SWITCH_OFF_BG));
    SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_SWITCH));

    SetInitialSize(size);

    return true;
}

wxMoSwitchCtrl::~wxMoSwitchCtrl()
{
}

void wxMoSwitchCtrl::Init()
{
    m_value = true;
}

wxSize wxMoSwitchCtrl::DoGetBestSize() const
{
    wxClientDC dc(wx_const_cast(wxMoSwitchCtrl *, this));
    dc.SetFont(GetFont());

    // Pixels between labels and edges
    int spacing = SWITCH_EDGE_MARGIN;

    int totalSpacing = spacing*4;

    wxString onString(_("ON"));
    wxString offString(_("OFF"));
    
    wxCoord w1, h1, w2, h2;
    dc.GetTextExtent(onString, &w1, &h1);
    dc.GetTextExtent(offString, &w2, &h2);

    int width = totalSpacing + w1 + w2;
    int height = BUTTON_HEIGHT_FROM_CHAR_HEIGHT(h1);
    
    wxSize best(width, height);
    CacheBestSize(best);
    return best;
}

bool wxMoSwitchCtrl::SendClickEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, GetId());
    event.SetInt(GetValue() ? 1 : 0);
    event.SetEventObject(this);

    return ProcessCommand(event);
}

bool wxMoSwitchCtrl::SetBackgroundColour(const wxColour &colour)
{
    if ( !wxControl::SetBackgroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();

    return true;
}

bool wxMoSwitchCtrl::SetForegroundColour(const wxColour &colour)
{
    if ( !wxControl::SetForegroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();

    return true;
}

bool wxMoSwitchCtrl::Enable(bool enable)
{
    wxControl::Enable(enable);
    Refresh();
    return true;
}

bool wxMoSwitchCtrl::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);

    Refresh();

    return true;
}

void wxMoSwitchCtrl::SetValue(bool value)
{
    m_value = value;
    Refresh();
}

bool wxMoSwitchCtrl::GetValue() const
{
    return m_value;
}

void wxMoSwitchCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif

    wxRect rect(wxPoint(0, 0), GetSize());
    int state = 0;
    if (!IsEnabled())
        state = wxCTRL_STATE_DISABLED;

    int sliderState = m_pressDetector.GetButtonState();
    if (!IsEnabled())
        sliderState = wxCTRL_STATE_DISABLED;

    wxRect winRect(wxPoint(0, 0), GetSize());

    wxRect leftRect;
    wxRect rightRect;
    if (GetValue())
    {
        leftRect = wxRect(0, 0, winRect.width * 0.6, winRect.height);
        rightRect = wxRect(leftRect.width, 0, winRect.width - leftRect.width, winRect.height);
    }
    else
    {
        leftRect = wxRect(0, 0, winRect.width * 0.4, winRect.height);
        rightRect = wxRect(leftRect.width, 0, winRect.width - leftRect.width, winRect.height);
    }

    // Pixels between labels and edges
    int spacing = SWITCH_EDGE_MARGIN;
    wxUnusedVar(spacing);
    wxString onString(_("ON"));
    wxString offString(_("OFF"));

    wxColour sliderColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_SWITCH_THUMB_BG));

    wxColour backgroundColour;
    if (GetValue())
        backgroundColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_SWITCH_ON_BG);
    else
        backgroundColour = GetBackgroundColour();

    wxColour parentColour(GetParent()->GetBackgroundColour());
    wxMoRenderer::DrawButtonBackground(this, dc, parentColour, backgroundColour, wxColour(), winRect, state, wxMO_BUTTON_TWO_TONE, -8);
    if (GetValue())
    {
        wxMoRenderer::DrawButtonBackground(this, dc, wxColour(), sliderColour, wxColour(), rightRect, sliderState, wxMO_BUTTON_TWO_TONE, -8);
    }
    else
    {
        wxMoRenderer::DrawButtonBackground(this, dc, wxColour(), sliderColour, wxColour(), leftRect, sliderState, wxMO_BUTTON_TWO_TONE, -8);
    }

    wxColour textColour;
    if (GetValue())
        textColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_SWITCH_ON_TEXT);
    else
        textColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_SWITCH_OFF_TEXT);

    dc.SetTextForeground(textColour);
    dc.SetFont(GetFont());

    wxCoord w1, h1, w2, h2;
    dc.GetTextExtent(onString, &w1, &h1);
    dc.GetTextExtent(offString, &w2, &h2);

    if (GetValue())
    {
        int x1 = leftRect.x + (leftRect.width - w1)/2;
        int y1 = (leftRect.height - h1)/2;
        dc.DrawText(onString, x1, y1);
    }

    if (!GetValue())
    {
        int x2 = rightRect.x + (rightRect.width - w2)/2;
        int y2 = (rightRect.height - h2)/2;
        dc.DrawText(offString, x2, y2);
    }
}

void wxMoSwitchCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}


void wxMoSwitchCtrl::OnMouseEvent(wxMouseEvent& event)
{
    if (!IsEnabled())
        return;

    wxRect winRect(GetRect());

    wxRect rect(wxPoint(0, 0), GetSize());

    if (m_pressDetector.ProcessMouseEvent(this, rect, event))
    {
        SetFocus();
        SetValue(!GetValue());
        SendClickEvent();
        Refresh();
    }
    else if ((event.LeftUp() || event.Leaving()) && (wxWindow::GetCapture() == this))
    {
        SetFocus();
        m_pressDetector.SetButtonState(0);
        ReleaseMouse();
    }
}
