///////////////////////////////////////////////////////////////////////////////
// Name:        tipdlg.cpp
// Purpose:     implementation of wxTipDialog
// Author:      Vadim Zeitlin
// Modified by:
// Created:     28.06.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "tipdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STARTUP_TIPS

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/statbox.h"
    #include "wx/dialog.h"
    #include "wx/icon.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/textctrl.h"
    #include "wx/statbmp.h"
    #include "wx/stattext.h"
    #include "wx/sizer.h"
#endif // WX_PRECOMP

#include "wx/statline.h"

#include "wx/tipdlg.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const int wxID_NEXT_TIP = -100;  // whatever

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// an implementation which takes the tips from the text file - each line
// represents a tip
class WXDLLEXPORT wxFileTipProvider : public wxTipProvider
{
public:
    wxFileTipProvider(const wxString& filename, size_t currentTip);

    virtual wxString GetTip();

private:
    wxTextFile m_textfile;
};

#ifdef __WIN32__
// TODO an implementation which takes the tips from the given registry key
class WXDLLEXPORT wxRegTipProvider : public wxTipProvider
{
public:
    wxRegTipProvider(const wxString& keyname);

    virtual wxString GetTip();
};

// Empty implementation for now to keep the linker happy
wxString wxRegTipProvider::GetTip()
{
    return "";
}

#endif // __WIN32__

// the dialog we show in wxShowTip()
class WXDLLEXPORT wxTipDialog : public wxDialog
{
public:
    wxTipDialog(wxWindow *parent,
                wxTipProvider *tipProvider,
                bool showAtStartup);

    // the tip dialog has "Show tips on startup" checkbox - return TRUE if it
    // was checked (or wasn't unchecked)
    bool ShowTipsOnStartup() const { return m_checkbox->GetValue(); }

    // sets the (next) tip text
    void SetTipText() { m_text->SetValue(m_tipProvider->GetTip()); }

    // "Next" button handler
    void OnNextTip(wxCommandEvent& WXUNUSED(event)) { SetTipText(); }

private:
    wxTipProvider *m_tipProvider;

    wxTextCtrl *m_text;
    wxCheckBox *m_checkbox;

    DECLARE_EVENT_TABLE()
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFileTipProvider
// ----------------------------------------------------------------------------

wxFileTipProvider::wxFileTipProvider(const wxString& filename,
                                     size_t currentTip)
                 : wxTipProvider(currentTip), m_textfile(filename)
{
    m_textfile.Open();
}

wxString wxFileTipProvider::GetTip()
{
    size_t count = m_textfile.GetLineCount();
    if ( !count )
        return _("Tips not available, sorry!");

    // notice that it may be greater, actually, if we remembered it from the
    // last time and the number of tips changed
    if ( m_currentTip == count )
    {
        // wrap
        m_currentTip = 0;
    }

    return m_textfile.GetLine(m_currentTip++);
}

// ----------------------------------------------------------------------------
// wxTipDialog
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxTipDialog, wxDialog)
    EVT_BUTTON(wxID_NEXT_TIP, wxTipDialog::OnNextTip)
END_EVENT_TABLE()

wxTipDialog::wxTipDialog(wxWindow *parent,
                         wxTipProvider *tipProvider,
                         bool showAtStartup)
           : wxDialog(parent, -1, _("Tip of the Day"),
                      wxDefaultPosition, wxDefaultSize,
                      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    m_tipProvider = tipProvider;

    // 1) create all controls in tab order
    
    wxButton *btnClose = new wxButton(this, wxID_CANCEL, _("&Close"));
    
    m_checkbox = new wxCheckBox(this, -1, _("&Show tips at startup"));
    m_checkbox->SetValue(showAtStartup);
    
    wxButton *btnNext = new wxButton(this, wxID_NEXT_TIP, _("&Next"));

    wxStaticText *text = new wxStaticText(this, -1, _("Did you know..."), wxDefaultPosition, wxSize(-1,25) );
    text->SetFont(wxFont(18, wxSWISS, wxNORMAL, wxBOLD));
//
//    text->SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_BTNFACE));

    m_text = new wxTextCtrl(this, -1, _T(""),
                            wxDefaultPosition, wxSize(200, 160),
                            wxTE_MULTILINE | wxTE_READONLY | wxSUNKEN_BORDER);
    m_text->SetFont(wxFont(14, wxROMAN, wxNORMAL, wxNORMAL));

#if defined(__WXMSW__) || defined(__WXPM__)
    wxIcon icon("wxICON_TIP");
#else
    #include "wx/generic/tip.xpm"
    wxIcon icon(tipIcon);
#endif
    wxStaticBitmap *bmp = new wxStaticBitmap(this, -1, icon);

    // 2) put them in boxes

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    
    wxBoxSizer *icon_text = new wxBoxSizer( wxHORIZONTAL );
    icon_text->Add( bmp, 0, wxCENTER );
    icon_text->Add( text, 1, wxCENTER | wxLEFT, 20 );
    topsizer->Add( icon_text, 0, wxEXPAND | wxALL, 10 );
    
    topsizer->Add( m_text, 1, wxEXPAND | wxLEFT|wxRIGHT, 10 );

    wxBoxSizer *bottom = new wxBoxSizer( wxHORIZONTAL );
    bottom->Add( m_checkbox, 0, wxCENTER );
    bottom->Add( 10,10,1 );
    bottom->Add( btnNext, 0, wxCENTER | wxLEFT, 10 );
    bottom->Add( btnClose, 0, wxCENTER | wxLEFT, 10 );
    topsizer->Add( bottom, 0, wxEXPAND | wxALL, 10 );

    SetTipText();
    
    SetAutoLayout(TRUE);
    SetSizer( topsizer );
    
    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    Centre(wxBOTH | wxCENTER_FRAME);

}

// ----------------------------------------------------------------------------
// our public interface
// ----------------------------------------------------------------------------

wxTipProvider *wxCreateFileTipProvider(const wxString& filename,
                                       size_t currentTip)
{
    return new wxFileTipProvider(filename, currentTip);
}

bool wxShowTip(wxWindow *parent,
               wxTipProvider *tipProvider,
               bool showAtStartup)
{
    wxTipDialog dlg(parent, tipProvider, showAtStartup);
    dlg.ShowModal();

    return dlg.ShowTipsOnStartup();
}

#endif // wxUSE_STARTUP_TIPS

