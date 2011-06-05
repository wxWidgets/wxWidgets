/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_button_g.cpp
// Purpose:     wxMoButton class
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

#include "wx/mobile/generic/button.h"
#include "wx/mobile/generic/utils.h"

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

bool wxMoButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString& lbl,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxString label(lbl);
    if (label.empty() && wxIsStockID(id))
    {
        label = wxGetStockLabel(id, false);
    }

    if ( !wxControl::Create(parent, id, pos, size, style|wxBORDER_NONE, validator, name) )
        return false;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    if (GetWindowStyle() & wxBU_ROUNDED_RECTANGLE)
    {
        SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_ROUNDED_RECT_BUTTON_BG));
        SetForegroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_ROUNDED_RECT_BUTTON_TEXT));
    }
    else
    {
        SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_NORMAL_BUTTON_BG));
        SetForegroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_NORMAL_BUTTON_TEXT));
    }
    SetBorderColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_NORMAL_BUTTON_BORDER));
    SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_NORMAL_BUTTON));

    SetLabel(label);
    SetInitialSize(size);

    return true;
}

wxMoButton::~wxMoButton()
{
}

void wxMoButton::Init()
{
}

wxSize wxMoButton::DoGetBestSize() const
{
    wxClientDC dc(wx_const_cast(wxMoButton *, this));
    dc.SetFont(GetFont());
    
    wxCoord wBtn,
        hBtn;
    dc.GetMultiLineTextExtent(GetLabelText(), &wBtn, &hBtn);
    
    // add a margin -- the button is wider than just its label
    wBtn += 3*GetCharWidth();
    hBtn = BUTTON_HEIGHT_FROM_CHAR_HEIGHT(hBtn);
    
    // all buttons have at least the standard size unless the user explicitly
    // wants them to be of smaller size and used wxBU_EXACTFIT style when
    // creating the button
    if ( !HasFlag(wxBU_EXACTFIT) )
    {
        wxSize sz = GetDefaultSize();
        if (wBtn > sz.x)
            sz.x = wBtn;
        if (hBtn > sz.y)
            sz.y = hBtn;
        
        return sz;
    }
    
    wxSize best(wBtn, hBtn);
    CacheBestSize(best);
    return best;
}

wxSize wxMoButton::IPGetDefaultSize()
{
    static wxSize s_sizeBtn;

    if ( s_sizeBtn.x == 0 )
    {
        wxScreenDC dc;
        dc.SetFont(wxMoSystemSettings::GetFont(wxMO_FONT_NORMAL_BUTTON));

        // the size of a standard button in the dialog units is 50x14,
        // translate this to pixels
        // NB1: the multipliers come from the Windows convention
        // NB2: the extra +1/+2 were needed to get the size be the same as the
        //      size of the buttons in the standard dialog - I don't know how
        //      this happens, but on my system this size is 75x23 in pixels and
        //      23*8 isn't even divisible by 14... Would be nice to understand
        //      why these constants are needed though!
        s_sizeBtn.x = (50 * (dc.GetCharWidth() + 1))/4;
        s_sizeBtn.y = ((14 * dc.GetCharHeight()) + 2)/8;
    }

    return s_sizeBtn;
}

bool wxMoButton::SendClickEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    event.SetEventObject(this);

    return ProcessCommand(event);
}

bool wxMoButton::SetBackgroundColour(const wxColour &colour)
{
    if ( !wxControl::SetBackgroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();

    return true;
}

bool wxMoButton::SetForegroundColour(const wxColour &colour)
{
    if ( !wxControl::SetForegroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();

    return true;
}

bool wxMoButton::Enable(bool enable)
{
    wxControl::Enable(enable);
    Refresh();
    return true;
}

void wxMoButton::SetLabel( const wxString &label )
{
    wxControl::SetLabel(label);

    OnSetLabel();
}

bool wxMoButton::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);

    OnSetLabel();

    return true;
}

void wxMoButton::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif

    wxRect rect(wxPoint(0, 0), GetSize());
    int state = m_pressDetector.GetButtonState();
    if (!IsEnabled())
        state = wxCTRL_STATE_DISABLED;

    wxColour parentColour(GetParent()->GetBackgroundColour());
    if (GetWindowStyle() & wxBU_ROUNDED_RECTANGLE)
    {
        dc.SetBrush(wxBrush(parentColour));
        dc.Clear();

        dc.SetPen(wxPen(GetBorderColour()));
        dc.SetBrush(wxBrush(GetBackgroundColour()));
        int corner = wxMax(8, rect.GetHeight()/6);
        dc.DrawRoundedRectangle(rect, corner);
    }
    else
    {
        wxMoRenderer::DrawButtonBackground(this, dc, parentColour, GetBackgroundColour(), GetBorderColour(), rect, state, wxMO_BUTTON_TWO_TONE);
    }

    wxSize sz = GetSize();
    dc.SetTextForeground(GetForegroundColour());
    dc.SetFont(GetFont());
    wxString label = GetLabelText();
    if (!label.IsEmpty())
    {
        int w, h;
        dc.GetTextExtent(label, & w, & h);
        // TODO: respect alignment and line breaks
        int x = (sz.x - w)/2;
        int y = (sz.y - h)/2;
        dc.DrawText(label, x, y);
    }
}

void wxMoButton::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}

void wxMoButton::OnMouseEvent(wxMouseEvent& event)
{
    if (!IsEnabled())
        return;

    wxRect rect(wxPoint(0, 0), GetRect().GetSize());
    if (m_pressDetector.ProcessMouseEvent(this, rect, event))
    {
        SendClickEvent();
    }
}
