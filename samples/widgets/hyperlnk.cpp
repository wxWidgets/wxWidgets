/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        hyperlnk.cpp
// Purpose:     Part of the widgets sample showing wxHyperlinkCtrl
// Author:      Dimitri Schoolwerth, Vadim Zeitlin
// Created:     27 Sep 2003
// Id:          $Id$
// Copyright:   (c) 2003 wxWindows team
// License:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_HYPERLINKCTRL

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"

    #include "wx/sizer.h"
#endif

#include "wx/hyperlink.h"

#include "widgets.h"

#include "icons/hyperlnk.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    HyperlinkPage_Reset = wxID_HIGHEST,
    HyperlinkPage_SetLabel,
    HyperlinkPage_SetURL,
    HyperlinkPage_Ctrl
};

// ----------------------------------------------------------------------------
// CheckBoxWidgetsPage
// ----------------------------------------------------------------------------

class HyperlinkWidgetsPage : public WidgetsPage
{
public:
    HyperlinkWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~HyperlinkWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_hyperlink; }
    virtual void RecreateWidget() { CreateHyperlink(); }

protected:
    // event handlers
    void OnButtonSetLabel(wxCommandEvent& event);
    void OnButtonSetURL(wxCommandEvent& event);

    void OnButtonReset(wxCommandEvent& event);

    // reset the control parameters
    void Reset();

    // (re)create the hyperctrl
    void CreateHyperlink();

    // the controls
    // ------------

    // the checkbox itself and the sizer it is in
    wxHyperlinkCtrl *m_hyperlink;
    wxSizer *m_sizerHyperlink;

    wxTextCtrl *m_label;
    wxTextCtrl *m_url;

    wxStaticText *m_visit;
    wxStaticText *m_fun;

    // the text entries for command parameters
    wxTextCtrl *m_textLabel;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(HyperlinkWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(HyperlinkWidgetsPage, WidgetsPage)
    EVT_BUTTON(HyperlinkPage_Reset, HyperlinkWidgetsPage::OnButtonReset)
    EVT_BUTTON(HyperlinkPage_SetLabel, HyperlinkWidgetsPage::OnButtonSetLabel)
    EVT_BUTTON(HyperlinkPage_SetURL, HyperlinkWidgetsPage::OnButtonSetURL)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(HyperlinkWidgetsPage, wxT("Hyperlink"),
                       GENERIC_CTRLS
                       );

HyperlinkWidgetsPage::HyperlinkWidgetsPage(WidgetsBookCtrl *book,
                                           wxImageList *imaglist)
                      :WidgetsPage(book, imaglist, hyperlnk_xpm)
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, wxT("Hyperlink details"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerLeft->Add( CreateSizerWithTextAndButton( HyperlinkPage_SetLabel , wxT("Set &Label"), wxID_ANY, &m_label ),
                    0, wxALL | wxALIGN_RIGHT , 5 );

    sizerLeft->Add( CreateSizerWithTextAndButton( HyperlinkPage_SetURL , wxT("Set &URL"), wxID_ANY, &m_url ),
                    0, wxALL | wxALIGN_RIGHT , 5 );

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);

    m_visit = new wxStaticText(this, wxID_ANY, wxT("Visit "));

    m_hyperlink = new wxHyperlinkCtrl(this,
                                      HyperlinkPage_Ctrl,
                                      wxT("wxWidgets website"),
                                      wxT("www.wxwidgets.org"));

    m_fun = new wxStaticText(this, wxID_ANY, wxT(" for fun!"));

    sizerRight->Add(0, 0, 1, wxCENTRE);
    sizerRight->Add(m_visit, 0, wxCENTRE);
    sizerRight->Add(m_hyperlink, 0, wxCENTRE);
    sizerRight->Add(m_fun, 0, wxCENTRE);
    sizerRight->Add(0, 0, 1, wxCENTRE);
    sizerRight->SetMinSize(150, 0);
    m_sizerHyperlink = sizerRight; // save it to modify it later

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetSizer(sizerTop);

    sizerTop->Fit(this);
}

void HyperlinkWidgetsPage::Reset()
{
    m_label->SetValue(m_hyperlink->GetLabel());
    m_url->SetValue(m_hyperlink->GetURL());
}

void HyperlinkWidgetsPage::CreateHyperlink()
{
    const wxString label = m_hyperlink->GetLabel();
    const wxString url = m_hyperlink->GetURL();

    size_t count = m_sizerHyperlink->GetChildren().GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_sizerHyperlink->Remove(0);
    }

    delete m_hyperlink;

    m_hyperlink = new wxHyperlinkCtrl(this,
                                      HyperlinkPage_Ctrl,
                                      label,
                                      url);

    m_sizerHyperlink->Add(0, 0, 1, wxCENTRE);
    m_sizerHyperlink->Add(m_visit, 0, wxCENTRE);
    m_sizerHyperlink->Add(m_hyperlink, 0, wxCENTRE);
    m_sizerHyperlink->Add(m_fun, 0, wxCENTRE);
    m_sizerHyperlink->Add(0, 0, 1, wxCENTRE);
    m_sizerHyperlink->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void HyperlinkWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateHyperlink();
}

void HyperlinkWidgetsPage::OnButtonSetLabel(wxCommandEvent& WXUNUSED(event))
{
    m_hyperlink->SetLabel(m_label->GetValue());
    CreateHyperlink();
}

void HyperlinkWidgetsPage::OnButtonSetURL(wxCommandEvent& WXUNUSED(event))
{
    m_hyperlink->SetURL(m_url->GetValue());
    CreateHyperlink();
}

#endif // wxUSE_HYPERLINKCTRL
