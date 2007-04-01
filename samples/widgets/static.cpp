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
    StaticPage_LabelText,
    StaticPage_LabelTextWithMarkup
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

enum
{
    StaticEllipsize_Start,
    StaticEllipsize_Middle,
    StaticEllipsize_End
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
    void OnButtonLabelWithMarkupText(wxCommandEvent& event);

    // reset all parameters
    void Reset();

    // (re)create all controls
    void CreateStatic();

    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkVert,
               *m_chkAutoResize,
               *m_chkEllipsize,
               *m_chkMarkup;

    wxRadioBox *m_radioHAlign,
               *m_radioVAlign,
               *m_radioEllipsize;

    // the controls and the sizer containing them
    wxStaticBox *m_staticBox;
    wxStaticBoxSizer *m_sizerStatBox;
    wxStaticText *m_statText,
                 *m_statTextWithMarkup;
#if wxUSE_STATLINE
    wxStaticLine *m_statLine;
#endif // wxUSE_STATLINE
    wxSizer *m_sizerStatic;

    // the text entries for command parameters
    wxTextCtrl *m_textBox,
               *m_textLabel,
               *m_textLabelWithMarkup;

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
    EVT_BUTTON(StaticPage_LabelTextWithMarkup, StaticWidgetsPage::OnButtonLabelWithMarkupText)
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
    m_statText = m_statTextWithMarkup = (wxStaticText *)NULL;

    m_staticBox = (wxStaticBox *)NULL;
    m_sizerStatBox = (wxStaticBoxSizer *)NULL;
    m_sizerStatic = (wxSizer *)NULL;

    m_textBox = m_textLabel = m_textLabelWithMarkup = NULL;
}

void StaticWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, _T("&Set style"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkMarkup = CreateCheckBoxAndAddToSizer(sizerLeft, _T("Support &markup"));
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


    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    m_chkEllipsize = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Ellipsize"));

    static const wxString ellipsizeMode[] =
    {
        _T("&start"),
        _T("&middle"),
        _T("&end"),
    };

    m_radioEllipsize = new wxRadioBox(this, wxID_ANY, _T("&Ellipsize mode"),
                                      wxDefaultPosition, wxDefaultSize,
                                      WXSIZEOF(ellipsizeMode), ellipsizeMode);

    sizerLeft->Add(m_radioEllipsize, 0, wxGROW | wxALL, 5);

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

    sizerRow = CreateSizerWithTextAndButton(StaticPage_LabelTextWithMarkup,
                                            _T("Change decorated text label"),
                                            wxID_ANY, &m_textLabelWithMarkup);
    sizerMiddle->Add(sizerRow, 0, wxGROW | wxALL, 5);


    // final initializations
    // NB: must be done _before_ calling CreateStatic()
    Reset();

    m_textBox->SetValue(_T("This is a box"));
    m_textLabel->SetValue(_T("And this is a\n\tlabel inside the box with a &mnemonic.\n")
                          _T("Only this text is affected by the ellipsize settings."));
    m_textLabelWithMarkup->SetValue(_T("Another label, this time <b>decorated</b> ")
                                    _T("with <u>markup</u>; here you need entities ")
                                    _T("for the symbols: &lt; &gt; &amp; &apos; &quot; ")
                                    _T(" but you can still place &mnemonics..."));

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);
    sizerRight->SetMinSize(150, 0);
    m_sizerStatic = sizerRight;

    CreateStatic();

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    SetSizer(sizerTop);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void StaticWidgetsPage::Reset()
{
    m_chkVert->SetValue(false);
    m_chkAutoResize->SetValue(true);
    m_chkEllipsize->SetValue(true);
    m_chkMarkup->SetValue(true);

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
        delete m_statTextWithMarkup;
#if wxUSE_STATLINE
        delete m_statLine;
#endif // wxUSE_STATLINE
    }

    int flagsBox = 0,
        flagsText = ms_defaultFlags,
        flagsDummyText = ms_defaultFlags;

    if ( !m_chkAutoResize->GetValue() )
    {
        flagsText |= wxST_NO_AUTORESIZE;
        flagsDummyText |= wxST_NO_AUTORESIZE;
    }

    if ( m_chkMarkup->GetValue() )
    {
        flagsText |= wxST_MARKUP;
        flagsDummyText |= wxST_MARKUP;
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

    if ( m_chkEllipsize->GetValue() )
    {
        switch ( m_radioEllipsize->GetSelection() )
        {
            default:
                wxFAIL_MSG(_T("unexpected radiobox selection"));
                // fall through

            case StaticEllipsize_Start:
                flagsDummyText |= wxST_ELLIPSIZE_START;
                break;

            case StaticEllipsize_Middle:
                flagsDummyText |= wxST_ELLIPSIZE_MIDDLE;
                break;

            case StaticEllipsize_End:
                flagsDummyText |= wxST_ELLIPSIZE_END;
                break;
        }
    }

    flagsDummyText |= align;
    flagsText |= align;
    flagsBox |= align;

    m_staticBox = new MyStaticBox(this, wxID_ANY, m_textBox->GetValue(),
                                  wxDefaultPosition, wxDefaultSize,
                                  flagsBox);
    m_sizerStatBox = new wxStaticBoxSizer(m_staticBox, isVert ? wxHORIZONTAL
                                                              : wxVERTICAL);

    m_statText = new MyStaticText(this, wxID_ANY, m_textLabel->GetValue(),
                                  wxDefaultPosition, wxDefaultSize,
                                            flagsDummyText);
    m_statTextWithMarkup = new wxStaticText(this, wxID_ANY, 
                                            m_textLabelWithMarkup->GetValue(),
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
    m_sizerStatBox->Add(m_statTextWithMarkup, 1, wxGROW | wxALL, 5);

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

void StaticWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& event)
{
    if (event.GetEventObject() == wx_static_cast(wxObject*, m_chkEllipsize))
    {
        m_radioEllipsize->Enable(event.IsChecked());
    }

    CreateStatic();
}

void StaticWidgetsPage::OnButtonBoxText(wxCommandEvent& WXUNUSED(event))
{
    m_sizerStatBox->GetStaticBox()->SetLabel(m_textBox->GetValue());
}

void StaticWidgetsPage::OnButtonLabelWithMarkupText(wxCommandEvent& WXUNUSED(event))
{
    m_statTextWithMarkup->SetLabel(m_textLabelWithMarkup->GetValue());

    // test GetLabel() and GetLabelText(); the first should return the
    // label as it is written in the relative text control; the second should
    // return the label as it's shown in the wxStaticText
    wxLogMessage(wxT("The original label should be '%s'"),
                 m_statTextWithMarkup->GetLabel().c_str());
    wxLogMessage(wxT("The label text is '%s'"),
                 m_statTextWithMarkup->GetLabelText().c_str());
}

void StaticWidgetsPage::OnButtonLabelText(wxCommandEvent& WXUNUSED(event))
{
    m_statText->SetLabel(m_textLabel->GetValue());

    // test GetLabel() and GetLabelText(); the first should return the
    // label as it is written in the relative text control; the second should
    // return the label as it's shown in the wxStaticText
    wxLogMessage(wxT("The original label should be '%s'"),
                 m_statText->GetLabel().c_str());
    wxLogMessage(wxT("The label text is '%s'"),
                 m_statText->GetLabelText().c_str());
}

