/////////////////////////////////////////////////////////////////////////////
// Name:        texttest.cpp
// Purpose:     wxTextCtrl sample
// Author:      Vadim Zeitlin
// Id:          $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
   Current bugs:

 */

// ============================================================================
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "texttest.cpp"
    #pragma interface "texttest.cpp"
#endif

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
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/dcclient.h"

    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/checklst.h"
    #include "wx/radiobox.h"
    #include "wx/radiobut.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
#endif

#include "wx/sizer.h"

#ifdef __WXUNIVERSAL__
    #include "wx/univ/theme.h"
#endif // __WXUNIVERSAL__

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    TextTest_Reset = 100,
    TextTest_ClearLog,

    TextTest_Add,
    TextTest_Insert,
    TextTest_Clear,

#if 0
    TextTest_AddText,
    TextTest_AddSeveral,
    TextTest_AddMany,
    TextTest_Change,
    TextTest_ChangeText,
    TextTest_Delete,
    TextTest_DeleteText,
    TextTest_DeleteSel,
#endif // 0

    TextTest_Password,
    TextTest_Textctrl,
    TextTest_Quit
};

// textctrl line number radiobox values
enum
{
    TextLines_Single,
    TextLines_Multi
};

// ----------------------------------------------------------------------------
// our classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class TextTestApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class TextTestFrame : public wxFrame
{
public:
    // ctor(s) and dtor
    TextTestFrame(const wxString& title);
    virtual ~TextTestFrame();

protected:
    // crate an info text contorl
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

#if 0
    void OnButtonChange(wxCommandEvent& event);
    void OnButtonDelete(wxCommandEvent& event);
    void OnButtonDeleteSel(wxCommandEvent& event);
    void OnButtonAddSeveral(wxCommandEvent& event);
    void OnButtonAddMany(wxCommandEvent& event);
#endif // 0

    void OnButtonQuit(wxCommandEvent& event);

    void OnText(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnUpdateUIClearButton(wxUpdateUIEvent& event);

#if 0
    void OnUpdateUIAddSeveral(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event);
#endif

    void OnUpdateUIPasswordCheckbox(wxUpdateUIEvent& event);

    void OnUpdateUIResetButton(wxUpdateUIEvent& event);

    void OnIdle(wxIdleEvent& event);

    // reset the textctrl parameters
    void Reset();

    // (re)create the textctrl
    void CreateText();

    // textctrl parameters
    // ------------------

    // single or multi line?
    bool m_isSingleLine;

    // readonly or can be modified?
    bool m_isReadOnly;

    // in password or normal mode?
    bool m_isPassword;

    // the controls
    // ------------

    // the one to contain them all
    wxPanel *m_panel;

    // the radiobox to choose between single and multi line
    wxRadioBox *m_radioTextLines;

    // the checkboxes
    wxCheckBox *m_chkPassword,
               *m_chkReadonly;

    // the textctrl itself and the sizer it is in
    wxTextCtrl *m_text;
    wxSizer *m_sizerText;

    // the textctrl for logging messages
    wxTextCtrl *m_textLog;

    // the information text zones
    wxTextCtrl *m_textPosCur,
               *m_textPosLast,
               *m_textSelFrom,
               *m_textSelTo;

    // and the data to show in them
    long m_posCur,
         m_posLast,
         m_selFrom,
         m_selTo;

private:
    // the old log target which we replaced
    wxLog *m_logTargetOld;

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// misc macros
// ----------------------------------------------------------------------------

IMPLEMENT_APP(TextTestApp)

#ifdef __WXUNIVERSAL__
    WX_USE_THEME(win32);
    WX_USE_THEME(gtk);
#endif // __WXUNIVERSAL__

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TextTestFrame, wxFrame)
    EVT_IDLE(TextTestFrame::OnIdle)

    EVT_BUTTON(TextTest_Reset, TextTestFrame::OnButtonReset)
    EVT_BUTTON(TextTest_ClearLog, TextTestFrame::OnButtonClearLog)
    EVT_BUTTON(TextTest_Quit, TextTestFrame::OnButtonQuit)

    EVT_BUTTON(TextTest_Clear, TextTestFrame::OnButtonClear)
    EVT_BUTTON(TextTest_Add, TextTestFrame::OnButtonAdd)
    EVT_BUTTON(TextTest_Insert, TextTestFrame::OnButtonInsert)

#if 0
    EVT_BUTTON(TextTest_Change, TextTestFrame::OnButtonChange)
    EVT_BUTTON(TextTest_Delete, TextTestFrame::OnButtonDelete)
    EVT_BUTTON(TextTest_DeleteSel, TextTestFrame::OnButtonDeleteSel)
    EVT_BUTTON(TextTest_AddSeveral, TextTestFrame::OnButtonAddSeveral)
    EVT_BUTTON(TextTest_AddMany, TextTestFrame::OnButtonAddMany)

    EVT_TEXT_ENTER(TextTest_AddText, TextTestFrame::OnButtonAdd)
    EVT_TEXT_ENTER(TextTest_DeleteText, TextTestFrame::OnButtonDelete)
#endif // 0

    EVT_UPDATE_UI(TextTest_Clear, TextTestFrame::OnUpdateUIClearButton)

#if 0
    EVT_UPDATE_UI(TextTest_AddSeveral, TextTestFrame::OnUpdateUIAddSeveral)
    EVT_UPDATE_UI(TextTest_DeleteText, TextTestFrame::OnUpdateUIClearButton)
    EVT_UPDATE_UI(TextTest_Delete, TextTestFrame::OnUpdateUIDeleteButton)
    EVT_UPDATE_UI(TextTest_Change, TextTestFrame::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(TextTest_ChangeText, TextTestFrame::OnUpdateUIDeleteSelButton)
    EVT_UPDATE_UI(TextTest_DeleteSel, TextTestFrame::OnUpdateUIDeleteSelButton)
#endif // 0

    EVT_UPDATE_UI(TextTest_Password, TextTestFrame::OnUpdateUIPasswordCheckbox)

    EVT_UPDATE_UI(TextTest_Reset, TextTestFrame::OnUpdateUIResetButton)

    EVT_TEXT(TextTest_Textctrl, TextTestFrame::OnText)
    EVT_TEXT_ENTER(TextTest_Textctrl, TextTestFrame::OnTextEnter)

    EVT_CHECKBOX(-1, TextTestFrame::OnCheckOrRadioBox)
    EVT_RADIOBOX(-1, TextTestFrame::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// app class
// ----------------------------------------------------------------------------

bool TextTestApp::OnInit()
{
    wxFrame *frame = new TextTestFrame(_T("wxTextCtrl sample"));
    frame->Show();

    //wxLog::AddTraceMask(_T("textctrl"));
    //wxLog::AddTraceMask(_T("scrollbar"));

    return TRUE;
}

// ----------------------------------------------------------------------------
// top level frame class
// ----------------------------------------------------------------------------

TextTestFrame::TextTestFrame(const wxString& title)
             : wxFrame(NULL, -1, title, wxPoint(100, 100))
{
    // init everything
    m_radioTextLines = (wxRadioBox *)NULL;

    m_chkPassword =
    m_chkReadonly = (wxCheckBox *)NULL;

    m_text =
    m_textLog =
    m_textPosCur =
    m_textPosLast =
    m_textSelFrom =
    m_textSelTo = (wxTextCtrl *)NULL;
    m_sizerText = (wxSizer *)NULL;

    m_posCur =
    m_posLast =
    m_selFrom =
    m_selTo = -2; // not -1 which means "no selection"

    m_logTargetOld = (wxLog *)NULL;

    m_panel = new wxPanel(this, -1);

    /*
       What we create here is a frame having 3 panes: the explanatory pane to
       the left allowing to set the textctrl styles and recreate the control,
       the pane containing the textctrl itself and the lower pane containing
       the buttons which allow to add/change/delete strings to/from it.
    */
    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL),
            *sizerUp = new wxBoxSizer(wxHORIZONTAL),
            *sizerLeft;

    // upper left pane
    static const wxString modes[] =
    {
        _T("single line"),
        _T("multi line"),
    };

    wxStaticBox *box = new wxStaticBox(m_panel, -1, _T("&Set textctrl parameters"));
    m_radioTextLines = new wxRadioBox(m_panel, -1, _T("&Number of lines:"),
                                      wxDefaultPosition, wxDefaultSize,
                                      WXSIZEOF(modes), modes,
                                      1, wxRA_SPECIFY_COLS);

    m_chkPassword = new wxCheckBox(m_panel, TextTest_Password, _T("&Password control"));
    m_chkReadonly = new wxCheckBox(m_panel, -1, _T("&Read-only mode"));

    sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerLeft->Add(m_radioTextLines, 0, wxGROW | wxALL, 5);
    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer
    sizerLeft->Add(m_chkPassword, 0, wxLEFT | wxRIGHT, 5);
    sizerLeft->Add(m_chkReadonly, 0, wxLEFT | wxRIGHT, 5);

    wxButton *btn = new wxButton(m_panel, TextTest_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(m_panel, -1, _T("&Change contents:"));
    wxSizer *sizerMiddleUp = new wxStaticBoxSizer(box2, wxVERTICAL);

    btn = new wxButton(m_panel, TextTest_Add, _T("&Append text"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(m_panel, TextTest_Insert, _T("&Insert text"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(m_panel, TextTest_Clear, _T("&Clear"));
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 5);

#if 0
    btn = new wxButton(m_panel, TextTest_AddSeveral, _T("&Insert a few strings"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(m_panel, TextTest_AddMany, _T("Add &many strings"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(m_panel, TextTest_Change, _T("C&hange current"));
    m_textChange = new wxTextCtrl(m_panel, TextTest_ChangeText, _T(""));
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textChange, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(m_panel, TextTest_Delete, _T("&Delete this item"));
    m_textDelete = new wxTextCtrl(m_panel, TextTest_DeleteText, _T(""));
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textDelete, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(m_panel, TextTest_DeleteSel, _T("Delete &selection"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);
#endif // 0

    wxStaticBox *box4 = new wxStaticBox(m_panel, -1, _T("&Info:"));
    wxSizer *sizerMiddleDown = new wxStaticBoxSizer(box4, wxVERTICAL);

    m_textPosCur = CreateInfoText();
    sizerMiddleDown->Add
                     (
                        CreateTextWithLabelSizer
                        (
                          _T("Current position:"),
                          m_textPosCur
                        ),
                        0, wxALL | wxGROW, 5
                     );

    m_textPosLast = CreateInfoText();
    sizerMiddleDown->Add
                     (
                        CreateTextWithLabelSizer
                        (
                          _T("Last position:"),
                          m_textPosLast
                        ),
                        0, wxALL | wxGROW, 5
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
                        0, wxALL | wxGROW, 5
                     );
    wxSizer *sizerMiddle = new wxBoxSizer(wxVERTICAL);
    sizerMiddle->Add(sizerMiddleUp, 1, wxGROW, 5);
    sizerMiddle->Add(sizerMiddleDown, 1, wxGROW | wxTOP, 5);

    // right pane
    wxStaticBox *box3 = new wxStaticBox(m_panel, -1, _T("&Text:"));
    m_sizerText = new wxStaticBoxSizer(box3, wxHORIZONTAL);
    m_text = new wxTextCtrl(m_panel, TextTest_Textctrl, _T("Hello, world!"));
    m_sizerText->Add(m_text, 1, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
    m_sizerText->SetMinSize(250, 300);

    // the 3 panes panes compose the upper part of the window
    sizerUp->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerUp->Add(sizerMiddle, 1, wxGROW | wxALL, 10);
    sizerUp->Add(m_sizerText, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // the lower one only has the log textctrl and a button to clear it
    wxSizer *sizerDown = new wxStaticBoxSizer
                             (
                               new wxStaticBox(m_panel, -1, _T("&Log window")),
                               wxVERTICAL
                             );
    m_textLog = new wxTextCtrl(m_panel, -1, _T(""),
                               wxDefaultPosition,
    // wxGTK bug: creating control with default size break layout
#ifdef __WXGTK__
                               wxSize(-1, 100),
#else
                               wxDefaultSize,
#endif
                               wxTE_MULTILINE);
    sizerDown->Add(m_textLog, 1, wxGROW | wxALL, 5);
    wxBoxSizer *sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(m_panel, TextTest_ClearLog, _T("Clear &log"));
    sizerBtns->Add(btn);
    sizerBtns->Add(10, 0); // spacer
    btn = new wxButton(m_panel, TextTest_Quit, _T("E&xit"));
    sizerBtns->Add(btn);
    sizerDown->Add(sizerBtns, 0, wxALL | wxALIGN_RIGHT, 5);

    // put everything together
    sizerTop->Add(sizerUp, 1, wxGROW | (wxALL & ~(wxTOP | wxBOTTOM)), 10);
    sizerTop->Add(0, 5, 0, wxGROW); // spacer in between
    sizerTop->Add(sizerDown, 0,  wxGROW | (wxALL & ~wxTOP), 10);

    // final initialization
    Reset();

    m_panel->SetAutoLayout(TRUE);
    m_panel->SetSizer(sizerTop);

    sizerTop->Fit(this);
    sizerTop->SetSizeHints(this);

    // now that everything is created we can redirect the log messages to the
    // textctrl
    m_logTargetOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_textLog));
}

TextTestFrame::~TextTestFrame()
{
    delete wxLog::SetActiveTarget(m_logTargetOld);
}

// ----------------------------------------------------------------------------
// creation helpers
// ----------------------------------------------------------------------------

wxTextCtrl *TextTestFrame::CreateInfoText()
{
    static int s_maxWidth = 0;
    if ( !s_maxWidth )
    {
        // calc it once only
        GetTextExtent(_T("99999"), &s_maxWidth, NULL);
    }

    wxTextCtrl *text = new wxTextCtrl(m_panel, -1, _T(""),
                                      wxDefaultPosition,
                                      wxSize(s_maxWidth, -1),
                                      wxTE_READONLY);
    return text;
}

wxSizer *TextTestFrame::CreateTextWithLabelSizer(const wxString& label,
                                                 wxTextCtrl *text,
                                                 const wxString& label2,
                                                 wxTextCtrl *text2)
{
    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    sizerRow->Add(new wxStaticText(m_panel, -1, label), 0,
                  wxALIGN_CENTRE_VERTICAL | wxRIGHT, 5);
    sizerRow->Add(text, 0, wxALIGN_CENTRE_VERTICAL);
    if ( text2 )
    {
        sizerRow->Add(new wxStaticText(m_panel, -1, label2), 0,
                      wxALIGN_CENTRE_VERTICAL | wxLEFT | wxRIGHT, 5);
        sizerRow->Add(text2, 0, wxALIGN_CENTRE_VERTICAL);
    }

    return sizerRow;
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void TextTestFrame::Reset()
{
    m_radioTextLines->SetSelection(TextLines_Single);
    m_chkPassword->SetValue(FALSE);
    m_chkReadonly->SetValue(FALSE);
}

void TextTestFrame::CreateText()
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

    wxString valueOld;
    if ( m_text )
    {
        valueOld = m_text->GetValue();

        m_sizerText->Remove(m_text);
        delete m_text;
    }

    m_text = new wxTextCtrl(m_panel, TextTest_Textctrl,
                            valueOld,
                            wxDefaultPosition, wxDefaultSize,
                            flags);
    m_sizerText->Add(m_text, 1, wxALL |
                     (flags & wxTE_MULTILINE ? wxGROW
                                             : wxALIGN_CENTRE_VERTICAL), 5);
    m_sizerText->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void TextTestFrame::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    // update all info texts

    if ( m_textPosCur )
    {
        long posCur = m_text->GetInsertionPoint();
        if ( posCur != m_posCur )
        {
            m_textPosCur->Clear();
            *m_textPosCur << posCur;

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

void TextTestFrame::OnButtonQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void TextTestFrame::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateText();
}

void TextTestFrame::OnButtonClearLog(wxCommandEvent& WXUNUSED(event))
{
    m_textLog->Clear();
}

void TextTestFrame::OnButtonAdd(wxCommandEvent& WXUNUSED(event))
{
    m_text->AppendText(_T("here, there and everywhere"));
    m_text->SetFocus();
}

void TextTestFrame::OnButtonInsert(wxCommandEvent& WXUNUSED(event))
{
    m_text->AppendText(_T("is there anybody going to listen to my story"));
    m_text->SetFocus();
}

void TextTestFrame::OnButtonClear(wxCommandEvent& WXUNUSED(event))
{
    m_text->Clear();
    m_text->SetFocus();
}

#if 0
void TextTestFrame::OnButtonChange(wxCommandEvent& WXUNUSED(event))
{
    wxArrayInt selections;
    int count = m_text->GetSelections(selections);
    wxString s = m_textChange->GetValue();
    for ( int n = 0; n < count; n++ )
    {
        m_text->SetString(selections[n], s);
    }
}

void TextTestFrame::OnButtonDelete(wxCommandEvent& WXUNUSED(event))
{
    unsigned long n;
    if ( !m_textDelete->GetValue().ToULong(&n) ||
            (n >= (unsigned)m_text->GetCount()) )
    {
        return;
    }

    m_text->Delete(n);
}

void TextTestFrame::OnButtonDeleteSel(wxCommandEvent& WXUNUSED(event))
{
    wxArrayInt selections;
    int n = m_text->GetSelections(selections);
    while ( n > 0 )
    {
        m_text->Delete(selections[--n]);
    }
}

void TextTestFrame::OnButtonAdd(wxCommandEvent& event)
{
    static size_t s_item = 0;

    wxString s = m_textAdd->GetValue();
    if ( !m_textAdd->IsModified() )
    {
        // update the default string
        m_textAdd->SetValue(wxString::Format(_T("test item %u"), ++s_item));
    }

    m_text->Append(s);
}

void TextTestFrame::OnButtonAddMany(wxCommandEvent& WXUNUSED(event))
{
    // "many" means 1000 here
    for ( size_t n = 0; n < 1000; n++ )
    {
        m_text->Append(wxString::Format(_T("item #%u"), n));
    }
}

void TextTestFrame::OnButtonAddSeveral(wxCommandEvent& event)
{
    wxArrayString items;
    items.Add(_T("First"));
    items.Add(_T("another one"));
    items.Add(_T("and the last (very very very very very very very very very very long) one"));
    m_text->InsertItems(items, 0);
}

void TextTestFrame::OnUpdateUIDeleteButton(wxUpdateUIEvent& event)
{
    unsigned long n;
    event.Enable(m_textDelete->GetValue().ToULong(&n) &&
                    (n < (unsigned)m_text->GetCount()));
}

void TextTestFrame::OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event)
{
    wxArrayInt selections;
    event.Enable(m_text->GetSelections(selections) != 0);
}

void TextTestFrame::OnUpdateUIAddSeveral(wxUpdateUIEvent& event)
{
    event.Enable(!(m_text->GetWindowStyle() & wxLB_SORT));
}

#endif // 0

void TextTestFrame::OnUpdateUIClearButton(wxUpdateUIEvent& event)
{
    event.Enable(!m_text->GetValue().empty());
}

void TextTestFrame::OnUpdateUIPasswordCheckbox(wxUpdateUIEvent& event)
{
    // can't put multiline control in password mode
    event.Enable( m_radioTextLines->GetSelection() == TextLines_Single );
}

void TextTestFrame::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( (m_radioTextLines->GetSelection() != TextLines_Single) ||
                  m_chkReadonly->GetValue() ||
                  m_chkPassword->GetValue() );
}

void TextTestFrame::OnText(wxCommandEvent& event)
{
    wxLogMessage(_T("Text ctrl value changed"));
}

void TextTestFrame::OnTextEnter(wxCommandEvent& event)
{
    wxLogMessage(_T("Text entered: '%s'"), event.GetString().c_str());
}

void TextTestFrame::OnCheckOrRadioBox(wxCommandEvent& event)
{
    CreateText();
}

