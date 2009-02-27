/////////////////////////////////////////////////////////////////////////////
// Name:        docmdi.cpp
// Purpose:     Frame classes for MDI document/view applications
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_MDI_ARCHITECTURE

#include "wx/docmdi.h"

/*
 * Docview MDI parent frame
 */

IMPLEMENT_CLASS(wxDocMDIParentFrame, wxMDIParentFrame)

BEGIN_EVENT_TABLE(wxDocMDIParentFrame, wxMDIParentFrame)
    EVT_MENU(wxID_EXIT, wxDocMDIParentFrame::OnExit)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, wxDocMDIParentFrame::OnMRUFile)
    EVT_CLOSE(wxDocMDIParentFrame::OnCloseWindow)
END_EVENT_TABLE()

wxDocMDIParentFrame::wxDocMDIParentFrame()
{
    Init();
}

wxDocMDIParentFrame::wxDocMDIParentFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString& title,
  const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    Init();
    Create(manager, frame, id, title, pos, size, style, name);
}

bool wxDocMDIParentFrame::Create(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString& title,
  const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_docManager = manager;
    return wxMDIParentFrame::Create(frame, id, title, pos, size, style, name);
}

void wxDocMDIParentFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void wxDocMDIParentFrame::Init()
{
    m_docManager = NULL;
}

void wxDocMDIParentFrame::OnMRUFile(wxCommandEvent& event)
{
    wxString f(m_docManager->GetHistoryFile(event.GetId() - wxID_FILE1));
    if (!f.empty())
        (void)m_docManager->CreateDocument(f, wxDOC_SILENT);
}

bool wxDocMDIParentFrame::TryBefore(wxEvent& event)
{
    if ( m_docManager && m_docManager->ProcessEventHere(event) )
        return true;

    return wxMDIParentFrame::TryBefore(event);
}

void wxDocMDIParentFrame::OnCloseWindow(wxCloseEvent& event)
{
  if (m_docManager->Clear(!event.CanVeto()))
  {
    this->Destroy();
  }
  else
    event.Veto();
}


IMPLEMENT_CLASS(wxDocMDIChildFrame, wxMDIChildFrame)

#endif // wxUSE_DOC_VIEW_ARCHITECTURE

