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

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_HTML

#include "wx/html/helpctrl.h"
#include "wx/wx.h"
#include "wx/busyinfo.h"

IMPLEMENT_DYNAMIC_CLASS(wxHtmlHelpController, wxEvtHandler)

BEGIN_EVENT_TABLE(wxHtmlHelpController, wxEvtHandler)
    EVT_CLOSE(wxHtmlHelpController::OnCloseFrame)
END_EVENT_TABLE()

wxHtmlHelpController::wxHtmlHelpController()
{
    m_helpFrame = NULL;
    m_Config = NULL;
    m_ConfigRoot = wxEmptyString;
    m_titleFormat = _("Help: %s");
}

wxHtmlHelpController::~wxHtmlHelpController()
{
    WriteCustomization(m_Config, m_ConfigRoot);
    if (m_helpFrame)
	m_helpFrame->Close();
}

void wxHtmlHelpController::SetTitleFormat(const wxString& title)
{
    m_titleFormat = title;
    if (m_helpFrame)
	m_helpFrame->SetTitleFormat(title);
}

bool wxHtmlHelpController::AddBook(const wxString& book, bool show_wait_msg)
{
    wxBusyCursor cur;
#if wxUSE_BUSYINFO
    wxBusyInfo* busy = NULL;
    wxString info;
    if (show_wait_msg) {
	info.Printf(_("Adding book %s"), book.c_str());
	busy = new wxBusyInfo(info);
    }
#endif
    bool retval = m_helpData.AddBook(book);
#if wxUSE_BUSYINFO
    if (show_wait_msg)
	delete busy;
#endif
    return retval;
}

void wxHtmlHelpController::CreateHelpWindow(bool show_progress)
{
    if (m_helpFrame) {
	m_helpFrame->Raise();
	return;
    }
    m_helpFrame = new wxHtmlHelpFrame(&m_helpData);
    m_helpFrame->PushEventHandler(this);
    if (m_Config)
	m_helpFrame->UseConfig(m_Config, m_ConfigRoot);
    m_helpFrame->Create(NULL, wxID_HTML_HELPFRAME);
    m_helpFrame->RefreshLists(show_progress);
    m_helpFrame->SetTitleFormat(m_titleFormat);
    m_helpFrame->Show(TRUE);
}

void wxHtmlHelpController::ReadCustomization(wxConfigBase* cfg, const wxString& path)
{
    /* should not be called by the user; call UseConfig, and the controller
     * will do the rest */
    if (m_helpFrame)
	m_helpFrame->ReadCustomization(cfg, path);
}

void wxHtmlHelpController::WriteCustomization(wxConfigBase* cfg, const wxString& path)
{
    /* typically called by the controllers OnCloseFrame handler */
    if (m_helpFrame)
	m_helpFrame->WriteCustomization(cfg, path);
}

#endif
