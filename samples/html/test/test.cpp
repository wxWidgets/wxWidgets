/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     wxHtml testing example
// Author:      Vaclav Slavik
// Created:     1999-07-07
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// For all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/sysopt.h"
#include "wx/html/htmlwin.h"
#include "wx/html/htmlproc.h"
#include "wx/fs_inet.h"
#include "wx/filedlg.h"
#include "wx/utils.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"

#include "../../sample.xpm"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// Define a new html window type: this is a wrapper for handling wxHtmlWindow events
class MyHtmlWindow : public wxHtmlWindow
{
public:
    MyHtmlWindow(wxWindow *parent) : wxHtmlWindow( parent ) { }

    virtual wxHtmlOpeningStatus OnOpeningURL(wxHtmlURLType WXUNUSED(type),
                                             const wxString& WXUNUSED(url),
                                             wxString *WXUNUSED(redirect)) const;

private:
    void OnClipboardEvent(wxClipboardTextEvent& event);

#if wxUSE_CLIPBOARD
    DECLARE_EVENT_TABLE()
#endif // wxUSE_CLIPBOARD
    DECLARE_NO_COPY_CLASS(MyHtmlWindow)
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnPageOpen(wxCommandEvent& event);
    void OnDefaultLocalBrowser(wxCommandEvent& event);
    void OnDefaultWebBrowser(wxCommandEvent& event);
    void OnBack(wxCommandEvent& event);
    void OnForward(wxCommandEvent& event);
    void OnProcessor(wxCommandEvent& event);

    void OnHtmlLinkClicked(wxHtmlLinkEvent& event);
    void OnHtmlCellHover(wxHtmlCellEvent &event);
    void OnHtmlCellClicked(wxHtmlCellEvent &event);

private:
    MyHtmlWindow *m_Html;
    wxHtmlProcessor *m_Processor;

    // Any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};


class BoldProcessor : public wxHtmlProcessor
{
public:
    virtual wxString Process(const wxString& s) const
    {
        wxString r(s);
        r.Replace(wxT("<b>"), wxEmptyString);
        r.Replace(wxT("<B>"), wxEmptyString);
        r.Replace(wxT("</b>"), wxEmptyString);
        r.Replace(wxT("</B>"), wxEmptyString);

        return r;
    }
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    ID_PageOpen = wxID_HIGHEST,
    ID_DefaultLocalBrowser,
    ID_DefaultWebBrowser,
    ID_Back,
    ID_Forward,
    ID_Processor
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  MyFrame::OnQuit)
    EVT_MENU(ID_PageOpen, MyFrame::OnPageOpen)
    EVT_MENU(ID_DefaultLocalBrowser, MyFrame::OnDefaultLocalBrowser)
    EVT_MENU(ID_DefaultWebBrowser, MyFrame::OnDefaultWebBrowser)
    EVT_MENU(ID_Back, MyFrame::OnBack)
    EVT_MENU(ID_Forward, MyFrame::OnForward)
    EVT_MENU(ID_Processor, MyFrame::OnProcessor)

    EVT_HTML_LINK_CLICKED(wxID_ANY, MyFrame::OnHtmlLinkClicked)
    EVT_HTML_CELL_HOVER(wxID_ANY, MyFrame::OnHtmlCellHover)
    EVT_HTML_CELL_CLICKED(wxID_ANY, MyFrame::OnHtmlCellClicked)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#if wxUSE_SYSTEM_OPTIONS
    wxSystemOptions::SetOption(wxT("no-maskblt"), 1);
#endif

    wxInitAllImageHandlers();
#if wxUSE_FS_INET && wxUSE_STREAMS && wxUSE_SOCKETS
    wxFileSystem::AddHandler(new wxInternetFSHandler);
#endif

    SetVendorName(wxT("wxWidgets"));
    SetAppName(wxT("wxHtmlTest"));
    // the following call to wxConfig::Get will use it to create an object...

    // Create the main application window
    MyFrame *frame = new MyFrame(_("wxHtmlWindow testing application"),
        wxDefaultPosition, wxSize(640, 480));

    frame->Show();

    return true /* continue running */;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
   : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size,
             wxDEFAULT_FRAME_STYLE, wxT("html_test_app"))
{
    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    wxMenu *menuNav = new wxMenu;

    menuFile->Append(ID_PageOpen, _("&Open HTML page..."));
    menuFile->Append(ID_DefaultLocalBrowser, _("&Open current page with default browser"));
    menuFile->Append(ID_DefaultWebBrowser, _("Open a &web page with default browser"));
    menuFile->AppendSeparator();
    menuFile->Append(ID_Processor, _("&Remove bold attribute"),
                     wxEmptyString, wxITEM_CHECK);

    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, _("&Close frame"));
    menuNav->Append(ID_Back, _("Go &BACK"));
    menuNav->Append(ID_Forward, _("Go &FORWARD"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(menuNav, _("&Navigate"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    SetIcon(wxIcon(sample_xpm));

#if wxUSE_ACCEL
    // Create convenient accelerators for Back and Forward navigation
    wxAcceleratorEntry entries[2];
    entries[0].Set(wxACCEL_ALT, WXK_LEFT, ID_Back);
    entries[1].Set(wxACCEL_ALT, WXK_RIGHT, ID_Forward);

    wxAcceleratorTable accel(WXSIZEOF(entries), entries);
    SetAcceleratorTable(accel);
#endif // wxUSE_ACCEL

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    m_Processor = new BoldProcessor;
    m_Processor->Enable(false);
    m_Html = new MyHtmlWindow(this);
    m_Html->SetRelatedFrame(this, _("HTML : %s"));
#if wxUSE_STATUSBAR
    m_Html->SetRelatedStatusBar(0);
#endif // wxUSE_STATUSBAR
    m_Html->ReadCustomization(wxConfig::Get());
    m_Html->LoadFile(wxFileName(wxT("test.htm")));
    m_Html->AddProcessor(m_Processor);

    wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, _T(""),
                                      wxDefaultPosition, wxDefaultSize,
                                      wxTE_MULTILINE);

    delete wxLog::SetActiveTarget(new wxLogTextCtrl(text));

    wxSizer *sz = new wxBoxSizer(wxVERTICAL);
    sz->Add(m_Html, 3, wxGROW);
    sz->Add(text, 1, wxGROW);
    SetSizer(sz);
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    m_Html->WriteCustomization(wxConfig::Get());
    delete wxConfig::Set(NULL);

    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnPageOpen(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_FILEDLG
    wxString p = wxFileSelector(_("Open HTML document"), wxEmptyString,
        wxEmptyString, wxEmptyString, wxT("HTML files|*.htm;*.html"));

    if (!p.empty())
        m_Html->LoadFile(wxFileName(p));
#endif // wxUSE_FILEDLG
}

void MyFrame::OnDefaultLocalBrowser(wxCommandEvent& WXUNUSED(event))
{
    wxString page = m_Html->GetOpenedPage();
    if (!page.empty())
    {
        wxLaunchDefaultBrowser(page);
    }
}

void MyFrame::OnDefaultWebBrowser(wxCommandEvent& WXUNUSED(event))
{
    wxString page = m_Html->GetOpenedPage();
    if (!page.empty())
    {
        wxLaunchDefaultBrowser(wxT("http://www.google.com"));
    }
}

void MyFrame::OnBack(wxCommandEvent& WXUNUSED(event))
{
    if (!m_Html->HistoryBack())
    {
        wxMessageBox(_("You reached prehistory era!"));
    }
}

void MyFrame::OnForward(wxCommandEvent& WXUNUSED(event))
{
    if (!m_Html->HistoryForward())
    {
        wxMessageBox(_("No more items in history!"));
    }
}

void MyFrame::OnProcessor(wxCommandEvent& WXUNUSED(event))
{
    m_Processor->Enable(!m_Processor->IsEnabled());
    m_Html->LoadPage(m_Html->GetOpenedPage());
}

void MyFrame::OnHtmlLinkClicked(wxHtmlLinkEvent &event)
{
    wxLogMessage(wxT("The url '%s' has been clicked!"), event.GetLinkInfo().GetHref().c_str());

    // skipping this event the default behaviour (load the clicked URL)
    // will happen...
    event.Skip();
}

void MyFrame::OnHtmlCellHover(wxHtmlCellEvent &event)
{
    wxLogMessage(wxT("Mouse moved over cell %p at %d;%d"),
                 event.GetCell(), event.GetPoint().x, event.GetPoint().y);
}

void MyFrame::OnHtmlCellClicked(wxHtmlCellEvent &event)
{
    wxLogMessage(wxT("Click over cell %p at %d;%d"),
                 event.GetCell(), event.GetPoint().x, event.GetPoint().y);

    // if we don't skip the event, OnHtmlLinkClicked won't be called!
    event.Skip();
}

wxHtmlOpeningStatus MyHtmlWindow::OnOpeningURL(wxHtmlURLType WXUNUSED(type),
                                               const wxString& url,
                                               wxString *WXUNUSED(redirect)) const
{
    GetRelatedFrame()->SetStatusText(url + _T(" lately opened"),1);
    return wxHTML_OPEN;
}

#if wxUSE_CLIPBOARD
BEGIN_EVENT_TABLE(MyHtmlWindow, wxHtmlWindow)
    EVT_TEXT_COPY(wxID_ANY, MyHtmlWindow::OnClipboardEvent)
END_EVENT_TABLE()

void MyHtmlWindow::OnClipboardEvent(wxClipboardTextEvent& WXUNUSED(event))
{
    // explicitly call wxHtmlWindow::CopySelection() method
    // and show the first 100 characters of the text copied in the status bar
    if ( CopySelection() )
    {
        wxTextDataObject data;
        if ( wxTheClipboard && wxTheClipboard->GetData(data) )
        {
            const wxString text = data.GetText();
            const size_t maxTextLength = 100;

            wxLogStatus(wxString::Format(_T("Clipboard: '%s%s'"),
                        wxString(text, maxTextLength).c_str(),
                        (text.length() > maxTextLength) ? _T("...")
                                                        : _T("")));
            return;
        }
    }

    wxLogStatus(_T("Clipboard: nothing"));
}
#endif // wxUSE_CLIPBOARD
