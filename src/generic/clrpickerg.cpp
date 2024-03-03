///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/clrpickerg.cpp
// Purpose:     wxGenericColourButton class implementation
// Author:      Francesco Montorsi (readapted code written by Vadim Zeitlin)
// Created:     15/04/2006
// Copyright:   (c) Vadim Zeitlin, Francesco Montorsi
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_COLOURPICKERCTRL

#include "wx/clrpicker.h"
#include "wx/colordlg.h"
#include "wx/dcmemory.h"

namespace // anonymous namespace
{
const wxSize defaultBitmapSize(60, 13);
}

// ============================================================================
// implementation
// ============================================================================

wxColourData wxGenericColourButton::ms_data;
wxIMPLEMENT_DYNAMIC_CLASS(wxGenericColourButton, wxBitmapButton);

// ----------------------------------------------------------------------------
// wxGenericColourButton
// ----------------------------------------------------------------------------

bool wxGenericColourButton::Create( wxWindow *parent, wxWindowID id,
                        const wxColour &col, const wxPoint &pos,
                        const wxSize &size, long style,
                        const wxValidator& validator, const wxString &name)
{
    // create this button
    if (!wxBitmapButton::Create( parent, id, m_bitmap, pos,
                           size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxGenericColourButton creation failed") );
        return false;
    }

    // and handle user clicks on it
    Bind(wxEVT_BUTTON, &wxGenericColourButton::OnButtonClick, this, GetId());

    m_bitmap = wxBitmap(FromDIP(defaultBitmapSize));
    m_colour = col;
    UpdateColour();
    InitColourData();
    ms_data.SetChooseAlpha((style & wxCLRP_SHOW_ALPHA) != 0);

    Bind(wxEVT_DPI_CHANGED, &wxGenericColourButton::OnDPIChanged, this);

    return true;
}

void wxGenericColourButton::InitColourData()
{
    ms_data.SetChooseFull(true);
    unsigned char grey = 0;
    for (int i = 0; i < 16; i++, grey += 16)
    {
        // fill with grey tones the custom colors palette
        wxColour colour(grey, grey, grey);
        ms_data.SetCustomColour(i, colour);
    }
}

void wxGenericColourButton::OnButtonClick(wxCommandEvent& WXUNUSED(ev))
{
    // update the wxColouData to be shown in the dialog
    ms_data.SetColour(m_colour);

    // create the colour dialog and display it
    wxColourDialog dlg(this, &ms_data);
    dlg.Bind(wxEVT_COLOUR_CHANGED, &wxGenericColourButton::OnColourChanged, this);

    wxEventType eventType;
    if (dlg.ShowModal() == wxID_OK)
    {
        ms_data = dlg.GetColourData();
        SetColour(ms_data.GetColour());

        eventType = wxEVT_COLOURPICKER_CHANGED;
    }
    else
    {
        eventType = wxEVT_COLOURPICKER_DIALOG_CANCELLED;
    }

    // Fire the corresponding event: note that we want it to appear as
    // originating from our parent, which is the user-visible window, and not
    // this button itself, which is just an implementation detail.
    wxWindow* const parent = GetParent();
    wxColourPickerEvent event(parent, parent->GetId(), m_colour, eventType);

    ProcessWindowEvent(event);
}

void wxGenericColourButton::OnColourChanged(wxColourDialogEvent& ev)
{
    wxWindow* const parent = GetParent();
    wxColourPickerEvent event(parent, parent->GetId(), ev.GetColour(),
                              wxEVT_COLOURPICKER_CURRENT_CHANGED);
    parent->ProcessWindowEvent(event);
}

void wxGenericColourButton::OnDPIChanged(wxDPIChangedEvent& event)
{
    m_bitmap = wxBitmap(FromDIP(defaultBitmapSize));
    UpdateColour();

    event.Skip();
}

void wxGenericColourButton::UpdateColour()
{
    wxMemoryDC dc(m_bitmap);
    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.SetBrush( wxBrush(m_colour) );
    dc.DrawRectangle( 0,0,m_bitmap.GetWidth(),m_bitmap.GetHeight() );

    if ( HasFlag(wxCLRP_SHOW_LABEL) )
    {
        wxColour col( ~m_colour.Red(), ~m_colour.Green(), ~m_colour.Blue() );
        dc.SetTextForeground( col );
        dc.SetFont( GetFont() );

        const wxString text = m_colour.GetAsString(wxC2S_HTML_SYNTAX);
        const wxSize textSize = dc.GetTextExtent(text);
        const int x = (m_bitmap.GetWidth() - textSize.GetWidth()) / 2;
        const int y = (m_bitmap.GetHeight() - textSize.GetHeight()) / 2;
        dc.DrawText(text, x, y);
    }

    dc.SelectObject( wxNullBitmap );

    SetBitmapLabel( wxNullBitmap );
    SetBitmapLabel( m_bitmap );
}

wxSize wxGenericColourButton::DoGetBestSize() const
{
    wxSize sz(wxBitmapButton::DoGetBestSize());
#ifdef __WXMAC__
    sz.y += 6;
#else
    sz.y += 2;
#endif
    sz.x += 30;
    if ( HasFlag(wxCLRP_SHOW_LABEL) )
        return sz;

    // if we have no label, then make this button a square
    // (like e.g. native GTK version of this control) ???
    // sz.SetWidth(sz.GetHeight());
    return sz;
}

#endif      // wxUSE_COLOURPICKERCTRL
