/////////////////////////////////////////////////////////////////////////////
// Name:        textdlgg.cpp
// Purpose:     wxTextEntryDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "textdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <stdio.h>

    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/intl.h"
#endif

#if wxUSE_STATLINE
    #include "wx/statline.h"
#endif

#include "wx/generic/textdlgg.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxNumberEntryDialog : public wxDialog
{
public:
    wxNumberEntryDialog(wxWindow *parent,
                        const wxString& message,
                        const wxString& prompt,
                        const wxString& caption,
                        long value, long min, long max,
                        const wxPoint& pos);

    long GetValue() const { return m_value; }

    // implementation only
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

protected:
    wxTextCtrl *m_spinctrl; // TODO replace it with wxSpinCtrl once it's done

    long m_value, m_min, m_max;

private:
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const int wxID_TEXT = 3000;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTextEntryDialog
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxNumberEntryDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxNumberEntryDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, wxNumberEntryDialog::OnCancel)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxTextEntryDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxTextEntryDialog::OnOK)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxTextEntryDialog, wxDialog)
#endif

wxTextEntryDialog::wxTextEntryDialog(wxWindow *parent,
                                     const wxString& message,
                                     const wxString& caption,
                                     const wxString& value,
                                     long style,
                                     const wxPoint& pos)
                 : wxDialog(parent, -1, caption, pos, wxDefaultSize,
                            wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL),
                   m_value(value)
{
    // calculate the sizes
    // -------------------

    wxArrayString lines;
    wxSize sizeText = SplitTextMessage(message, &lines);

    wxSize sizeBtn = GetStandardButtonSize();

    long wText = wxMax(4*sizeBtn.GetWidth(), sizeText.GetWidth());
    long hText = GetStandardTextHeight();

    long wDialog = 4*LAYOUT_X_MARGIN + wText;
    long hDialog = 2*LAYOUT_Y_MARGIN +
                   sizeText.GetHeight() * lines.GetCount() +
                   2*LAYOUT_Y_MARGIN +
                   hText +
                   2*LAYOUT_Y_MARGIN +
                   sizeBtn.GetHeight() +
                   2*LAYOUT_Y_MARGIN;

    // create the controls
    // -------------------

    // message
    long x = 2*LAYOUT_X_MARGIN;
    long y = CreateTextMessage(lines,
                               wxPoint(x, 2*LAYOUT_Y_MARGIN),
                               sizeText);

    y += 2*LAYOUT_X_MARGIN;

    // text ctrl
    m_textctrl = new wxTextCtrl(this, wxID_TEXT, m_value,
                                wxPoint(x, y),
                                wxSize(wText, hText));
    y += hText + 2*LAYOUT_X_MARGIN;

    // and buttons
    CreateStandardButtons(wDialog, y, sizeBtn.GetWidth(), sizeBtn.GetHeight());

    // set the dialog size and position
    SetClientSize(wDialog, hDialog);
    if ( pos == wxDefaultPosition )
    {
        // centre the dialog if no explicit position given
        Centre(wxBOTH | wxCENTER_FRAME);
    }

    m_textctrl->SetFocus();
}

void wxTextEntryDialog::OnOK(wxCommandEvent& WXUNUSED(event) )
{
    m_value = m_textctrl->GetValue();

    EndModal(wxID_OK);
}

// ----------------------------------------------------------------------------
// wxNumberEntryDialog
// ----------------------------------------------------------------------------

wxNumberEntryDialog::wxNumberEntryDialog(wxWindow *parent,
                                         const wxString& message,
                                         const wxString& prompt,
                                         const wxString& caption,
                                         long value,
                                         long min,
                                         long max,
                                         const wxPoint& pos)
                   : wxDialog(parent, -1, caption,
                              pos, wxDefaultSize,
                              wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{
    // init members
    // ------------

    m_value = value;
    m_max = max;
    m_min = min;

    // calculate the sizes
    // -------------------

    wxArrayString lines;
    wxSize sizeText = SplitTextMessage(message, &lines);

    wxSize sizeBtn = GetStandardButtonSize();

    int wPrompt, hPrompt;
    GetTextExtent(prompt, &wPrompt, &hPrompt);

    long wText = wxMax(2*sizeBtn.GetWidth(),
                       wxMax(wPrompt, sizeText.GetWidth()));
    long hText = GetStandardTextHeight();

    long wDialog = 5*LAYOUT_X_MARGIN + wText + wPrompt;
    long hDialog = 2*LAYOUT_Y_MARGIN +
                   sizeText.GetHeight() * lines.GetCount() +
                   2*LAYOUT_Y_MARGIN +
                   hText +
                   2*LAYOUT_Y_MARGIN +
                   sizeBtn.GetHeight() +
                   2*LAYOUT_Y_MARGIN;

    // create the controls
    // -------------------

    // message
    long x = 2*LAYOUT_X_MARGIN;
    long y = CreateTextMessage(lines,
                               wxPoint(x, 2*LAYOUT_Y_MARGIN),
                               sizeText);

    y += 2*LAYOUT_X_MARGIN;

    // prompt
    (void)new wxStaticText(this, -1, prompt,
                           wxPoint(x, y),
                           wxSize(wPrompt, hPrompt));

    // spin ctrl
    wxString valStr;
    valStr.Printf("%lu", m_value);
    m_spinctrl = new wxTextCtrl(this, -1, valStr,
                                wxPoint(x + wPrompt + LAYOUT_X_MARGIN, y),
                                wxSize(wText, hText));
    y += hText + 2*LAYOUT_X_MARGIN;

    // and buttons
    CreateStandardButtons(wDialog, y, sizeBtn.GetWidth(), sizeBtn.GetHeight());

    // set the dialog size and position
    SetClientSize(wDialog, hDialog);
    if ( pos == wxDefaultPosition )
    {
        // centre the dialog if no explicit position given
        Centre(wxBOTH | wxCENTER_FRAME);
    }

    m_spinctrl->SetFocus();
}

void wxNumberEntryDialog::OnOK(wxCommandEvent& event)
{
    if ( (sscanf(m_spinctrl->GetValue(), "%lu", &m_value) != 1) ||
         (m_value < m_min) || (m_value > m_max) )
    {
        // not a number or out of range
        m_value = -1;
    }

    EndModal(wxID_OK);
}

void wxNumberEntryDialog::OnCancel(wxCommandEvent& event)
{
    m_value = -1;

    EndModal(wxID_CANCEL);
}

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

// wxGetTextFromUser is in utilscmn.cpp

long wxGetNumberFromUser(const wxString& msg,
                         const wxString& prompt,
                         const wxString& title,
                         long value,
                         long min,
                         long max,
                         wxWindow *parent,
                         const wxPoint& pos)
{
    wxNumberEntryDialog dialog(parent, msg, prompt, title,
                               value, min, max, pos);
    (void)dialog.ShowModal();

    return dialog.GetValue();
}
