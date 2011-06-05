/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_bmpbutton_g.cpp
// Purpose:     wxMoBitmapButton class
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

#include "wx/mobile/generic/bmpbutton.h"
#include "wx/mobile/generic/utils.h"

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

#include "wx/mobile/generic/bitmaps/disclosure_16x20.inc"

#include "wx/dcbuffer.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoBitmapButton, wxControl)

BEGIN_EVENT_TABLE(wxMoBitmapButton, wxControl)
    EVT_PAINT(wxMoBitmapButton::OnPaint)
    EVT_MOUSE_EVENTS(wxMoBitmapButton::OnMouseEvent)
    EVT_ERASE_BACKGROUND(wxMoBitmapButton::OnEraseBackground)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxMoBitmapButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxBitmap& bitmap,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    m_bmpNormal = bitmap;

    if (style & wxBU_DISCLOSURE)
    {
        wxMemoryInputStream is(disclosure_16x20_png, sizeof(disclosure_16x20_png));
        m_bmpNormal = wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);
    }

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    
    SetInitialSize(size);

    return true;
}

wxMoBitmapButton::~wxMoBitmapButton()
{
}

void wxMoBitmapButton::Init()
{
    m_marginX = 4;
    m_marginY = 4;
}

wxSize wxMoBitmapButton::DoGetBestSize() const
{
    wxSize best(10, 10);
    if (m_bmpNormal.IsOk())
    {
        best.x = m_bmpNormal.GetWidth();
        best.y = m_bmpNormal.GetHeight();
        best.x += (2*m_marginX);
        best.y += (2*m_marginY);
    }
    CacheBestSize(best);
    return best;
}

wxSize wxMoBitmapButton::IPGetDefaultSize()
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

bool wxMoBitmapButton::SendClickEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    event.SetEventObject(this);

    return ProcessCommand(event);
}

bool wxMoBitmapButton::SetBackgroundColour(const wxColour &colour)
{
    if ( !wxControl::SetBackgroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();

    return true;
}

bool wxMoBitmapButton::SetForegroundColour(const wxColour &colour)
{
    if ( !wxControl::SetForegroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();

    return true;
}

bool wxMoBitmapButton::Enable(bool enable)
{
    wxControl::Enable(enable);
    Refresh();
    return true;
}

void wxMoBitmapButton::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif
    wxColour parentColour(GetParent()->GetBackgroundColour());
    dc.SetBrush(wxBrush(parentColour));
    dc.Clear();

    wxRect rect(wxPoint(0, 0), GetSize());
    int state = m_pressDetector.GetButtonState();
    if (!IsEnabled())
        state = wxCTRL_STATE_DISABLED;
    wxSize sz = GetSize();

    wxBitmap toDraw = m_bmpNormal;
    if (state & wxCTRL_STATE_DISABLED)
    {
        if (!m_bmpDisabled.IsOk())
            m_bmpDisabled = wxMoRenderer::MakeDisabledBitmap(toDraw);
        toDraw = m_bmpDisabled;
    }
    else if ((state & wxCTRL_STATE_HIGHLIGHTED) && m_bmpSelected.IsOk())
        toDraw = m_bmpSelected;

    //wxMoRenderer::DrawButtonBackground(this, dc, parentColour, GetBackgroundColour(), GetBorderColour(), rect, state, wxMO_BUTTON_TWO_TONE);

    if (toDraw.IsOk())
    {
        int x = (sz.x - toDraw.GetWidth())/2;
        int y = (sz.y - toDraw.GetHeight())/2;
        
        dc.DrawBitmap(toDraw, x, y, true);
    }
}

void wxMoBitmapButton::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}


void wxMoBitmapButton::OnMouseEvent(wxMouseEvent& event)
{
    if (!IsEnabled())
        return;

    wxRect rect(wxPoint(0, 0), GetRect().GetSize());
    if (m_pressDetector.ProcessMouseEvent(this, rect, event))
    {
        SendClickEvent();
    }
}

