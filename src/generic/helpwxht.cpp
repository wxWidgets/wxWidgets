/////////////////////////////////////////////////////////////////////////////
// Name:        helpext.cpp
// Purpose:     an external help controller for wxWindows
// Author:      Karsten Ballueder
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Karsten Ballueder
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#   pragma implementation "helpwxht.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#if wxUSE_HTML

#ifndef WX_PRECOMP
#   include "wx/string.h"
#   include "wx/utils.h"
#   include "wx/list.h"
#   include "wx/intl.h"
#   include "wx/layout.h"
#endif

#include "wx/helpbase.h"
#include "wx/generic/helpwxht.h"
#include "wx/html/htmlwin.h"

#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>

#ifndef __WINDOWS__
#   include   <unistd.h>
#endif

IMPLEMENT_CLASS(wxHelpControllerHtml, wxHTMLHelpControllerBase)

/**
   This class implements help via an external browser.
   It requires the name of a directory containing the documentation
   and a file mapping numerical Section numbers to relative URLS.
*/

#define FRAME_WIDTH  400
#define FRAME_HEIGHT 400
#define LAYOUT_X_MARGIN 2
#define LAYOUT_Y_MARGIN 2
#define OFFSET 10

class wxHelpFrame : public wxFrame
{
public:
   wxHelpFrame(wxWindow *parent, int id, const wxString &title,
               const wxPoint &pos, const wxSize &size,
               wxHelpControllerHtml *controller);
   ~wxHelpFrame();
   void OnClose(wxCloseEvent &ev);
   bool LoadPage(const wxString &url) { return m_htmlwin->LoadPage(url); }
private:
   wxHelpControllerHtml *m_controller;
   wxHtmlWindow         *m_htmlwin;
   DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxHelpFrame, wxFrame)
   EVT_CLOSE(wxHelpFrame::OnClose)
END_EVENT_TABLE()

wxHelpFrame::wxHelpFrame(wxWindow *parent, int id,
                         const wxString &title,
                         const wxPoint &pos, const wxSize &size,
                         wxHelpControllerHtml *controller)
   : wxFrame(parent, id, title, pos, size)
{

   m_controller = controller;
   m_htmlwin = new wxHtmlWindow(this,-1,wxDefaultPosition,wxSize(FRAME_WIDTH,
                                                          FRAME_HEIGHT));

   wxLayoutConstraints *c;

   c = new wxLayoutConstraints;
   c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
   c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
   c->top.SameAs(this, wxTop, 2*LAYOUT_Y_MARGIN);
   c->bottom.SameAs(this, wxBottom, 2*LAYOUT_Y_MARGIN);
   m_htmlwin->SetConstraints(c);
   SetAutoLayout(TRUE);
   Show(TRUE);
}

wxHelpFrame::~wxHelpFrame()
{
}

void
wxHelpFrame::OnClose(wxCloseEvent &ev)
{
   wxASSERT(m_controller);
   m_controller->m_Frame = NULL;
   bool newFrame;
   int x,y;
   GetPosition(&x,&y);

   m_controller->GetFrameParameters(NULL, NULL, &newFrame);
   m_controller->SetFrameParameters(GetTitle(), GetSize(),
                                    wxPoint(x,y),
                                    newFrame);
   Destroy();
}

wxHelpControllerHtml::wxHelpControllerHtml(void)
{
   m_Frame = NULL;
   m_offset = 0;

   SetFrameParameters(_("Help"),
                      wxSize(FRAME_WIDTH, FRAME_HEIGHT),
                      wxDefaultPosition);
}

wxHelpControllerHtml::~wxHelpControllerHtml(void)
{
   if(m_Frame && ! m_NewFrameEachTime)
      m_Frame->Close();
}


#ifdef __WXMSW__
#   define   SEP   '\\'
#else
#   define   SEP   '/'
#endif

bool
wxHelpControllerHtml::DisplayHelp(wxString const &relativeURL)
{
   wxBusyCursor b; // display a busy cursor

   wxString url;
   url << m_MapFile << SEP<< relativeURL;
   if(! m_Frame || m_NewFrameEachTime)
   {
      m_Frame = new wxHelpFrame(NULL, -1, m_FrameTitle,
                                m_FramePosition+wxPoint(m_offset,m_offset),
                                m_FrameSize,
                                this);
      if(m_NewFrameEachTime)
      {
         m_offset += OFFSET;
         if(m_offset > 200)
            m_offset = 0;
      }

   }
   return m_Frame->LoadPage(url);
}


void
wxHelpControllerHtml::SetFrameParameters(const wxString &title,
                                         const wxSize &size,
                                         const wxPoint &pos,
                                         bool newFrame)
{
   m_FrameTitle = title;
   m_FrameSize = size;
   m_FramePosition = pos;
   m_NewFrameEachTime = newFrame;
}

void
wxHelpControllerHtml::GetFrameParameters(wxSize *size = NULL,
                                         wxPoint *pos = NULL,
                                         bool *newframe = NULL)
{
   if(size) *size = m_FrameSize;
   if(pos) *pos = m_FramePosition;
   if(newframe) *newframe = m_NewFrameEachTime;
}

#endif // wxUSE_HTML
