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

// alignment radiobox indices
enum
{
    Align_Left,
    Align_Centre,
    Align_Right,
    Align_Max
};

// ----------------------------------------------------------------------------
// CheckBoxWidgetsPage
// ----------------------------------------------------------------------------

class HyperlinkWidgetsPage : public WidgetsPage
{
public:
    HyperlinkWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~HyperlinkWidgetsPage() {}

    virtual wxControl *GetWidget() const { return m_hyperlink; }
    virtual void RecreateWidget() { CreateHyperlink(); }

    // lazy creation of the content
    virtual void CreateContent();

protected:
    // event handlers
    void OnButtonSetLabel(wxCommandEvent& event);
    void OnButtonSetURL(wxCommandEvent& event);

    void OnButtonReset(wxCommandEvent& event);
    void OnAlignment(wxCommandEvent& event);

    // reset the control parameters
    void Reset();

    // (re)create the hyperctrl
    void CreateHyperlink();
    void CreateHyperlinkLong(long);

    // the controls
    // ------------

    // the checkbox itself and the sizer it is in
    wxHyperlinkCtrl *m_hyperlink;
    wxHyperlinkCtrl *m_hyperlinkLong;

    wxTextCtrl *m_label;
    wxTextCtrl *m_url;

    wxStaticText *m_visit;
    wxStaticText *m_fun;

    // the text entries for command parameters
    wxTextCtrl *m_textLabel;

    wxRadioBox *m_radioAlignMode;

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

    EVT_RADIOBOX(wxID_ANY, HyperlinkWidgetsPage::OnAlignment)
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
}

void HyperlinkWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, wxT("Hyperlink details"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerLeft->Add( CreateSizerWithTextAndButton( HyperlinkPage_SetLabel , wxT("Set &Label"), wxID_ANY, &m_label ),
                    0, wxALL | wxALIGN_RIGHT , 5 );

    sizerLeft->Add( CreateSizerWithTextAndButton( HyperlinkPage_SetURL , wxT("Set &URL"), wxID_ANY, &m_url ),
                    0, wxALL | wxALIGN_RIGHT , 5 );

    static const wxString alignments[] =
    {
        _T("&left"),
        _T("&centre"),
        _T("&right")
    };
    wxCOMPILE_TIME_ASSERT( WXSIZEOF(alignments) == Align_Max,
                           AlignMismatch );

    m_radioAlignMode = new wxRadioBox(this, wxID_ANY, _T("alignment"),
                                      wxDefaultPosition, wxDefaultSize,
                                      WXSIZEOF(alignments), alignments);
    m_radioAlignMode->SetSelection(1);  // start with "centre" selected since
                                        // wxHL_DEFAULT_STYLE contains wxHL_ALIGN_CENTRE
    sizerLeft->Add(m_radioAlignMode, 0, wxALL|wxGROW, 5);



    // right pane
    wxSizer *szHyperlinkLong = new wxBoxSizer(wxVERTICAL);
    wxSizer *szHyperlink = new wxBoxSizer(wxHORIZONTAL);

    m_visit = new wxStaticText(this, wxID_ANY, wxT("Visit "));

    m_hyperlink = new wxHyperlinkCtrl(this,
                                      HyperlinkPage_Ctrl,
                                      wxT("wxWidgets website"),
                                      wxT("www.wxwidgets.org"));

    m_fun = new wxStaticText(this, wxID_ANY, wxT(" for fun!"));

    szHyperlink->Add(0, 0, 1, wxCENTRE);
    szHyperlink->Add(m_visit, 0, wxCENTRE);
    szHyperlink->Add(m_hyperlink, 0, wxCENTRE);
    szHyperlink->Add(m_fun, 0, wxCENTRE);
    szHyperlink->Add(0, 0, 1, wxCENTRE);
    szHyperlink->SetMinSize(150, 0);

    m_hyperlinkLong = new wxHyperlinkCtrl(this,
                                          wxID_ANY,
                                          wxT("This is a long hyperlink"),
                                          wxT("www.wxwidgets.org"));

    szHyperlinkLong->Add(0, 0, 1, wxCENTRE);
    szHyperlinkLong->Add(szHyperlink, 0, wxCENTRE|wxGROW);
    szHyperlinkLong->Add(0, 0, 1, wxCENTRE);
    szHyperlinkLong->Add(m_hyperlinkLong, 0, wxGROW);
    szHyperlinkLong->Add(0, 0, 1, wxCENTRE);


    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, (wxALL & ~wxLEFT), 10);
    sizerTop->Add(szHyperlinkLong, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetSizer(sizerTop);
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

    wxHyperlinkCtrl *hyp = new wxHyperlinkCtrl(this,
                                               HyperlinkPage_Ctrl,
                                               label,
                                               url);

    // update sizer's child window
    GetSizer()->Replace(m_hyperlink, hyp, true);

    // update our pointer
    delete m_hyperlink;
    m_hyperlink = hyp;

    // relayout the sizer
    GetSizer()->Layout();
}

void HyperlinkWidgetsPage::CreateHyperlinkLong(long style)
{
    style = (wxHL_DEFAULT_STYLE & ~wxHL_ALIGN_CENTRE)|style;
    wxHyperlinkCtrl *hyp = new wxHyperlinkCtrl(this,
                                               wxID_ANY,
                                               wxT("This is a long hyperlink"),
                                               wxT("www.wxwidgets.org"),
                                               wxDefaultPosition,
                                               wxDefaultSize,
                                               style);

    // update sizer's child window
    GetSizer()->Replace(m_hyperlinkLong, hyp, true);

    // update our pointer
    delete m_hyperlinkLong;
    m_hyperlinkLong = hyp;

    // relayout the sizer
    GetSizer()->Layout();
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

void HyperlinkWidgetsPage::OnAlignment(wxCommandEvent& WXUNUSED(event))
{
    long addstyle;
    switch ( m_radioAlignMode->GetSelection() )
    {
        default:
        case Align_Max:
            wxFAIL_MSG( _T("unknown alignment") );
            // fall through

        case Align_Left:
            addstyle = wxHL_ALIGN_LEFT;
            break;

        case Align_Centre:
            addstyle = wxHL_ALIGN_CENTRE;
            break;

        case Align_Right:
            addstyle = wxHL_ALIGN_RIGHT;
            break;
    }

    CreateHyperlinkLong(addstyle);
}

#endif // wxUSE_HYPERLINKCTRL
