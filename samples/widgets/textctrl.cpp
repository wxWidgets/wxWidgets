/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        textctrl.cpp
// Purpose:     part of the widgets sample showing wxTextCtrl
// Author:      Vadim Zeitlin
// Created:     27.03.01
// Id:          $Id$
// Copyright:   (c) 2001 Vadim Zeitlin
// Licence:     wxWindows licence
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
    #include "wx/timer.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/msgdlg.h"
#endif

#include "wx/sizer.h"
#include "wx/ioswrap.h"

#include "widgets.h"

#include "icons/text.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    TextPage_Reset = wxID_HIGHEST,

    TextPage_Set,
    TextPage_Add,
    TextPage_Insert,
    TextPage_Clear,
    TextPage_Load,

    TextPage_StreamRedirector,

    TextPage_Password,
    TextPage_WrapLines,
    TextPage_Textctrl
};

// textctrl line number radiobox values
enum TextLines
{
    TextLines_Single,
    TextLines_Multi,
    TextLines_Max
};

// wrap style radio box
enum WrapStyle
{
    WrapStyle_None,
    WrapStyle_Word,
    WrapStyle_Char,
    WrapStyle_Best,
    WrapStyle_Max
};

#ifdef __WXMSW__

// textctrl kind values
enum TextKind
{
    TextKind_Plain,
    TextKind_Rich,
    TextKind_Rich2,
    TextKind_Max
};

#endif // __WXMSW__

// default values for the controls
static const struct ControlValues
{
    TextLines textLines;

    bool password;
    bool readonly;
    bool processEnter;
    bool filename;

    WrapStyle wrapStyle;

#ifdef __WXMSW__
    TextKind textKind;
#endif // __WXMSW__
} DEFAULTS =
{
    TextLines_Multi,    // multiline
    false,              // not password
    false,              // not readonly
    true,               // do process enter
    false,              // not filename
    WrapStyle_Word,     // wrap on word boundaries
#ifdef __WXMSW__
    TextKind_Plain      // plain EDIT control
#endif // __WXMSW__
};

// ----------------------------------------------------------------------------
// TextWidgetsPage
// ----------------------------------------------------------------------------

// Define a new frame type: this is going to be our main frame
class TextWidgetsPage : public WidgetsPage
{
public:
    // ctor(s) and dtor
    TextWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~TextWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_text; }
    virtual wxTextEntryBase *GetTextEntry() const { return m_text; }
    virtual void RecreateWidget() { CreateText(); }

    // lazy creation of the content
    virtual void CreateContent();

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

    void OnButtonSet(wxCommandEvent& event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonInsert(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonLoad(wxCommandEvent& event);

    void OnStreamRedirector(wxCommandEvent& event);
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

    // and another one to choose the wrapping style
    wxRadioBox *m_radioWrap;

    // the checkboxes controlling text ctrl styles
    wxCheckBox *m_chkPassword,
               *m_chkReadonly,
               *m_chkProcessEnter,
               *m_chkFilename;

    // under MSW we test rich edit controls as well here
#ifdef __WXMSW__
    wxRadioBox *m_radioKind;
#endif // __WXMSW__

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
               *m_textSelTo,
               *m_textRange;

    // and the data to show in them
    long m_posCur,
         m_posLast,
         m_selFrom,
         m_selTo;

    wxString m_range10_20;

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(TextWidgetsPage)
};

// ----------------------------------------------------------------------------
// WidgetsTextCtrl
// ----------------------------------------------------------------------------

class WidgetsTextCtrl : public wxTextCtrl
{
public:
    WidgetsTextCtrl(wxWindow *parent,
                    wxWindowID id,
                    const wxString& value,
                    int flags)
        : wxTextCtrl(parent, id, value, wxDefaultPosition, wxDefaultSize, flags)
    {
    }

protected:
    void OnRightClick(wxMouseEvent& event)
    {
        wxString where;
        wxTextCoord x, y;
        switch ( HitTest(event.GetPosition(), &x, &y) )
        {
            default:
                wxFAIL_MSG( wxT("unexpected HitTest() result") );
                // fall through

            case wxTE_HT_UNKNOWN:
                x = y = -1;
                where = wxT("nowhere near");
                break;

            case wxTE_HT_BEFORE:
                where = wxT("before");
                break;

            case wxTE_HT_BELOW:
                where = wxT("below");
                break;

            case wxTE_HT_BEYOND:
                where = wxT("beyond");
                break;

            case wxTE_HT_ON_TEXT:
                where = wxT("at");
                break;
        }

        wxLogMessage(wxT("Mouse is %s (%ld, %ld)"), where.c_str(), x, y);

        event.Skip();
    }

private:
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TextWidgetsPage, WidgetsPage)
    EVT_IDLE(TextWidgetsPage::OnIdle)

    EVT_BUTTON(TextPage_Reset, TextWidgetsPage::OnButtonReset)

    EVT_BUTTON(TextPage_StreamRedirector, TextWidgetsPage::OnStreamRedirector)

    EVT_BUTTON(TextPage_Clear, TextWidgetsPage::OnButtonClear)
    EVT_BUTTON(TextPage_Set, TextWidgetsPage::OnButtonSet)
    EVT_BUTTON(TextPage_Add, TextWidgetsPage::OnButtonAdd)
    EVT_BUTTON(TextPage_Insert, TextWidgetsPage::OnButtonInsert)
    EVT_BUTTON(TextPage_Load, TextWidgetsPage::OnButtonLoad)

    EVT_UPDATE_UI(TextPage_Clear, TextWidgetsPage::OnUpdateUIClearButton)

    EVT_UPDATE_UI(TextPage_Password, TextWidgetsPage::OnUpdateUIPasswordCheckbox)
    EVT_UPDATE_UI(TextPage_WrapLines, TextWidgetsPage::OnUpdateUIWrapLinesCheckbox)

    EVT_UPDATE_UI(TextPage_Reset, TextWidgetsPage::OnUpdateUIResetButton)

    EVT_TEXT(TextPage_Textctrl, TextWidgetsPage::OnText)
    EVT_TEXT_ENTER(TextPage_Textctrl, TextWidgetsPage::OnTextEnter)

    EVT_CHECKBOX(wxID_ANY, TextWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, TextWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(WidgetsTextCtrl, wxTextCtrl)
    EVT_RIGHT_UP(WidgetsTextCtrl::OnRightClick)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXX11__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#elif defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(TextWidgetsPage, wxT("Text"),
                       FAMILY_CTRLS | EDITABLE_CTRLS
                       );

// ----------------------------------------------------------------------------
// TextWidgetsPage creation
// ----------------------------------------------------------------------------

TextWidgetsPage::TextWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist)
               : WidgetsPage(book, imaglist, text_xpm)
{
    // init everything
#ifdef __WXMSW__
    m_radioKind =
#endif // __WXMSW__
    m_radioWrap =
    m_radioTextLines = (wxRadioBox *)NULL;

    m_chkPassword =
    m_chkReadonly =
    m_chkProcessEnter =
    m_chkFilename = (wxCheckBox *)NULL;

    m_text =
    m_textPosCur =
    m_textRowCur =
    m_textColCur =
    m_textPosLast =
    m_textLineLast =
    m_textSelFrom =
    m_textSelTo =
    m_textRange = (wxTextCtrl *)NULL;

    m_sizerText = (wxSizer *)NULL;

    m_posCur =
    m_posLast =
    m_selFrom =
    m_selTo = -2; // not -1 which means "no selection"
}

void TextWidgetsPage::CreateContent()
{
    // left pane
    static const wxString modes[] =
    {
        wxT("single line"),
        wxT("multi line"),
    };

    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, wxT("&Set textctrl parameters"));
    m_radioTextLines = new wxRadioBox(this, wxID_ANY, wxT("&Number of lines:"),
                                      wxDefaultPosition, wxDefaultSize,
                                      WXSIZEOF(modes), modes,
                                      1, wxRA_SPECIFY_COLS);

    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerLeft->Add(m_radioTextLines, 0, wxGROW | wxALL, 5);
    sizerLeft->AddSpacer(5);

    m_chkPassword = CreateCheckBoxAndAddToSizer(
                        sizerLeft, wxT("&Password control"), TextPage_Password
                    );
    m_chkReadonly = CreateCheckBoxAndAddToSizer(
                        sizerLeft, wxT("&Read-only mode")
                    );
    m_chkProcessEnter = CreateCheckBoxAndAddToSizer(
                        sizerLeft, wxT("Process &Enter")
                    );
    m_chkFilename = CreateCheckBoxAndAddToSizer(
                        sizerLeft, wxT("&Filename control")
                    );
    m_chkFilename->Disable(); // not implemented yet
    sizerLeft->AddSpacer(5);

    static const wxString wrap[] =
    {
        wxT("no wrap"),
        wxT("word wrap"),
        wxT("char wrap"),
        wxT("best wrap"),
    };

    m_radioWrap = new wxRadioBox(this, wxID_ANY, wxT("&Wrap style:"),
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(wrap), wrap,
                                 1, wxRA_SPECIFY_COLS);
    sizerLeft->Add(m_radioWrap, 0, wxGROW | wxALL, 5);

#ifdef __WXMSW__
    static const wxString kinds[] =
    {
        wxT("plain edit"),
        wxT("rich edit"),
        wxT("rich edit 2.0"),
    };

    m_radioKind = new wxRadioBox(this, wxID_ANY, wxT("Control &kind"),
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(kinds), kinds,
                                 1, wxRA_SPECIFY_COLS);

    sizerLeft->AddSpacer(5);
    sizerLeft->Add(m_radioKind, 0, wxGROW | wxALL, 5);
#endif // __WXMSW__

    wxButton *btn = new wxButton(this, TextPage_Reset, wxT("&Reset"));
    sizerLeft->Add(2, 2, 0, wxGROW | wxALL, 1); // spacer
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, wxT("&Change contents:"));
    wxSizer *sizerMiddleUp = new wxStaticBoxSizer(box2, wxVERTICAL);

    btn = new wxButton(this, TextPage_Set, wxT("&Set text value"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 1);

    btn = new wxButton(this, TextPage_Add, wxT("&Append text"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 1);

    btn = new wxButton(this, TextPage_Insert, wxT("&Insert text"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 1);

    btn = new wxButton(this, TextPage_Load, wxT("&Load file"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 1);

    btn = new wxButton(this, TextPage_Clear, wxT("&Clear"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 1);

    btn = new wxButton(this, TextPage_StreamRedirector, wxT("St&ream redirection"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 1);

    wxStaticBox *box4 = new wxStaticBox(this, wxID_ANY, wxT("&Info:"));
    wxSizer *sizerMiddleDown = new wxStaticBoxSizer(box4, wxVERTICAL);

    m_textPosCur = CreateInfoText();
    m_textRowCur = CreateInfoText();
    m_textColCur = CreateInfoText();

    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    sizerRow->Add(CreateTextWithLabelSizer
                  (
                    wxT("Current pos:"),
                    m_textPosCur
                  ),
                  0, wxRIGHT, 5);
    sizerRow->Add(CreateTextWithLabelSizer
                  (
                    wxT("Col:"),
                    m_textColCur
                  ),
                  0, wxLEFT | wxRIGHT, 5);
    sizerRow->Add(CreateTextWithLabelSizer
                  (
                    wxT("Row:"),
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
                          wxT("Number of lines:"),
                          m_textLineLast,
                          wxT("Last position:"),
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
                          wxT("Selection: from"),
                          m_textSelFrom,
                          wxT("to"),
                          m_textSelTo
                        ),
                        0, wxALL, 5
                     );

    m_textRange = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY);
    sizerMiddleDown->Add
                     (
                        CreateTextWithLabelSizer
                        (
                          wxT("Range 10..20:"),
                          m_textRange
                        ),
                        0, wxALL, 5
                     );

    wxSizer *sizerMiddle = new wxBoxSizer(wxVERTICAL);
    sizerMiddle->Add(sizerMiddleUp, 0, wxGROW);
    sizerMiddle->Add(sizerMiddleDown, 1, wxGROW | wxTOP, 5);

    // right pane
    wxStaticBox *box3 = new wxStaticBox(this, wxID_ANY, wxT("&Text:"));
    m_sizerText = new wxStaticBoxSizer(box3, wxHORIZONTAL);
    Reset();
    CreateText();
    m_sizerText->SetMinSize(150, 0);

    // the 3 panes panes compose the upper part of the window
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, wxGROW | wxALL, 10);
    sizerTop->Add(m_sizerText, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    SetSizer(sizerTop);
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
        GetTextExtent(wxT("9999999"), &s_maxWidth, NULL);
    }

    wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                      wxDefaultPosition,
                                      wxSize(s_maxWidth, wxDefaultCoord),
                                      wxTE_READONLY);
    return text;
}

wxSizer *TextWidgetsPage::CreateTextWithLabelSizer(const wxString& label,
                                                 wxTextCtrl *text,
                                                 const wxString& label2,
                                                 wxTextCtrl *text2)
{
    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    sizerRow->Add(new wxStaticText(this, wxID_ANY, label), 0,
                  wxALIGN_CENTRE_VERTICAL | wxRIGHT, 5);
    sizerRow->Add(text, 0, wxALIGN_CENTRE_VERTICAL);
    if ( text2 )
    {
        sizerRow->Add(new wxStaticText(this, wxID_ANY, label2), 0,
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
    m_chkReadonly->SetValue(DEFAULTS.readonly);
    m_chkProcessEnter->SetValue(DEFAULTS.processEnter);
    m_chkFilename->SetValue(DEFAULTS.filename);

    m_radioWrap->SetSelection(DEFAULTS.wrapStyle);

#ifdef __WXMSW__
    m_radioKind->SetSelection(DEFAULTS.textKind);
#endif // __WXMSW__
}

void TextWidgetsPage::CreateText()
{
    int flags = ms_defaultFlags;
    switch ( m_radioTextLines->GetSelection() )
    {
        default:
            wxFAIL_MSG( wxT("unexpected lines radio box selection") );

        case TextLines_Single:
            break;

        case TextLines_Multi:
            flags |= wxTE_MULTILINE;
            m_chkPassword->SetValue(false);
            break;
    }

    if ( m_chkPassword->GetValue() )
        flags |= wxTE_PASSWORD;
    if ( m_chkReadonly->GetValue() )
        flags |= wxTE_READONLY;
    if ( m_chkProcessEnter->GetValue() )
        flags |= wxTE_PROCESS_ENTER;

    switch ( m_radioWrap->GetSelection() )
    {
        default:
            wxFAIL_MSG( wxT("unexpected wrap style radio box selection") );

        case WrapStyle_None:
            flags |= wxTE_DONTWRAP; // same as wxHSCROLL
            break;

        case WrapStyle_Word:
            flags |= wxTE_WORDWRAP;
            break;

        case WrapStyle_Char:
            flags |= wxTE_CHARWRAP;
            break;

        case WrapStyle_Best:
            // this is default but use symbolic file name for consistency
            flags |= wxTE_BESTWRAP;
            break;
    }

#ifdef __WXMSW__
    switch ( m_radioKind->GetSelection() )
    {
        default:
            wxFAIL_MSG( wxT("unexpected kind radio box selection") );

        case TextKind_Plain:
            break;

        case TextKind_Rich:
            flags |= wxTE_RICH;
            break;

        case TextKind_Rich2:
            flags |= wxTE_RICH2;
            break;
    }
#endif // __WXMSW__

    wxString valueOld;
    if ( m_text )
    {
        valueOld = m_text->GetValue();

        m_sizerText->Detach( m_text );
        delete m_text;
    }
    else
    {
        valueOld = wxT("Hello, Universe!");
    }

    m_text = new WidgetsTextCtrl(this, TextPage_Textctrl, valueOld, flags);

#if 0
    if ( m_chkFilename->GetValue() )
        ;
#endif // TODO

    // cast to int needed to silence gcc warning about different enums
    m_sizerText->Add(m_text, 1, wxALL |
                     (flags & wxTE_MULTILINE ? (int)wxGROW
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
                wxString::Format(wxT("%d"), m_text->GetNumberOfLines()) );
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

    if ( m_textRange )
    {
        wxString range = m_text->GetRange(10, 20);
        if ( range != m_range10_20 )
        {
            m_range10_20 = range;
            m_textRange->SetValue(range);
        }
    }
}

void TextWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateText();
}

void TextWidgetsPage::OnButtonSet(wxCommandEvent& WXUNUSED(event))
{
    m_text->SetValue(m_text->GetWindowStyle() & wxTE_MULTILINE
                        ? wxT("Here,\nthere and\neverywhere")
                        : wxT("Yellow submarine"));

    m_text->SetFocus();
}

void TextWidgetsPage::OnButtonAdd(wxCommandEvent& WXUNUSED(event))
{
    if ( m_text->GetWindowStyle() & wxTE_MULTILINE )
    {
        m_text->AppendText(wxT("We all live in a\n"));
    }

    m_text->AppendText(wxT("Yellow submarine"));
}

void TextWidgetsPage::OnButtonInsert(wxCommandEvent& WXUNUSED(event))
{
    m_text->WriteText(wxT("Is there anybody going to listen to my story"));
    if ( m_text->GetWindowStyle() & wxTE_MULTILINE )
    {
        m_text->WriteText(wxT("\nall about the girl who came to stay"));
    }
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
    pathlist.Add(wxT("."));
    pathlist.Add(wxT(".."));
    pathlist.Add(wxT("../../../samples/widgets"));

    wxString filename = pathlist.FindValidPath(wxT("textctrl.cpp"));
    if ( !filename )
    {
        wxLogError(wxT("File textctrl.cpp not found."));
    }
    else // load it
    {
        wxStopWatch sw;
        if ( !m_text->LoadFile(filename) )
        {
            // this is not supposed to happen ...
            wxLogError(wxT("Error loading file."));
        }
        else
        {
            long elapsed = sw.Time();
            wxLogMessage(wxT("Loaded file '%s' in %lu.%us"),
                         filename.c_str(), elapsed / 1000,
                         (unsigned int) elapsed % 1000);
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
#ifdef __WXMSW__
                  (m_radioKind->GetSelection() != DEFAULTS.textKind) ||
#endif // __WXMSW__
                  (m_chkPassword->GetValue() != DEFAULTS.password) ||
                  (m_chkReadonly->GetValue() != DEFAULTS.readonly) ||
                  (m_chkProcessEnter->GetValue() != DEFAULTS.processEnter) ||
                  (m_chkFilename->GetValue() != DEFAULTS.filename) ||
                  (m_radioWrap->GetSelection() != DEFAULTS.wrapStyle) );
}

void TextWidgetsPage::OnText(wxCommandEvent& WXUNUSED(event))
{
    // small hack to suppress the very first message: by then the logging is
    // not yet redirected and so initial setting of the text value results in
    // an annoying message box
    static bool s_firstTime = true;
    if ( s_firstTime )
    {
        s_firstTime = false;
        return;
    }

    wxLogMessage(wxT("Text ctrl value changed"));
}

void TextWidgetsPage::OnTextEnter(wxCommandEvent& event)
{
    wxLogMessage(wxT("Text entered: '%s'"), event.GetString().c_str());
    event.Skip();
}

void TextWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateText();
}

void TextWidgetsPage::OnStreamRedirector(wxCommandEvent& WXUNUSED(event))
{
#if wxHAS_TEXT_WINDOW_STREAM
    wxStreamToTextRedirector redirect(m_text);
    wxString str( wxT("Outputed to cout, appears in wxTextCtrl!") );
    wxSTD cout << str << wxSTD endl;
#else
    wxMessageBox(wxT("This wxWidgets build does not support wxStreamToTextRedirector"));
#endif
}
