/////////////////////////////////////////////////////////////////////////////
// Program:     wxWindows Widgets Sample
// Name:        textctrl.cpp
// Purpose:     part of the widgets sample showing wxTextCtrl
// Author:      Vadim Zeitlin
// Created:     27.03.01
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
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/textctrl.h"
#endif

#include "wx/sizer.h"

#include "widgets.h"

#include "icons/text.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    TextPage_Reset = 100,

    TextPage_Add,
    TextPage_Insert,
    TextPage_Clear,
    TextPage_Load,

    TextPage_Password,
    TextPage_WrapLines,
    TextPage_Textctrl
};

// textctrl line number radiobox values
enum TextLines
{
    TextLines_Single,
    TextLines_Multi
};

// default values for the controls
static const struct ControlValues
{
    TextLines textLines;
    bool password;
    bool wraplines;
    bool readonly;
} DEFAULTS =
{
    TextLines_Multi,    // multiline
    FALSE,              // not password
    TRUE,               // do wrap lines
    FALSE               // not readonly
};

// ----------------------------------------------------------------------------
// TextWidgetsPage
// ----------------------------------------------------------------------------

// Define a new frame type: this is going to be our main frame
class TextWidgetsPage : public WidgetsPage
{
public:
    // ctor(s) and dtor
    TextWidgetsPage(wxNotebook *notebook, wxImageList *imaglist);
    virtual ~TextWidgetsPage();

protected:
    // create an info text contorl
    wxTextCtrl *CreateInfoText();

    // create a horz sizer holding a static text and this text control
    wxSizer *CreateTextWithLabelSizer(const wxString& label,
                                      wxTextCtrl *text,
                                      const wxString& label2 = wxEmptyString,
                                      wxTextCtrl *text2 = NULL);

    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonClearLog(wxCommandEvent& event);

    void OnButtonInsert(wxCommandEvent& event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonLoad(wxCommandEvent& event);

    void OnButtonQuit(wxCommandEvent& event);

    void OnText(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnUpdateUIClearButton(wxUpdateUIEvent& event);

    void OnUpdateUIPasswordCheckbox(wxUpdateUIEvent& event);
    void OnUpdateUIWrapLinesCheckbox(wxUpdateUIEvent& event);

    void OnUpdateUIResetButton(wxUpdateUIEvent& event);

    void OnIdle(wxIdleEvent& event);

    // reset the textctrl parameters
    void Reset();

    // (re)create the textctrl
    void CreateText();

    // is the control currently single line?
    bool IsSingleLine() const
    {
        return m_radioTextLines->GetSelection() == TextLines_Single;
    }

    // the controls
    // ------------

    // the radiobox to choose between single and multi line
    wxRadioBox *m_radioTextLines;

    // the checkboxes controlling text ctrl styles
    wxCheckBox *m_chkPassword,
               *m_chkWrapLines,
               *m_chkReadonly;

    // the textctrl itself and the sizer it is in
    wxTextCtrl *m_text;
    wxSizer *m_sizerText;

    // the information text zones
    wxTextCtrl *m_textPosCur,
               *m_textRowCur,
               *m_textColCur,
               *m_textPosLast,
               *m_textLineLast,
               *m_textSelFrom,
               *m_textSelTo;

    // and the data to show in them
    long m_posCur,
         m_posLast,
         m_selFrom,
         m_selTo;

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();

    DECLARE_WIDGETS_PAGE(TextWidgetsPage);
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TextWidgetsPage, WidgetsPage)
    EVT_IDLE(TextWidgetsPage::OnIdle)

    EVT_BUTTON(TextPage_Reset, TextWidgetsPage::OnButtonReset)

    EVT_BUTTON(TextPage_Clear, TextWidgetsPage::OnButtonClear)
    EVT_BUTTON(TextPage_Add, TextWidgetsPage::OnButtonAdd)
    EVT_BUTTON(TextPage_Insert, TextWidgetsPage::OnButtonInsert)
    EVT_BUTTON(TextPage_Load, TextWidgetsPage::OnButtonLoad)

    EVT_UPDATE_UI(TextPage_Clear, TextWidgetsPage::OnUpdateUIClearButton)

    EVT_UPDATE_UI(TextPage_Password, TextWidgetsPage::OnUpdateUIPasswordCheckbox)
    EVT_UPDATE_UI(TextPage_WrapLines, TextWidgetsPage::OnUpdateUIWrapLinesCheckbox)

    EVT_UPDATE_UI(TextPage_Reset, TextWidgetsPage::OnUpdateUIResetButton)

    EVT_TEXT(TextPage_Textctrl, TextWidgetsPage::OnText)
    EVT_TEXT_ENTER(TextPage_Textctrl, TextWidgetsPage::OnTextEnter)

    EVT_CHECKBOX(-1, TextWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(-1, TextWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(TextWidgetsPage, _T("Text"));

// ----------------------------------------------------------------------------
// TextWidgetsPage creation
// ----------------------------------------------------------------------------

TextWidgetsPage::TextWidgetsPage(wxNotebook *notebook, wxImageList *imaglist)
               : WidgetsPage(notebook)
{
    imaglist->Add(wxBitmap(text_xpm));

    // init everything
    m_radioTextLines = (wxRadioBox *)NULL;

    m_chkPassword =
    m_chkWrapLines =
    m_chkReadonly = (wxCheckBox *)NULL;

    m_text =
    m_textPosCur =
    m_textRowCur =
    m_textColCur =
    m_textPosLast =
    m_textLineLast =
    m_textSelFrom =
    m_textSelTo = (wxTextCtrl *)NULL;
    m_sizerText = (wxSizer *)NULL;

    m_posCur =
    m_posLast =
    m_selFrom =
    m_selTo = -2; // not -1 which means "no selection"

    // left pane
    static const wxString modes[] =
    {
        _T("single line"),
        _T("multi line"),
    };

    wxStaticBox *box = new wxStaticBox(this, -1, _T("&Set textctrl parameters"));
    m_radioTextLines = new wxRadioBox(this, -1, _T("&Number of lines:"),
                                      wxDefaultPosition, wxDefaultSize,
                                      WXSIZEOF(modes), modes,
                                      1, wxRA_SPECIFY_COLS);

    m_chkPassword = new wxCheckBox(this, TextPage_Password, _T("&Password control"));
    m_chkWrapLines = new wxCheckBox(this, TextPage_WrapLines, _T("Line &wrap"));
    m_chkReadonly = new wxCheckBox(this, -1, _T("&Read-only mode"));

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerLeft->Add(m_radioTextLines, 0, wxGROW | wxALL, 5);
    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer
    sizerLeft->Add(m_chkPassword, 0, wxLEFT | wxRIGHT, 5);
    sizerLeft->Add(m_chkWrapLines, 0, wxLEFT | wxRIGHT, 5);
    sizerLeft->Add(m_chkReadonly, 0, wxLEFT | wxRIGHT, 5);

    wxButton *btn = new wxButton(this, TextPage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, -1, _T("&Change contents:"));
    wxSizer *sizerMiddleUp = new wxStaticBoxSizer(box2, wxVERTICAL);

    btn = new wxButton(this, TextPage_Add, _T("&Append text"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, TextPage_Insert, _T("&Insert text"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, TextPage_Load, _T("&Load file"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(this, TextPage_Clear, _T("&Clear"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 5);

    wxStaticBox *box4 = new wxStaticBox(this, -1, _T("&Info:"));
    wxSizer *sizerMiddleDown = new wxStaticBoxSizer(box4, wxVERTICAL);

    m_textPosCur = CreateInfoText();
    m_textRowCur = CreateInfoText();
    m_textColCur = CreateInfoText();

    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    sizerRow->Add(CreateTextWithLabelSizer
                  (
                    _T("Current pos:"),
                    m_textPosCur
                  ),
                  0, wxRIGHT, 5);
    sizerRow->Add(CreateTextWithLabelSizer
                  (
                    _T("Col:"),
                    m_textColCur
                  ),
                  0, wxLEFT | wxRIGHT, 5);
    sizerRow->Add(CreateTextWithLabelSizer
                  (
                    _T("Row:"),
                    m_textRowCur
                  ),
                  0, wxLEFT, 5);
    sizerMiddleDown->Add(sizerRow, 0, wxALL, 5);

    m_textLineLast = CreateInfoText();
    m_textPosLast = CreateInfoText();
    sizerMiddleDown->Add
                     (
                        CreateTextWithLabelSizer
                        (
                          _T("Number of lines:"),
                          m_textLineLast,
                          _T("Last position:"),
                          m_textPosLast
                        ),
                        0, wxALL, 5
                     );

    m_textSelFrom = CreateInfoText();
    m_textSelTo = CreateInfoText();
    sizerMiddleDown->Add
                     (
                        CreateTextWithLabelSizer
                        (
                          _T("Selection: from"),
                          m_textSelFrom,
                          _T("to"),
                          m_textSelTo
                        ),
                        0, wxALL, 5
                     );
    wxSizer *sizerMiddle = new wxBoxSizer(wxVERTICAL);
    sizerMiddle->Add(sizerMiddleUp, 0, wxGROW);
    sizerMiddle->Add(sizerMiddleDown, 1, wxGROW | wxTOP, 5);

    // right pane
    wxStaticBox *box3 = new wxStaticBox(this, -1, _T("&Text:"));
    m_sizerText = new wxStaticBoxSizer(box3, wxHORIZONTAL);
    Reset();
    CreateText();
    m_sizerText->SetMinSize(250, 0);

    // the 3 panes panes compose the upper part of the window
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, wxGROW | wxALL, 10);
    sizerTop->Add(m_sizerText, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    SetAutoLayout(TRUE);
    SetSizer(sizerTop);

    sizerTop->Fit(this);
}

TextWidgetsPage::~TextWidgetsPage()
{
}

// ----------------------------------------------------------------------------
// creation helpers
// ----------------------------------------------------------------------------

wxTextCtrl *TextWidgetsPage::CreateInfoText()
{
    static int s_maxWidth = 0;
    if ( !s_maxWidth )
    {
        // calc it once only
        GetTextExtent(_T("9999999"), &s_maxWidth, NULL);
    }

    wxTextCtrl *text = new wxTextCtrl(this, -1, _T(""),
                                      wxDefaultPosition,
                                      wxSize(s_maxWidth, -1),
                                      wxTE_READONLY);
    return text;
}

wxSizer *TextWidgetsPage::CreateTextWithLabelSizer(const wxString& label,
                                                 wxTextCtrl *text,
                                                 const wxString& label2,
                                                 wxTextCtrl *text2)
{
    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    sizerRow->Add(new wxStaticText(this, -1, label), 0,
                  wxALIGN_CENTRE_VERTICAL | wxRIGHT, 5);
    sizerRow->Add(text, 0, wxALIGN_CENTRE_VERTICAL);
    if ( text2 )
    {
        sizerRow->Add(new wxStaticText(this, -1, label2), 0,
                      wxALIGN_CENTRE_VERTICAL | wxLEFT | wxRIGHT, 5);
        sizerRow->Add(text2, 0, wxALIGN_CENTRE_VERTICAL);
    }

    return sizerRow;
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void TextWidgetsPage::Reset()
{
    m_radioTextLines->SetSelection(DEFAULTS.textLines);
    m_chkPassword->SetValue(DEFAULTS.password);
    m_chkWrapLines->SetValue(DEFAULTS.wraplines);
    m_chkReadonly->SetValue(DEFAULTS.readonly);
}

void TextWidgetsPage::CreateText()
{
    int flags = 0;
    switch ( m_radioTextLines->GetSelection() )
    {
        default:
            wxFAIL_MSG( _T("unexpected radio box selection") );

        case TextLines_Single:
            break;

        case TextLines_Multi:
            flags |= wxTE_MULTILINE;
            m_chkPassword->SetValue(FALSE);
            break;
    }

    if ( m_chkPassword->GetValue() )
        flags |= wxTE_PASSWORD;
    if ( m_chkReadonly->GetValue() )
        flags |= wxTE_READONLY;
    if ( !m_chkWrapLines->GetValue() )
        flags |= wxHSCROLL;

    wxString valueOld;
    if ( m_text )
    {
        valueOld = m_text->GetValue();

        m_sizerText->Remove(m_text);
        delete m_text;
    }
    else
    {
        valueOld = _T("Hello, Universe!");
    }

    m_text = new wxTextCtrl(this, TextPage_Textctrl,
                            valueOld,
                            wxDefaultPosition, wxDefaultSize,
                            flags);
    m_sizerText->Add(m_text, 1, wxALL |
                     (flags & wxTE_MULTILINE ? wxGROW
                                             : wxALIGN_TOP), 5);
    m_sizerText->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void TextWidgetsPage::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    // update all info texts

    if ( m_textPosCur )
    {
        long posCur = m_text->GetInsertionPoint();
        if ( posCur != m_posCur )
        {
            m_textPosCur->Clear();
            m_textRowCur->Clear();
            m_textColCur->Clear();

            long col, row;
            m_text->PositionToXY(posCur, &col, &row);

            *m_textPosCur << posCur;
            *m_textRowCur << row;
            *m_textColCur << col;

            m_posCur = posCur;
        }
    }

    if ( m_textPosLast )
    {
        long posLast = m_text->GetLastPosition();
        if ( posLast != m_posLast )
        {
            m_textPosLast->Clear();
            *m_textPosLast << posLast;

            m_posLast = posLast;
        }
    }

    if ( m_textLineLast )
    {
        m_textLineLast->SetValue(
                wxString::Format(_T("%ld"), m_text->GetNumberOfLines()));
    }

    if ( m_textSelFrom && m_textSelTo )
    {
        long selFrom, selTo;
        m_text->GetSelection(&selFrom, &selTo);
        if ( selFrom != m_selFrom )
        {
            m_textSelFrom->Clear();
            *m_textSelFrom << selFrom;

            m_selFrom = selFrom;
        }

        if ( selTo != m_selTo )
        {
            m_textSelTo->Clear();
            *m_textSelTo << selTo;

            m_selTo = selTo;
        }
    }
}

void TextWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateText();
}

void TextWidgetsPage::OnButtonAdd(wxCommandEvent& WXUNUSED(event))
{
    m_text->AppendText(_T("here, there and everywhere"));
    m_text->SetFocus();
}

void TextWidgetsPage::OnButtonInsert(wxCommandEvent& WXUNUSED(event))
{
    m_text->WriteText(_T("is there anybody going to listen to my story"));
    m_text->SetFocus();
}

void TextWidgetsPage::OnButtonClear(wxCommandEvent& WXUNUSED(event))
{
    m_text->Clear();
    m_text->SetFocus();
}

void TextWidgetsPage::OnButtonLoad(wxCommandEvent& WXUNUSED(event))
{
    // search for the file in several dirs where it's likely to be
    wxPathList pathlist;
    pathlist.Add(_T("."));
    pathlist.Add(_T(".."));
    pathlist.Add(_T("../../../samples/widgets"));

    wxString filename = pathlist.FindValidPath(_T("textctrl.cpp"));
    if ( !filename )
    {
        wxLogError(_T("File textctrl.cpp not found."));
    }
    else // load it
    {
        wxStopWatch sw;
        if ( !m_text->LoadFile(filename) )
        {
            // this is not supposed to happen ...
            wxLogError(_T("Error loading file."));
        }
        else
        {
            long elapsed = sw.Time();
            wxLogMessage(_T("Loaded file '%s' in %u.%us"),
                         filename.c_str(), elapsed / 1000, elapsed % 1000);
        }
    }
}

void TextWidgetsPage::OnUpdateUIClearButton(wxUpdateUIEvent& event)
{
    event.Enable(!m_text->GetValue().empty());
}

void TextWidgetsPage::OnUpdateUIWrapLinesCheckbox(wxUpdateUIEvent& event)
{
    event.Enable( !IsSingleLine() );
}

void TextWidgetsPage::OnUpdateUIPasswordCheckbox(wxUpdateUIEvent& event)
{
    // can't put multiline control in password mode
    event.Enable( IsSingleLine() );
}

void TextWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( (m_radioTextLines->GetSelection() != DEFAULTS.textLines) ||
                  (m_chkReadonly->GetValue() != DEFAULTS.readonly) ||
                  (m_chkPassword->GetValue() != DEFAULTS.password) ||
                  (m_chkWrapLines->GetValue() != DEFAULTS.wraplines) );
}

void TextWidgetsPage::OnText(wxCommandEvent& event)
{
    // small hack to suppress the very first message: by then the logging is
    // not yet redirected and so initial setting of the text value results in
    // an annoying message box
    static bool s_firstTime = TRUE;
    if ( s_firstTime )
    {
        s_firstTime = FALSE;
        return;
    }

    wxLogMessage(_T("Text ctrl value changed"));
}

void TextWidgetsPage::OnTextEnter(wxCommandEvent& event)
{
    wxLogMessage(_T("Text entered: '%s'"), event.GetString().c_str());
}

void TextWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& event)
{
    CreateText();
}

