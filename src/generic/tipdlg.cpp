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
    #include "wx/statbmp.h"
    #include "wx/dialog.h"
    #include "wx/icon.h"
    #include "wx/intl.h"
    #include "wx/layout.h"
    #include "wx/settings.h"
    #include "wx/textctrl.h"
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
    EVT_BUTTON(wxID_NEXT_TIP, OnNextTip)
END_EVENT_TABLE()

wxTipDialog::wxTipDialog(wxWindow *parent,
                         wxTipProvider *tipProvider,
                         bool showAtStartup)
           : wxDialog(parent, -1, _("Tip of the Day"),
                      wxDefaultPosition, wxDefaultSize,
                      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    m_tipProvider = tipProvider;

    wxSize sizeBtn = GetStandardButtonSize();
    wxLayoutConstraints *c;

    // create the controls in the right order, then set the constraints
    wxButton *btnClose = new wxButton(this, wxID_CANCEL, _("&Close"));
    m_checkbox = new wxCheckBox(this, -1, _("&Show tips at startup"));
    wxButton *btnNext = new wxButton(this, wxID_NEXT_TIP, _("&Next"));

    wxTextCtrl *text = new wxTextCtrl(this, -1, _("Did you know..."),
                                      wxDefaultPosition, wxDefaultSize,
                                      wxTE_READONLY | wxNO_BORDER);
    text->SetFont(wxFont(18, wxSWISS, wxNORMAL, wxBOLD));
    text->SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_BTNFACE));

    m_text = new wxTextCtrl(this, -1, _T(""),
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_READONLY | wxSUNKEN_BORDER);
    m_text->SetFont(wxFont(14, wxROMAN, wxNORMAL, wxNORMAL));

#ifdef __WXMSW__
    wxIcon icon("wxICON_TIP");
#else
    #include "wx/generic/tip.xpm"
    wxIcon icon(tipIcon);
#endif

    wxStaticBitmap *bmp = new wxStaticBitmap(this, -1, icon);

    const int iconSize = icon.GetWidth();

    c = new wxLayoutConstraints;
    c->top.SameAs(this, wxTop, 2*LAYOUT_Y_MARGIN);
    c->left.RightOf(bmp, 2*LAYOUT_X_MARGIN);
    c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
    c->height.Absolute(2*text->GetSize().GetHeight());
    text->SetConstraints(c);

    c = new wxLayoutConstraints;
    c->centreY.SameAs(text, wxCentreY);
    c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
    c->width.Absolute(iconSize);
    c->height.Absolute(iconSize);
    bmp->SetConstraints(c);

    c = new wxLayoutConstraints;
    c->bottom.SameAs(this, wxBottom, 2*LAYOUT_X_MARGIN);
    c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
    c->width.Absolute(sizeBtn.GetWidth());
    c->height.Absolute(sizeBtn.GetHeight());
    btnClose->SetConstraints(c);

    c = new wxLayoutConstraints;
    c->bottom.SameAs(this, wxBottom, 2*LAYOUT_X_MARGIN);
    c->right.LeftOf(btnClose, 2*LAYOUT_X_MARGIN);
    c->width.Absolute(sizeBtn.GetWidth());
    c->height.Absolute(sizeBtn.GetHeight());
    btnNext->SetConstraints(c);

    c = new wxLayoutConstraints;
    c->bottom.SameAs(this, wxBottom, 2*LAYOUT_X_MARGIN);
    c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
    c->width.AsIs();
    c->height.AsIs();
    m_checkbox->SetConstraints(c);
    m_checkbox->SetValue(showAtStartup);

    c = new wxLayoutConstraints;
    c->top.Below(text);
    c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
    c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
    c->bottom.Above(btnClose, -2*LAYOUT_Y_MARGIN);
    m_text->SetConstraints(c);

    SetTipText();

    Centre(wxBOTH | wxCENTER_FRAME);

    wxSize size(5*sizeBtn.GetWidth(), 10*sizeBtn.GetHeight());
    SetSize(size);
    SetSizeHints(size.x, size.y);

    SetAutoLayout(TRUE);
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

