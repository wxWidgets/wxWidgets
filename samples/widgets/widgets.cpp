/////////////////////////////////////////////////////////////////////////////
// Program:     wxWindows Widgets Sample
// Name:        widgets.cpp
// Purpose:     Sample showing most of the simple wxWindows widgets
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
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/listbox.h"
    #include "wx/panel.h"
    #include "wx/stattext.h"
#endif

#include "wx/notebook.h"
#include "wx/sizer.h"

#include "widgets.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    Widgets_ClearLog = 100,
    Widgets_Quit
};

// ----------------------------------------------------------------------------
// our classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class WidgetsApp : public wxApp
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
class WidgetsFrame : public wxFrame
{
public:
    // ctor(s) and dtor
    WidgetsFrame(const wxString& title);
    virtual ~WidgetsFrame();

protected:
    // event handlers
    void OnButtonClearLog(wxCommandEvent& event);
    void OnButtonQuit(wxCommandEvent& event);

    // initialize the notebook: add all pages to it
    void InitNotebook();

private:
    // the panel containing everything
    wxPanel *m_panel;

    // the listbox for logging messages
    wxListBox *m_lboxLog;

    // the log target we use to redirect messages to the listbox
    wxLog *m_logTarget;

    // the notebook containing the test pages
    wxNotebook *m_notebook;

    // and the image list for it
    wxImageList *m_imaglist;

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
        //m_lbox->Disable(); -- looks ugly under MSW
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

        #ifdef __WXUNIVERSAL__
            m_lbox->AppendAndEnsureVisible(msg);
        #else // other ports don't have this method yet
            m_lbox->Append(msg);
            m_lbox->SetFirstItem(m_lbox->GetCount() - 1);
        #endif
    }

    // the control we use
    wxListBox *m_lbox;

    // the old log target
    wxLog *m_logOld;
};

// array of pages
WX_DEFINE_ARRAY(WidgetsPage *, ArrayWidgetsPage);

// ----------------------------------------------------------------------------
// misc macros
// ----------------------------------------------------------------------------

IMPLEMENT_APP(WidgetsApp)

#ifdef __WXUNIVERSAL__
    WX_USE_THEME(win32);
    WX_USE_THEME(gtk);
#endif // __WXUNIVERSAL__

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(WidgetsFrame, wxFrame)
    EVT_BUTTON(Widgets_ClearLog, WidgetsFrame::OnButtonClearLog)
    EVT_BUTTON(Widgets_Quit, WidgetsFrame::OnButtonQuit)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// app class
// ----------------------------------------------------------------------------

bool WidgetsApp::OnInit()
{
    wxFrame *frame = new WidgetsFrame(_T("wxWindows widgets demo"));
    frame->Show();

    //wxLog::AddTraceMask(_T("listbox"));
    //wxLog::AddTraceMask(_T("scrollbar"));

    return TRUE;
}

// ----------------------------------------------------------------------------
// WidgetsFrame construction
// ----------------------------------------------------------------------------

WidgetsFrame::WidgetsFrame(const wxString& title)
            : wxFrame(NULL, -1, title, wxPoint(100, 100))
{
    // init everything
    m_lboxLog = (wxListBox *)NULL;
    m_logTarget = (wxLog *)NULL;
    m_notebook = (wxNotebook *)NULL;
    m_imaglist = (wxImageList *)NULL;

    // create controls
    m_panel = new wxPanel(this, -1);

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    // we have 2 panes: notebook which pages demonstrating the controls in the
    // upper one and the log window with some buttons in the lower

    m_notebook = new wxNotebook(m_panel, -1);
    InitNotebook();
    wxSizer *sizerUp = new wxNotebookSizer(m_notebook);

    // the lower one only has the log listbox and a button to clear it
    wxSizer *sizerDown = new wxStaticBoxSizer
                             (
                               new wxStaticBox(m_panel, -1, _T("&Log window")),
                               wxVERTICAL
                             );
    m_lboxLog = new wxListBox(m_panel, -1);
    sizerDown->Add(m_lboxLog, 1, wxGROW | wxALL, 5);
    wxBoxSizer *sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    wxButton *btn = new wxButton(m_panel, Widgets_ClearLog, _T("Clear &log"));
    sizerBtns->Add(btn);
    sizerBtns->Add(10, 0); // spacer
    btn = new wxButton(m_panel, Widgets_Quit, _T("E&xit"));
    sizerBtns->Add(btn);
    sizerDown->Add(sizerBtns, 0, wxALL | wxALIGN_RIGHT, 5);

    // put everything together
    sizerTop->Add(sizerUp, 1, wxGROW | (wxALL & ~(wxTOP | wxBOTTOM)), 10);
    sizerTop->Add(0, 5, 0, wxGROW); // spacer in between
    sizerTop->Add(sizerDown, 0,  wxGROW | (wxALL & ~wxTOP), 10);

    m_panel->SetAutoLayout(TRUE);
    m_panel->SetSizer(sizerTop);

    sizerTop->Fit(this);
    sizerTop->SetSizeHints(this);

    // now that everything is created we can redirect the log messages to the
    // listbox
    m_logTarget = new LboxLogger(m_lboxLog, wxLog::GetActiveTarget());
    wxLog::SetActiveTarget(m_logTarget);
}

void WidgetsFrame::InitNotebook()
{
    m_imaglist = new wxImageList(32, 32);

    ArrayWidgetsPage pages;
    wxArrayString labels;

    // we need to first create all pages and only then add them to the notebook
    // as we need the image list first
    WidgetsPageInfo *info = WidgetsPage::ms_widgetPages;
    while ( info )
    {
        WidgetsPage *page = (*info->GetCtor())(m_notebook, m_imaglist);
        pages.Add(page);

        labels.Add(info->GetLabel());

        info = info->GetNext();
    }

    m_notebook->SetImageList(m_imaglist);

    // now do add them
    size_t count = pages.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_notebook->AddPage(
                            pages[n],
                            labels[n],
                            FALSE, // don't select
                            n // image id
                           );
    }
}

WidgetsFrame::~WidgetsFrame()
{
    delete m_logTarget;
    delete m_imaglist;
}

// ----------------------------------------------------------------------------
// WidgetsFrame event handlers
// ----------------------------------------------------------------------------

void WidgetsFrame::OnButtonQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void WidgetsFrame::OnButtonClearLog(wxCommandEvent& event)
{
    m_lboxLog->Clear();
}

// ----------------------------------------------------------------------------
// WidgetsPage and WidgetsPageInfo
// ----------------------------------------------------------------------------

WidgetsPageInfo *WidgetsPage::ms_widgetPages = NULL;

WidgetsPage::WidgetsPage(wxNotebook *notebook)
           : wxPanel(notebook, -1)
{
}

WidgetsPageInfo::WidgetsPageInfo(Constructor ctor, const wxChar *label)
               : m_label(label)
{
    m_ctor = ctor;

    m_next = WidgetsPage::ms_widgetPages;
    WidgetsPage::ms_widgetPages = this;
}
