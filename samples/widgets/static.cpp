/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        static.cpp
// Purpose:     Part of the widgets sample showing various static controls
// Author:      Vadim Zeitlin
// Created:     11.04.01
// Id:          $Id$
// Copyright:   (c) 2001 Vadim Zeitlin
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

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
#endif

#include "wx/sizer.h"

#include "wx/statline.h"

#include "widgets.h"
#include "icons/statbox.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    StaticPage_Reset = wxID_HIGHEST,
    StaticPage_BoxText,
    StaticPage_LabelText
};

// alignment radiobox values
enum
{
    StaticHAlign_Left,
    StaticHAlign_Centre,
    StaticHAlign_Right,
    StaticHAlign_Max
};

enum
{
    StaticVAlign_Top,
    StaticVAlign_Centre,
    StaticVAlign_Bottom,
    StaticVAlign_Max
};

// ----------------------------------------------------------------------------
// MyStaticText and MyStaticBox
// ----------------------------------------------------------------------------

// these 2 classes simply show that the static controls can get the mouse
// clicks too -- this used to be broken under MSW but works now

class MyStaticText : public wxStaticText
{
public:
    MyStaticText(wxWindow* parent,
                      wxWindowID id,
                      const wxString& label,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0)
        : wxStaticText(parent, id, label, pos, size, style)
    {
    }

protected:
    void OnMouseEvent(wxMouseEvent& WXUNUSED(event))
    {
        wxLogMessage(wxT("Clicked on static text"));
    }

    DECLARE_EVENT_TABLE()
};

class MyStaticBox : public wxStaticBox
{
public:
    MyStaticBox(wxWindow* parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0)
        : wxStaticBox(parent, id, label, pos, size, style)
    {
    }

protected:
    void OnMouseEvent(wxMouseEvent& WXUNUSED(event))
    {
        wxLogMessage(wxT("Clicked on static box"));
    }

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyStaticText, wxStaticText)
    EVT_LEFT_UP(MyStaticText::OnMouseEvent)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyStaticBox, wxStaticBox)
    EVT_LEFT_UP(MyStaticBox::OnMouseEvent)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// StaticWidgetsPage
// ----------------------------------------------------------------------------

class StaticWidgetsPage : public WidgetsPage
{
public:
    StaticWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~StaticWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_statText; }
    virtual void RecreateWidget() { CreateStatic(); }

    // lazy creation of the content
    virtual void CreateContent();

protected:
    // event handlers
    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnButtonReset(wxCommandEvent& event);
    void OnButtonBoxText(wxCommandEvent& event);
    void OnButtonLabelText(wxCommandEvent& event);

    // reset all parameters
    void Reset();

    // (re)create all controls
    void CreateStatic();

    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkVert,
               *m_chkAutoResize;

    wxRadioBox *m_radioHAlign,
               *m_radioVAlign;

    // the controls and the sizer containing them
    wxStaticBox *m_staticBox;
    wxStaticBoxSizer *m_sizerStatBox;
    wxStaticText *m_statText;
#if wxUSE_STATLINE
    wxStaticLine *m_statLine;
#endif // wxUSE_STATLINE
    wxSizer *m_sizerStatic;

    // the text entries for command parameters
    wxTextCtrl *m_textBox,
               *m_textLabel;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(StaticWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(StaticWidgetsPage, WidgetsPage)
    EVT_BUTTON(StaticPage_Reset, StaticWidgetsPage::OnButtonReset)
    EVT_BUTTON(StaticPage_LabelText, StaticWidgetsPage::OnButtonLabelText)
    EVT_BUTTON(StaticPage_BoxText, StaticWidgetsPage::OnButtonBoxText)

    EVT_CHECKBOX(wxID_ANY, StaticWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, StaticWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(StaticWidgetsPage, _T("Static"),
                       (int)wxPlatform(GENERIC_CTRLS).If(wxOS_WINDOWS,NATIVE_CTRLS)
                       );

StaticWidgetsPage::StaticWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, statbox_xpm)
{
    // init everything
    m_chkVert =
    m_chkAutoResize = (wxCheckBox *)NULL;

    m_radioHAlign =
    m_radioVAlign = (wxRadioBox *)NULL;

#if wxUSE_STATLINE
    m_statLine = (wxStaticLine *)NULL;
#endif // wxUSE_STATLINE
    m_statText = (wxStaticText *)NULL;

    m_staticBox = (wxStaticBox *)NULL;
    m_sizerStatBox = (wxStaticBoxSizer *)NULL;
    m_sizerStatic = (wxSizer *)NULL;
}

void StaticWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, _T("&Set style"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkVert = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Vertical line"));
    m_chkAutoResize = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Fit to text"));
    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    static const wxString halign[] =
    {
        _T("left"),
        _T("centre"),
        _T("right"),
    };

    static const wxString valign[] =
    {
        _T("top"),
        _T("centre"),
        _T("bottom"),
    };

    m_radioHAlign = new wxRadioBox(this, wxID_ANY, _T("&Horz alignment"),
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(halign), halign);
    m_radioVAlign = new wxRadioBox(this, wxID_ANY, _T("&Vert alignment"),
                                   wxDefaultPosition, wxDefaultSize,
                                   WXSIZEOF(valign), valign);

    sizerLeft->Add(m_radioHAlign, 0, wxGROW | wxALL, 5);
    sizerLeft->Add(m_radioVAlign, 0, wxGROW | wxALL, 5);

    wxButton *btn = new wxButton(this, StaticPage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, _T("&Change labels"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxSizer *sizerRow;

    sizerRow = CreateSizerWithTextAndButton(StaticPage_BoxText,
                                            _T("Change &box label"),
                                            wxID_ANY, &m_textBox);
    sizerMiddle->Add(sizerRow, 0, wxGROW | wxALL, 5);

    sizerRow = CreateSizerWithTextAndButton(StaticPage_LabelText,
                                            _T("Change &text label"),
                                            wxID_ANY, &m_textLabel);
    sizerMiddle->Add(sizerRow, 0, wxGROW | wxALL, 5);

    m_textBox->SetValue(_T("This is a box"));
    m_textLabel->SetValue(_T("And this is a label\ninside the box"));

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);
    sizerRight->SetMinSize(150, 0);
    m_sizerStatic = sizerRight;

    CreateStatic();

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    Reset();

    SetSizer(sizerTop);

    sizerTop->Fit(this);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void StaticWidgetsPage::Reset()
{
    m_chkVert->SetValue(false);
    m_chkAutoResize->SetValue(true);

    m_radioHAlign->SetSelection(StaticHAlign_Left);
    m_radioVAlign->SetSelection(StaticVAlign_Top);
}

void StaticWidgetsPage::CreateStatic()
{
    bool isVert = m_chkVert->GetValue();

    if ( m_sizerStatBox )
    {
        delete m_staticBox;
        // delete m_sizerStatBox; -- deleted by Remove()
        m_sizerStatic->Remove(m_sizerStatBox);
        delete m_statText;
#if wxUSE_STATLINE
        delete m_statLine;
#endif // wxUSE_STATLINE
    }

    int flagsBox = 0,
        flagsText = ms_defaultFlags;

    if ( !m_chkAutoResize->GetValue() )
    {
        flagsText |= wxST_NO_AUTORESIZE;
    }

    int align = 0;
    switch ( m_radioHAlign->GetSelection() )
    {
        default:
            wxFAIL_MSG(_T("unexpected radiobox selection"));
            // fall through

        case StaticHAlign_Left:
            align |= wxALIGN_LEFT;
            break;

        case StaticHAlign_Centre:
            align |= wxALIGN_CENTRE_HORIZONTAL;
            break;

        case StaticHAlign_Right:
            align |= wxALIGN_RIGHT;
            break;
    }

    switch ( m_radioVAlign->GetSelection() )
    {
        default:
            wxFAIL_MSG(_T("unexpected radiobox selection"));
            // fall through

        case StaticVAlign_Top:
            align |= wxALIGN_TOP;
            break;

        case StaticVAlign_Centre:
            align |= wxALIGN_CENTRE_VERTICAL;
            break;

        case StaticVAlign_Bottom:
            align |= wxALIGN_BOTTOM;
            break;
    }

    flagsText |= align;
    flagsBox |= align;

    m_staticBox = new MyStaticBox(this, wxID_ANY, m_textBox->GetValue(),
                                  wxDefaultPosition, wxDefaultSize,
                                  flagsBox);
    m_sizerStatBox = new wxStaticBoxSizer(m_staticBox, isVert ? wxHORIZONTAL
                                                              : wxVERTICAL);

    m_statText = new MyStaticText(this, wxID_ANY, m_textLabel->GetValue(),
                                  wxDefaultPosition, wxDefaultSize,
                                  flagsText);

#if wxUSE_STATLINE
    m_statLine = new wxStaticLine(this, wxID_ANY,
                                  wxDefaultPosition, wxDefaultSize,
                                  isVert ? wxLI_VERTICAL : wxLI_HORIZONTAL);
#endif // wxUSE_STATLINE

    m_sizerStatBox->Add(m_statText, 1, wxGROW | wxALL, 5);
#if wxUSE_STATLINE
    m_sizerStatBox->Add(m_statLine, 0, wxGROW | wxALL, 5);
#endif // wxUSE_STATLINE
    m_sizerStatBox->Add(0, 0, 1);

    m_sizerStatic->Add(m_sizerStatBox, 1, wxGROW);

    m_sizerStatic->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void StaticWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateStatic();
}

void StaticWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateStatic();
}

void StaticWidgetsPage::OnButtonBoxText(wxCommandEvent& WXUNUSED(event))
{
    m_sizerStatBox->GetStaticBox()->SetLabel(m_textBox->GetValue());
}

void StaticWidgetsPage::OnButtonLabelText(wxCommandEvent& WXUNUSED(event))
{
    m_statText->SetLabel(m_textLabel->GetValue());
}

