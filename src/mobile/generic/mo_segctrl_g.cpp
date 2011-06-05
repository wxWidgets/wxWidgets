/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_segctrl_g.h
// Purpose:     wxMoSegmentedCtrl class
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

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
#endif

#include "wx/mobile/generic/segctrl.h"

#include "wx/imaglist.h"
#include "wx/dcbuffer.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoSegmentedCtrl, wxMoTabCtrl)

BEGIN_EVENT_TABLE(wxMoSegmentedCtrl, wxMoTabCtrl)
    EVT_PAINT(wxMoSegmentedCtrl::OnPaint)
END_EVENT_TABLE()

wxMoSegmentedCtrl::wxMoSegmentedCtrl()
{
    Init();
}

bool wxMoSegmentedCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
    wxMoTabCtrl::Create(parent, id, pos, size, style, name);

    SetBackgroundColour(GetParent()->GetBackgroundColour());
    GetButtonStrip().SetButtonBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_SEGCTRL_BG));
    GetButtonStrip().SetLabelColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_SEGCTRL_TEXT));
    GetButtonStrip().SetBackgroundColour(GetParent()->GetBackgroundColour());
    GetButtonStrip().SetButtonSelectionColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_SEGCTRL_SELECTED_BG));
    GetButtonStrip().SetBorderColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_SEGCTRL_BORDER));
    GetButtonStrip().SetInterButtonSpacing(-1);
    GetButtonStrip().SetButtonMarginX(2);

    GetButtonStrip().SetLabelFont(wxMoSystemSettings::GetFont(wxMO_FONT_NORMAL_BUTTON));

    InvalidateBestSize();
    SetMinSize(wxDefaultSize);
    SetInitialSize(size);
    
    return true;
}

void wxMoSegmentedCtrl::Init()
{
    m_buttonStrip.SetControlType(wxMoButtonStrip::SegmentedCtrl);
    m_buttonStrip.SetLayoutStyle(wxMoButtonStrip::LayoutEvenButtons);
    m_buttonStrip.SetSelectionStyle(wxMoButtonStrip::SingleSelection);    
}

// wxEVT_COMMAND_TAB_SEL_CHANGED;
// wxEVT_COMMAND_TAB_SEL_CHANGING;

bool wxMoSegmentedCtrl::SendCommand(wxEventType eventType, int selection)
{
    wxTabEvent event(eventType, m_windowId);

    event.SetEventObject( this );
    event.SetEventType(eventType);
    event.SetSelection(selection);

    if (ProcessEvent(event))
    {
        if (!event.IsAllowed())
            return false;
    }
    
    return true;
}

void wxMoSegmentedCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif

    wxRect rect(wxPoint(0, 0), GetSize());
    m_buttonStrip.Draw(this, rect, dc);
}

void wxMoSegmentedCtrl::OnInsertItem(wxMoBarButton* button)
{
    button->SetMarginX(4);
    button->SetStyle(wxBBU_BORDERED);
    button->SetFont(GetFont());
    button->SetBackgroundColour(m_buttonStrip.GetButtonBackgroundColour());
    button->SetSelectionColour(m_buttonStrip.GetButtonSelectionColour());
    button->SetBorderColour(m_buttonStrip.GetBorderColour());
    button->SetPosition(wxPoint(0, 0));
    button->SetSize(wxSize(-1, GetSize().y - 2*m_buttonStrip.GetEndMargin()));
}

wxSize wxMoSegmentedCtrl::DoGetBestSize() const
{
    return wxMoTabCtrl::DoGetBestSize();
}
