/////////////////////////////////////////////////////////////////////////////
// Name:        textdlgg.cpp
// Purpose:     wxTextEntryDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "textdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TEXTDLG

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/intl.h"
    #include "wx/sizer.h"
#endif

#if wxUSE_STATLINE
    #include "wx/statline.h"
#endif

#include "wx/generic/textdlgg.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const int wxID_TEXT = 3000;

// ---------------------------------------------------------------------------
// macros
// ---------------------------------------------------------------------------

/* Macro for avoiding #ifdefs when value have to be different depending on size of
   device we display on - take it from something like wxDesktopPolicy in the future
 */

#if defined(__SMARTPHONE__)
    #define wxLARGESMALL(large,small) small
#else
    #define wxLARGESMALL(large,small) large
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTextEntryDialog
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxTextEntryDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxTextEntryDialog::OnOK)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxTextEntryDialog, wxDialog)

wxTextEntryDialog::wxTextEntryDialog(wxWindow *parent,
                                     const wxString& message,
                                     const wxString& caption,
                                     const wxString& value,
                                     long style,
                                     const wxPoint& pos)
                 : wxDialog(parent, wxID_ANY, caption, pos, wxDefaultSize,
                            wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL),
                   m_value(value)
{
    m_dialogStyle = style;
    m_value = value;

    wxBeginBusyCursor();

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    // 1) text message
    topsizer->Add( CreateTextSizer( message ), 0, wxALL, wxLARGESMALL(10,0) );

    // 2) text ctrl
    m_textctrl = new wxTextCtrl(this, wxID_TEXT, value,
                                wxDefaultPosition, wxSize(300, wxDefaultCoord),
                                style & ~wxTextEntryDialogStyle);
    topsizer->Add( m_textctrl, 1, wxEXPAND | wxLEFT|wxRIGHT, wxLARGESMALL(15,0) );

#if wxUSE_VALIDATORS
    wxTextValidator validator( wxFILTER_NONE, &m_value );
    m_textctrl->SetValidator( validator );
#endif
  // wxUSE_VALIDATORS

    // smart phones does not support or do not waste space for wxButtons
#ifdef __SMARTPHONE__

    SetRightMenu(wxID_CANCEL, _("Cancel"));

#else // __SMARTPHONE__/!__SMARTPHONE__

#if wxUSE_STATLINE
    // 3) static line
    topsizer->Add( new wxStaticLine( this, wxID_ANY ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10 );
#endif

    // 4) buttons
    topsizer->Add( CreateButtonSizer( style ), 0, wxCENTRE | wxALL, 10 );

#endif // !__SMARTPHONE__

    SetAutoLayout( true );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    if ( ( style & wxCENTRE ) == wxCENTRE )
        Centre( wxBOTH );

    m_textctrl->SetSelection(-1, -1);
    m_textctrl->SetFocus();

    wxEndBusyCursor();
}

void wxTextEntryDialog::OnOK(wxCommandEvent& WXUNUSED(event) )
{
#if wxUSE_VALIDATORS
    if( Validate() && TransferDataFromWindow() )
    {
        EndModal( wxID_OK );
    }
#else
    m_value = m_textctrl->GetValue();

    EndModal(wxID_OK);
#endif
  // wxUSE_VALIDATORS
}

void wxTextEntryDialog::SetValue(const wxString& val)
{
    m_value = val;

    m_textctrl->SetValue(val);
}

#if wxUSE_VALIDATORS
void wxTextEntryDialog::SetTextValidator( long style )
{
    wxTextValidator validator( style, &m_value );
    m_textctrl->SetValidator( validator );
}

void wxTextEntryDialog::SetTextValidator( wxTextValidator& validator )
{
    m_textctrl->SetValidator( validator );
}

#endif
  // wxUSE_VALIDATORS

#endif // wxUSE_TEXTDLG
