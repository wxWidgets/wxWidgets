/////////////////////////////////////////////////////////////////////////////
// Name:        helpctrl.cpp
// Purpose:     wxHtmlHelpController
// Notes:       Based on htmlhelp.cpp, implementing a monolithic
//              HTML Help controller class,  by Vaclav Slavik
// Author:      Harm van der Heijden and Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Harm van der Heijden and Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "helpctrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_WXHTML_HELP

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
#endif // WX_PRECOMP

#include "wx/html/helpctrl.h"
#include "wx/busyinfo.h"

#ifdef __WXGTK__
    // for the hack in AddGrabIfNeeded()
    #include "wx/dialog.h"
#endif // __WXGTK__

#if wxUSE_HELP
    #include "wx/tipwin.h"
#endif


#if wxUSE_LIBMSPACK
#include "wx/html/forcelnk.h"
FORCE_LINK(wxhtml_chm_support)
#endif

IMPLEMENT_DYNAMIC_CLASS(wxHtmlHelpController, wxHelpControllerBase)

wxHtmlHelpController::wxHtmlHelpController(int style)
{
    m_helpFrame = NULL;
    m_Config = NULL;
    m_ConfigRoot = wxEmptyString;
    m_titleFormat = _("Help: %s");
    m_FrameStyle = style;
}

wxHtmlHelpController::~wxHtmlHelpController()
{
    if (m_Config)
        WriteCustomization(m_Config, m_ConfigRoot);
    if (m_helpFrame)
        DestroyHelpWindow();
}


void wxHtmlHelpController::DestroyHelpWindow()
{
    //if (m_Config) WriteCustomization(m_Config, m_ConfigRoot);
    if (m_helpFrame)
        m_helpFrame->Destroy();
}

void wxHtmlHelpController::OnCloseFrame(wxCloseEvent& evt)
{
    evt.Skip();

    OnQuit();

    m_helpFrame->SetController((wxHelpControllerBase*) NULL);
    m_helpFrame = NULL;
}

void wxHtmlHelpController::SetTitleFormat(const wxString& title)
{
    m_titleFormat = title;
    if (m_helpFrame)
        m_helpFrame->SetTitleFormat(title);
}


bool wxHtmlHelpController::AddBook(const wxFileName& book_file, bool show_wait_msg)
{
    return AddBook(wxFileSystem::FileNameToURL(book_file), show_wait_msg);
}

bool wxHtmlHelpController::AddBook(const wxString& book, bool show_wait_msg)
{
    wxBusyCursor cur;
#if wxUSE_BUSYINFO
    wxBusyInfo* busy = NULL;
    wxString info;
    if (show_wait_msg)
    {
        info.Printf(_("Adding book %s"), book.c_str());
        busy = new wxBusyInfo(info);
    }
#endif
    bool retval = m_helpData.AddBook(book);
#if wxUSE_BUSYINFO
    if (show_wait_msg)
        delete busy;
#else
    wxUnusedVar(show_wait_msg);
#endif
    if (m_helpFrame)
        m_helpFrame->RefreshLists();
    return retval;
}



wxHtmlHelpFrame *wxHtmlHelpController::CreateHelpFrame(wxHtmlHelpData *data)
{
    return new wxHtmlHelpFrame(data);
}


void wxHtmlHelpController::CreateHelpWindow()
{
    if (m_helpFrame)
    {
        m_helpFrame->Raise();
        return ;
    }

    if (m_Config == NULL)
    {
        m_Config = wxConfigBase::Get(false);
        if (m_Config != NULL)
            m_ConfigRoot = _T("wxWindows/wxHtmlHelpController");
    }

    m_helpFrame = CreateHelpFrame(&m_helpData);
    m_helpFrame->SetController(this);

    if (m_Config)
        m_helpFrame->UseConfig(m_Config, m_ConfigRoot);

    m_helpFrame->Create(NULL, wxID_HTML_HELPFRAME, wxEmptyString, m_FrameStyle);
    m_helpFrame->SetTitleFormat(m_titleFormat);

    m_helpFrame->Show(true);
}

void wxHtmlHelpController::ReadCustomization(wxConfigBase* cfg, const wxString& path)
{
    /* should not be called by the user; call UseConfig, and the controller
     * will do the rest */
    if (m_helpFrame && cfg)
        m_helpFrame->ReadCustomization(cfg, path);
}

void wxHtmlHelpController::WriteCustomization(wxConfigBase* cfg, const wxString& path)
{
    /* typically called by the controllers OnCloseFrame handler */
    if (m_helpFrame && cfg)
        m_helpFrame->WriteCustomization(cfg, path);
}

void wxHtmlHelpController::UseConfig(wxConfigBase *config, const wxString& rootpath)
{
    m_Config = config;
    m_ConfigRoot = rootpath;
    if (m_helpFrame) m_helpFrame->UseConfig(config, rootpath);
    ReadCustomization(config, rootpath);
}

//// Backward compatibility with wxHelpController API

bool wxHtmlHelpController::Initialize(const wxString& file)
{
    wxString dir, filename, ext;
    wxSplitPath(file, & dir, & filename, & ext);

    if (!dir.IsEmpty())
        dir = dir + wxFILE_SEP_PATH;

    // Try to find a suitable file
    wxString actualFilename = dir + filename + wxString(wxT(".zip"));
    if (!wxFileExists(actualFilename))
    {
        actualFilename = dir + filename + wxString(wxT(".htb"));
        if (!wxFileExists(actualFilename))
        {
            actualFilename = dir + filename + wxString(wxT(".hhp"));
            if (!wxFileExists(actualFilename))
            {
#if wxUSE_LIBMSPACK
                actualFilename = dir + filename + wxString(wxT(".chm"));
                if (!wxFileExists(actualFilename))
#endif
                    return false;
            }
        }
    }
    return AddBook(wxFileName(actualFilename));
}

bool wxHtmlHelpController::LoadFile(const wxString& WXUNUSED(file))
{
    // Don't reload the file or we'll have it appear again, presumably.
    return true;
}

bool wxHtmlHelpController::DisplaySection(int sectionNo)
{
    return Display(sectionNo);
}

bool wxHtmlHelpController::DisplayTextPopup(const wxString& text, const wxPoint& WXUNUSED(pos))
{
#if wxUSE_TIPWINDOW
    static wxTipWindow* s_tipWindow = NULL;

    if (s_tipWindow)
    {
        // Prevent s_tipWindow being nulled in OnIdle,
        // thereby removing the chance for the window to be closed by ShowHelp
        s_tipWindow->SetTipWindowPtr(NULL);
        s_tipWindow->Close();
    }
    s_tipWindow = NULL;

    if ( !text.empty() )
    {
        s_tipWindow = new wxTipWindow(wxTheApp->GetTopWindow(), text, 100, & s_tipWindow);

        return true;
    }
#endif // wxUSE_TIPWINDOW

    return false;
}

void wxHtmlHelpController::SetFrameParameters(const wxString& title,
                                   const wxSize& size,
                                   const wxPoint& pos,
                                   bool WXUNUSED(newFrameEachTime))
{
    SetTitleFormat(title);
    if (m_helpFrame)
    {
        m_helpFrame->SetSize(pos.x, pos.y, size.x, size.y);
    }
}

wxFrame* wxHtmlHelpController::GetFrameParameters(wxSize *size,
                                   wxPoint *pos,
                                   bool *newFrameEachTime)
{
    if (newFrameEachTime)
        (* newFrameEachTime) = false;
    if (size && m_helpFrame)
        (* size) = m_helpFrame->GetSize();
    if (pos && m_helpFrame)
        (* pos) = m_helpFrame->GetPosition();
    return m_helpFrame;
}

bool wxHtmlHelpController::Quit()
{
    DestroyHelpWindow();
    return true;
}

// Make the help controller's frame 'modal' if
// needed
void wxHtmlHelpController::AddGrabIfNeeded()
{
    // So far, wxGTK only
#ifdef __WXGTK__
    bool needGrab = false;

    // Check if there are any modal windows present,
    // in which case we need to add a grab.
    for ( wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow *win = node->GetData();
        wxDialog *dialog = wxDynamicCast(win, wxDialog);

        if (dialog && dialog->IsModal())
            needGrab = true;
    }

    if (needGrab && m_helpFrame)
        m_helpFrame->AddGrab();
#endif // __WXGTK__
}

bool wxHtmlHelpController::Display(const wxString& x)
{
    CreateHelpWindow();
    bool success = m_helpFrame->Display(x);
    AddGrabIfNeeded();
    return success;
}

bool wxHtmlHelpController::Display(int id)
{
    CreateHelpWindow();
    bool success = m_helpFrame->Display(id);
    AddGrabIfNeeded();
    return success;
}

bool wxHtmlHelpController::DisplayContents()
{
    CreateHelpWindow();
    bool success = m_helpFrame->DisplayContents();
    AddGrabIfNeeded();
    return success;
}

bool wxHtmlHelpController::DisplayIndex()
{
    CreateHelpWindow();
    bool success = m_helpFrame->DisplayIndex();
    AddGrabIfNeeded();
    return success;
}

bool wxHtmlHelpController::KeywordSearch(const wxString& keyword,
                                         wxHelpSearchMode mode)
{
    CreateHelpWindow();
    bool success = m_helpFrame->KeywordSearch(keyword, mode);
    AddGrabIfNeeded();
    return success;
}

#endif // wxUSE_WXHTML_HELP

