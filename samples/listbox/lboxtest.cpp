/////////////////////////////////////////////////////////////////////////////
// Name:        lboxtest.cpp
// Purpose:     wxListBox sample
// Author:      Vadim Zeitlin
// Id:          $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
   Current bugs:

  +1. horz scrollbar doesn't appear in listbox
  +2. truncating text ctrl doesn't update display
  +3. deleting last listbox item doesn't update display
   4. text ctrl background corrupted after resize
 */

// ============================================================================
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "lboxtest.cpp"
    #pragma interface "lboxtest.cpp"
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
    #include "wx/listbox.h"
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
    LboxTest_Reset = 100,
    LboxTest_Create,
    LboxTest_Add,
    LboxTest_AddText,
    LboxTest_AddSeveral,
    LboxTest_Clear,
    LboxTest_ClearLog,
    LboxTest_Delete,
    LboxTest_DeleteText,
    LboxTest_DeleteSel,
    LboxTest_Listbox,
    LboxTest_Quit
};

// ----------------------------------------------------------------------------
// our classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class LboxTestApp : public wxApp
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
class LboxTestFrame : public wxFrame
{
public:
    // ctor(s) and dtor
    LboxTestFrame(const wxString& title);
    virtual ~LboxTestFrame();

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonCreate(wxCommandEvent& event);
    void OnButtonDelete(wxCommandEvent& event);
    void OnButtonDeleteSel(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonClearLog(wxCommandEvent& event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonAddSeveral(wxCommandEvent& event);
    void OnButtonQuit(wxCommandEvent& event);

    void OnListbox(wxCommandEvent& event);
    void OnListboxDClick(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnUpdateUIAddSeveral(wxUpdateUIEvent& event);
    void OnUpdateUICreateButton(wxUpdateUIEvent& event);
    void OnUpdateUIClearButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteButton(wxUpdateUIEvent& event);
    void OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event);

    // reset the listbox parameters
    void Reset();

    // (re)create the listbox
    void CreateLbox();

    // listbox parameters
    // ------------------

    // the selection mode
    enum LboxSelection
    {
        LboxSel_Single,
        LboxSel_Extended,
        LboxSel_Multiple
    } m_lboxSelMode;

    // should it be sorted?
    bool m_sorted;

    // should it have horz scroll/vert scrollbar permanently shown?
    bool m_horzScroll,
         m_vertScrollAlways;

    // should the recreate button be enabled?
    bool m_dirty;

    // the controls
    // ------------

    // the sel mode radiobox
    wxRadioBox *m_radioSelMode;

    // the checkboxes
    wxCheckBox *m_chkSort,
               *m_chkHScroll,
               *m_chkVScroll;

    // the listbox itself and the sizer it is in
    wxListBox *m_lbox;
    wxSizer *m_sizerLbox;

    // the listbox for logging messages
    wxListBox *m_lboxLog;

    // the text entries for "Add string" and "Delete" buttons
    wxTextCtrl *m_textAdd,
               *m_textDelete;

private:
    // the log target we use to redirect messages to the listbox
    wxLog *m_logTarget;

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// A log target which just redirects the messages to a listbox
class LboxLogger : public wxLog
{
public:
    LboxLogger(wxListBox *lbox, wxLog *logOld)
    {
        m_lbox = lbox;
        m_logOld = logOld;
    }

    virtual ~LboxLogger()
    {
        wxLog::SetActiveTarget(m_logOld);
    }

private:
    // implement sink functions
    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t)
    {
        // don't put trace messages into listbox or we can get into infinite
        // recursion
        if ( level == wxLOG_Trace )
        {
            if ( m_logOld )
            {
                // cast is needed to call protected method
                ((LboxLogger *)m_logOld)->DoLog(level, szString, t);
            }
        }
        else
        {
            wxLog::DoLog(level, szString, t);
        }
    }

    virtual void DoLogString(const wxChar *szString, time_t t)
    {
        wxString msg;
        TimeStamp(&msg);
        msg += szString;
        m_lbox->Append(msg);
    }

    // the control we use
    wxListBox *m_lbox;

    // the old log target
    wxLog *m_logOld;
};

// ----------------------------------------------------------------------------
// misc macros
// ----------------------------------------------------------------------------

IMPLEMENT_APP(LboxTestApp)

#ifdef __WXUNIVERSAL__
    WX_USE_THEME(win32);
    WX_USE_THEME(gtk);
#endif // __WXUNIVERSAL__

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(LboxTestFrame, wxFrame)
    EVT_BUTTON(LboxTest_Reset, LboxTestFrame::OnButtonReset)
    EVT_BUTTON(LboxTest_Create, LboxTestFrame::OnButtonCreate)
    EVT_BUTTON(LboxTest_Delete, LboxTestFrame::OnButtonDelete)
    EVT_BUTTON(LboxTest_DeleteSel, LboxTestFrame::OnButtonDeleteSel)
    EVT_BUTTON(LboxTest_Clear, LboxTestFrame::OnButtonClear)
    EVT_BUTTON(LboxTest_ClearLog, LboxTestFrame::OnButtonClearLog)
    EVT_BUTTON(LboxTest_Add, LboxTestFrame::OnButtonAdd)
    EVT_BUTTON(LboxTest_AddSeveral, LboxTestFrame::OnButtonAddSeveral)
    EVT_BUTTON(LboxTest_Quit, LboxTestFrame::OnButtonQuit)

    EVT_TEXT_ENTER(LboxTest_AddText, LboxTestFrame::OnButtonAdd)
    EVT_TEXT_ENTER(LboxTest_DeleteText, LboxTestFrame::OnButtonDelete)

    EVT_UPDATE_UI_RANGE(LboxTest_Reset, LboxTest_Create,
                        LboxTestFrame::OnUpdateUICreateButton)

    EVT_UPDATE_UI(LboxTest_AddSeveral, LboxTestFrame::OnUpdateUIAddSeveral)
    EVT_UPDATE_UI(LboxTest_Clear, LboxTestFrame::OnUpdateUIClearButton)
    EVT_UPDATE_UI(LboxTest_DeleteText, LboxTestFrame::OnUpdateUIClearButton)
    EVT_UPDATE_UI(LboxTest_Delete, LboxTestFrame::OnUpdateUIDeleteButton)
    EVT_UPDATE_UI(LboxTest_DeleteSel, LboxTestFrame::OnUpdateUIDeleteSelButton)

    EVT_LISTBOX(LboxTest_Listbox, LboxTestFrame::OnListbox)
    EVT_LISTBOX_DCLICK(-1, LboxTestFrame::OnListboxDClick)
    EVT_CHECKBOX(-1, LboxTestFrame::OnCheckOrRadioBox)
    EVT_RADIOBOX(-1, LboxTestFrame::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// app class
// ----------------------------------------------------------------------------

bool LboxTestApp::OnInit()
{
    wxFrame *frame = new LboxTestFrame(_T("wxListBox sample"));
    frame->Show();

    //wxLog::AddTraceMask(_T("listbox"));
    wxLog::AddTraceMask(_T("scrollbar"));

    return TRUE;
}

// ----------------------------------------------------------------------------
// top level frame class
// ----------------------------------------------------------------------------

LboxTestFrame::LboxTestFrame(const wxString& title)
             : wxFrame(NULL, -1, title, wxPoint(100, 100))
{
    // init everything
    m_dirty = FALSE;
    m_radioSelMode = (wxRadioBox *)NULL;

    m_chkVScroll =
    m_chkHScroll =
    m_chkSort = (wxCheckBox *)NULL;

    m_lbox =
    m_lboxLog = (wxListBox *)NULL;
    m_sizerLbox = (wxSizer *)NULL;

    m_logTarget = (wxLog *)NULL;

    wxPanel *panel = new wxPanel(this, -1);

    /*
       What we create here is a frame having 3 panes: the explanatory pane to
       the left allowing to set the listbox styles and recreate the control,
       the pane containing the listbox itself and the lower pane containing
       the buttons which allow to add/change/delete strings to/from it.
    */
    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL),
            *sizerUp = new wxBoxSizer(wxHORIZONTAL),
            *sizerLeft,
            *sizerRight = new wxBoxSizer(wxVERTICAL);

    // upper left pane
    static const wxString modes[] =
    {
        _T("single"),
        _T("extended"),
        _T("multiple"),
    };

    wxStaticBox *box = new wxStaticBox(panel, -1, _T("&Set listbox parameters"));
    m_radioSelMode = new wxRadioBox(panel, -1, _T("Selection &mode:"),
                                    wxDefaultPosition, wxDefaultSize,
                                    WXSIZEOF(modes), modes,
                                    1, wxRA_SPECIFY_COLS);

    m_chkVScroll = new wxCheckBox(panel, -1, _T("Always show &vertical scrollbar"));
    m_chkHScroll = new wxCheckBox(panel, -1, _T("Show &horizontal scrollbar"));
    m_chkSort = new wxCheckBox(panel, -1, _T("&Sort items"));

    sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    sizerLeft->Add(m_chkVScroll, 0, wxLEFT | wxRIGHT, 5);
    sizerLeft->Add(m_chkHScroll, 0, wxLEFT | wxRIGHT, 5);
    sizerLeft->Add(m_chkSort, 0, wxLEFT | wxRIGHT, 5);
    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer
    sizerLeft->Add(m_radioSelMode, 0, wxGROW | wxALL, 5);

    wxSizer *sizerBtn = new wxBoxSizer(wxHORIZONTAL);
    wxButton *btn = new wxButton(panel, LboxTest_Reset, _T("&Reset"));
    sizerBtn->Add(btn, 0, wxLEFT | wxRIGHT, 5);
    btn = new wxButton(panel, LboxTest_Create, _T("&Create"));
    sizerBtn->Add(btn, 0, wxLEFT | wxRIGHT, 5);
    sizerLeft->Add(sizerBtn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(panel, -1, _T("&Change listbox contents"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(panel, LboxTest_Add, _T("&Add this string"));
    m_textAdd = new wxTextCtrl(panel, LboxTest_AddText, _T("test item 0"));
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textAdd, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(panel, LboxTest_AddSeveral, _T("&Insert a few strings"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    sizerRow = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(panel, LboxTest_Delete, _T("&Delete panel item"));
    m_textDelete = new wxTextCtrl(panel, LboxTest_DeleteText, _T(""));
    sizerRow->Add(btn, 0, wxRIGHT, 5);
    sizerRow->Add(m_textDelete, 1, wxLEFT, 5);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    btn = new wxButton(panel, LboxTest_DeleteSel, _T("Delete &selection"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    btn = new wxButton(panel, LboxTest_Clear, _T("&Clear"));
    sizerMiddle->Add(btn, 0, wxALL | wxGROW, 5);

    // right pane
    m_lbox = new wxListBox(panel, LboxTest_Listbox,
                           wxDefaultPosition, wxDefaultSize,
                           0, NULL,
                           wxLB_HSCROLL);
    sizerRight->Add(m_lbox, 1, wxGROW | wxALL, 5);
    sizerRight->SetMinSize(250, 0);
    m_sizerLbox = sizerRight; // save it to modify it later

    // the 3 panes panes compose the upper part of the window
    sizerUp->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerUp->Add(sizerMiddle, 1, wxGROW | wxALL, 10);
    sizerUp->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // the lower one only has the log listbox and a button to clear it
    wxSizer *sizerDown = new wxStaticBoxSizer
                             (
                               new wxStaticBox(panel, -1, _T("&Log window")),
                               wxVERTICAL
                             );
    m_lboxLog = new wxListBox(panel, -1);
    sizerDown->Add(m_lboxLog, 1, wxGROW | wxALL, 5);
    wxBoxSizer *sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    btn = new wxButton(panel, LboxTest_ClearLog, _T("Clear &log"));
    sizerBtns->Add(btn);
    sizerBtns->Add(10, 0); // spacer
    btn = new wxButton(panel, LboxTest_Quit, _T("E&xit"));
    sizerBtns->Add(btn);
    sizerDown->Add(sizerBtns, 0, wxALL | wxALIGN_RIGHT, 5);

    // put everything together
    sizerTop->Add(sizerUp, 1, wxGROW | (wxALL & ~wxBOTTOM), 10);
    sizerTop->Add(0, 5, 0, wxGROW); // spacer in between
    sizerTop->Add(sizerDown, 0,  wxGROW | (wxALL & ~wxTOP), 10);

    // final initialization
    Reset();
    m_dirty = FALSE;

    panel->SetAutoLayout(TRUE);
    panel->SetSizer(sizerTop);

    sizerTop->Fit(this);
    sizerTop->SetSizeHints(this);

    // now that everything is created we can redirect the log messages to the
    // listbox
    m_logTarget = new LboxLogger(m_lboxLog, wxLog::GetActiveTarget());
    wxLog::SetActiveTarget(m_logTarget);
}

LboxTestFrame::~LboxTestFrame()
{
    delete m_logTarget;
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void LboxTestFrame::Reset()
{
    if ( m_radioSelMode->GetSelection() == LboxSel_Single &&
         !m_chkSort->GetValue() &&
         m_chkHScroll->GetValue() &&
         !m_chkVScroll->GetValue() )
    {
        // nothing to do
        return;
    }

    m_radioSelMode->SetSelection(LboxSel_Single);
    m_chkSort->SetValue(FALSE);
    m_chkHScroll->SetValue(TRUE);
    m_chkVScroll->SetValue(FALSE);

    m_dirty = TRUE;
}

void LboxTestFrame::CreateLbox()
{
    int flags = 0;
    switch ( m_radioSelMode->GetSelection() )
    {
        default:
            wxFAIL_MSG( _T("unexpected radio box selection") );

        case LboxSel_Single:    flags |= wxLB_SINGLE; break;
        case LboxSel_Extended:  flags |= wxLB_EXTENDED; break;
        case LboxSel_Multiple:  flags |= wxLB_MULTIPLE; break;
    }

    if ( m_chkVScroll->GetValue() )
        flags |= wxLB_ALWAYS_SB;
    if ( m_chkHScroll->GetValue() )
        flags |= wxLB_HSCROLL;
    if ( m_chkSort->GetValue() )
        flags |= wxLB_SORT;

    wxArrayString items;
    if ( m_lbox )
    {
        int count = m_lbox->GetCount();
        for ( int n = 0; n < count; n++ )
        {
            items.Add(m_lbox->GetString(n));
        }

        m_sizerLbox->Remove(m_lbox);
        delete m_lbox;
    }

    m_lbox = new wxListBox(this, -1,
                           wxDefaultPosition, wxDefaultSize,
                           0, NULL,
                           flags);
    m_lbox->Set(items);
    m_sizerLbox->Add(m_lbox, 1, wxGROW | wxALL, 5);
    m_sizerLbox->Layout();

    m_dirty = FALSE;
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void LboxTestFrame::OnButtonQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void LboxTestFrame::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();
}

void LboxTestFrame::OnButtonCreate(wxCommandEvent& WXUNUSED(event))
{
    CreateLbox();
}

void LboxTestFrame::OnButtonDelete(wxCommandEvent& WXUNUSED(event))
{
    unsigned long n;
    if ( !m_textDelete->GetValue().ToULong(&n) ||
            (n >= (unsigned)m_lbox->GetCount()) )
    {
        return;
    }

    m_lbox->Delete(n);
}

void LboxTestFrame::OnButtonDeleteSel(wxCommandEvent& WXUNUSED(event))
{
    wxArrayInt selections;
    int n = m_lbox->GetSelections(selections);
    while ( n > 0 )
    {
        m_lbox->Delete(selections[--n]);
    }
}

void LboxTestFrame::OnButtonClear(wxCommandEvent& event)
{
    m_lbox->Clear();
}

void LboxTestFrame::OnButtonClearLog(wxCommandEvent& event)
{
    m_lboxLog->Clear();
}

void LboxTestFrame::OnButtonAdd(wxCommandEvent& event)
{
    static size_t s_item = 0;

    wxString s = m_textAdd->GetValue();
    if ( !m_textAdd->IsModified() )
    {
        // update the default string
        m_textAdd->SetValue(wxString::Format(_T("test item %u"), ++s_item));
    }

    m_lbox->Append(s);
}

void LboxTestFrame::OnButtonAddSeveral(wxCommandEvent& event)
{
    wxArrayString items;
    items.Add(_T("First"));
    items.Add(_T("another one"));
    items.Add(_T("and the last (very very very very very very very very very very long) one"));
    m_lbox->InsertItems(items, 0);
}

void LboxTestFrame::OnUpdateUICreateButton(wxUpdateUIEvent& event)
{
    event.Enable(m_dirty);
}

void LboxTestFrame::OnUpdateUIDeleteButton(wxUpdateUIEvent& event)
{
    unsigned long n;
    event.Enable(m_textDelete->GetValue().ToULong(&n) &&
                    (n < (unsigned)m_lbox->GetCount()));
}

void LboxTestFrame::OnUpdateUIDeleteSelButton(wxUpdateUIEvent& event)
{
    wxArrayInt selections;
    event.Enable(m_lbox->GetSelections(selections) != 0);
}

void LboxTestFrame::OnUpdateUIClearButton(wxUpdateUIEvent& event)
{
    event.Enable(m_lbox->GetCount() != 0);
}

void LboxTestFrame::OnUpdateUIAddSeveral(wxUpdateUIEvent& event)
{
    event.Enable(!(m_lbox->GetWindowStyle() & wxLB_SORT));
}

void LboxTestFrame::OnListbox(wxCommandEvent& event)
{
    int sel = event.GetInt();
    m_textDelete->SetValue(wxString::Format(_T("%ld"), sel));

    wxLogMessage(_T("Listbox item %d selected"), sel);
}

void LboxTestFrame::OnListboxDClick(wxCommandEvent& event)
{
    wxLogMessage(_T("Listbox item %d double clicked"), event.GetInt());
}

void LboxTestFrame::OnCheckOrRadioBox(wxCommandEvent& event)
{
    m_dirty = TRUE;
}

