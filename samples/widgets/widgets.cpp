/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        widgets.cpp
// Purpose:     Sample showing most of the simple wxWidgets widgets
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
    #include "wx/log.h"
    #include "wx/frame.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/listbox.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
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
#if wxUSE_LOG
    void OnButtonClearLog(wxCommandEvent& event);
#endif // wxUSE_LOG
    void OnButtonQuit(wxCommandEvent& event);

    // initialize the notebook: add all pages to it
    void InitNotebook();

private:
    // the panel containing everything
    wxPanel *m_panel;

#if wxUSE_LOG
    // the listbox for logging messages
    wxListBox *m_lboxLog;

    // the log target we use to redirect messages to the listbox
    wxLog *m_logTarget;
#endif // wxUSE_LOG

    // the notebook containing the test pages
    wxNotebook *m_notebook;

    // and the image list for it
    wxImageList *m_imaglist;

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

#if wxUSE_LOG
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

    virtual void DoLogString(const wxChar *szString, time_t WXUNUSED(t))
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
#endif // wxUSE_LOG

// array of pages
WX_DEFINE_ARRAY_PTR(WidgetsPage *, ArrayWidgetsPage);

// ----------------------------------------------------------------------------
// misc macros
// ----------------------------------------------------------------------------

IMPLEMENT_APP(WidgetsApp)

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(WidgetsFrame, wxFrame)
#if wxUSE_LOG
    EVT_BUTTON(Widgets_ClearLog, WidgetsFrame::OnButtonClearLog)
#endif // wxUSE_LOG
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
    if ( !wxApp::OnInit() )
        return false;

    // the reason for having these ifdef's is that I often run two copies of
    // this sample side by side and it is useful to see which one is which
    wxString title;
#if defined(__WXUNIVERSAL__)
    title = _T("wxUniv/");
#endif

#if defined(__WXMSW__)
    title += _T("wxMSW");
#elif defined(__WXGTK__)
    title += _T("wxGTK");
#elif defined(__WXMAC__)
    title += _T("wxMAC");
#elif defined(__WXMOTIF__)
    title += _T("wxMOTIF");
#else
    title += _T("wxWidgets");
#endif

    wxFrame *frame = new WidgetsFrame(title + _T(" widgets demo"));
    frame->Show();

    //wxLog::AddTraceMask(_T("listbox"));
    //wxLog::AddTraceMask(_T("scrollbar"));
    //wxLog::AddTraceMask(_T("focus"));

    return true;
}

// ----------------------------------------------------------------------------
// WidgetsFrame construction
// ----------------------------------------------------------------------------

WidgetsFrame::WidgetsFrame(const wxString& title)
            : wxFrame(NULL, wxID_ANY, title,
                      wxPoint(0, 50), wxDefaultSize,
                      wxDEFAULT_FRAME_STYLE |
                      wxNO_FULL_REPAINT_ON_RESIZE |
                      wxCLIP_CHILDREN |
                      wxTAB_TRAVERSAL)
{
    // init everything
#if wxUSE_LOG
    m_lboxLog = (wxListBox *)NULL;
    m_logTarget = (wxLog *)NULL;
#endif // wxUSE_LOG
    m_notebook = (wxNotebook *)NULL;
    m_imaglist = (wxImageList *)NULL;

    // create controls
    m_panel = new wxPanel(this, wxID_ANY,
        wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    // we have 2 panes: notebook which pages demonstrating the controls in the
    // upper one and the log window with some buttons in the lower

    m_notebook = new wxNotebook(m_panel, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN);
    InitNotebook();

    // the lower one only has the log listbox and a button to clear it
#if wxUSE_LOG
    wxSizer *sizerDown = new wxStaticBoxSizer(
        new wxStaticBox( m_panel, wxID_ANY, _T("&Log window") ),
        wxVERTICAL);

    m_lboxLog = new wxListBox(m_panel, wxID_ANY);
    sizerDown->Add(m_lboxLog, 1, wxGROW | wxALL, 5);
    sizerDown->SetMinSize(100, 150);
#else
    wxSizer *sizerDown = new wxBoxSizer(wxVERTICAL);
#endif // wxUSE_LOG

    wxBoxSizer *sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    wxButton *btn;
#if wxUSE_LOG
    btn = new wxButton(m_panel, Widgets_ClearLog, _T("Clear &log"));
    sizerBtns->Add(btn);
    sizerBtns->Add(10, 0); // spacer
#endif // wxUSE_LOG
    btn = new wxButton(m_panel, Widgets_Quit, _T("E&xit"));
    sizerBtns->Add(btn);
    sizerDown->Add(sizerBtns, 0, wxALL | wxALIGN_RIGHT, 5);

    // put everything together
    sizerTop->Add(m_notebook, 1, wxGROW | (wxALL & ~(wxTOP | wxBOTTOM)), 10);
    sizerTop->Add(0, 5, 0, wxGROW); // spacer in between
    sizerTop->Add(sizerDown, 0,  wxGROW | (wxALL & ~wxTOP), 10);

    m_panel->SetSizer(sizerTop);

    sizerTop->Fit(this);
    sizerTop->SetSizeHints(this);

#if wxUSE_LOG && !defined(__WXCOCOA__)
    // wxCocoa's listbox is too flakey to use for logging right now
    // now that everything is created we can redirect the log messages to the
    // listbox
    m_logTarget = new LboxLogger(m_lboxLog, wxLog::GetActiveTarget());
    wxLog::SetActiveTarget(m_logTarget);
#endif
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
                            false, // don't select
                            n // image id
                           );
    }
}

WidgetsFrame::~WidgetsFrame()
{
#if wxUSE_LOG
    delete m_logTarget;
#endif // wxUSE_LOG
    delete m_imaglist;
}

// ----------------------------------------------------------------------------
// WidgetsFrame event handlers
// ----------------------------------------------------------------------------

void WidgetsFrame::OnButtonQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

#if wxUSE_LOG
void WidgetsFrame::OnButtonClearLog(wxCommandEvent& WXUNUSED(event))
{
    m_lboxLog->Clear();
}
#endif // wxUSE_LOG

// ----------------------------------------------------------------------------
// WidgetsPageInfo
// ----------------------------------------------------------------------------

WidgetsPageInfo *WidgetsPage::ms_widgetPages = NULL;

WidgetsPageInfo::WidgetsPageInfo(Constructor ctor, const wxChar *label)
               : m_label(label)
{
    m_ctor = ctor;

    m_next = NULL;

    // dummy sorting: add and immediately sort on list according to label

    if(WidgetsPage::ms_widgetPages)
    {
        WidgetsPageInfo *node_prev = WidgetsPage::ms_widgetPages;
        if(wxStrcmp(label,node_prev->GetLabel().c_str())<0)
        {
            // add as first
            m_next = node_prev;
            WidgetsPage::ms_widgetPages = this;
        }
        else
        {
            WidgetsPageInfo *node_next;
            do
            {
                node_next = node_prev->GetNext();
                if(node_next)
                {
                    // add if between two
                    if(wxStrcmp(label,node_next->GetLabel().c_str())<0)
                    {
                        node_prev->SetNext(this);
                        m_next = node_next;
                        // force to break loop
                        node_next = NULL;
                    }
                }
                else
                {
                    // add as last
                    node_prev->SetNext(this);
                    m_next = node_next;
                }
                node_prev = node_next;
            }while(node_next);
        }
    }
    else
    {
        // add when first

        WidgetsPage::ms_widgetPages = this;

    }

}

// ----------------------------------------------------------------------------
// WidgetsPage
// ----------------------------------------------------------------------------

WidgetsPage::WidgetsPage(wxNotebook *notebook)
           : wxPanel(notebook, wxID_ANY,
                     wxDefaultPosition, wxDefaultSize,
                     wxNO_FULL_REPAINT_ON_RESIZE |
                     wxCLIP_CHILDREN |
                     wxTAB_TRAVERSAL)
{
}

wxSizer *WidgetsPage::CreateSizerWithText(wxControl *control,
                                          wxWindowID id,
                                          wxTextCtrl **ppText)
{
    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    wxTextCtrl *text = new wxTextCtrl(this, id, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    sizerRow->Add(control, 0, wxRIGHT | wxALIGN_CENTRE_VERTICAL, 5);
    sizerRow->Add(text, 1, wxLEFT | wxALIGN_CENTRE_VERTICAL, 5);

    if ( ppText )
        *ppText = text;

    return sizerRow;
}

// create a sizer containing a label and a text ctrl
wxSizer *WidgetsPage::CreateSizerWithTextAndLabel(const wxString& label,
                                                  wxWindowID id,
                                                  wxTextCtrl **ppText)
{
    return CreateSizerWithText(new wxStaticText(this, wxID_ANY, label),
        id, ppText);
}

// create a sizer containing a button and a text ctrl
wxSizer *WidgetsPage::CreateSizerWithTextAndButton(wxWindowID idBtn,
                                                   const wxString& label,
                                                   wxWindowID id,
                                                   wxTextCtrl **ppText)
{
    return CreateSizerWithText(new wxButton(this, idBtn, label), id, ppText);
}

wxCheckBox *WidgetsPage::CreateCheckBoxAndAddToSizer(wxSizer *sizer,
                                                     const wxString& label,
                                                     wxWindowID id)
{
    wxCheckBox *checkbox = new wxCheckBox(this, id, label);
    sizer->Add(checkbox, 0, wxLEFT | wxRIGHT, 5);
    sizer->Add(0, 2, 0, wxGROW); // spacer

    return checkbox;
}

