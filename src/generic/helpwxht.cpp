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
   This class implements help via wxHTML.
   It requires the name of a directory containing the documentation
   and a file mapping numerical Section numbers to relative URLS.
*/

class wxForceHtmlFilter : public wxHtmlFilter
{
public:
   virtual wxString ReadFile(const wxFSFile& file)
      {
         wxInputStream *s = file.GetStream();
         char *src;
         wxString doc;

         if (s == NULL) return wxEmptyString;
         src = new char[s -> GetSize()+1];
         src[s -> GetSize()] = 0;
         s -> Read(src, s -> GetSize());
         doc = src;
         delete [] src;
         return doc;
      }
   
    virtual bool CanRead(const wxFSFile& file)
      {
         wxString filename = file.GetLocation();
         if(filename.Length() >= 5 &&
            (
               filename.Right(4).MakeUpper() == ".HTM" ||
               filename.Right(5).MakeUpper() == ".HTML"))
            return TRUE;
         else
            return FALSE;
      }
};

#define FRAME_WIDTH  500
#define FRAME_HEIGHT 400
#define LAYOUT_X_MARGIN 2
#define LAYOUT_Y_MARGIN 2
#define OFFSET 10
#define BUTTON_WIDTH  70
#define MAX_COMBO_ENTRIES   25

class wxHelpFrame : public wxFrame
{
public:
   wxHelpFrame(wxWindow *parent, int id, const wxString &title,
               const wxPoint &pos, const wxSize &size,
               wxHelpControllerHtml *controller);
   ~wxHelpFrame();
   void OnClose(wxCloseEvent &ev);
   void OnButton(wxCommandEvent &ev);
   bool LoadPage(const wxString &url) { return m_htmlwin->LoadPage(url); }
private:
   wxHelpControllerHtml *m_controller;
   wxHtmlWindow         *m_htmlwin;
   wxHtmlFilter         *m_filter;
   wxComboBox           *m_combo;
   long m_IdBack, m_IdFwd, m_IdContents, m_IdCombo, m_IdSearch;
   DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxHelpFrame, wxFrame)
   EVT_CLOSE(wxHelpFrame::OnClose)
   EVT_BUTTON(-1, wxHelpFrame::OnButton)
END_EVENT_TABLE()


void
wxHelpFrame::OnButton(wxCommandEvent &ev)
{
   long id =ev.GetId();

   if(id == m_IdBack)
      m_htmlwin->HistoryBack();
   else if(id == m_IdFwd)
      m_htmlwin->HistoryForward();
   else if(id == m_IdContents)
      m_controller->DisplayContents();
   else if(id == m_IdSearch)
   {
      wxString str = m_combo->GetValue();
      if(m_combo->FindString(str) == -1 && m_combo->Number() < MAX_COMBO_ENTRIES)
         m_combo->Append(str);
      m_controller->KeywordSearch(str);
   }
}

wxHelpFrame::wxHelpFrame(wxWindow *parent, int id,
                         const wxString &title,
                         const wxPoint &pos, const wxSize &size,
                         wxHelpControllerHtml *controller)
   : wxFrame(parent, id, title, pos, size)
{

   m_controller = controller;
   m_htmlwin = new wxHtmlWindow(this,-1,wxDefaultPosition,wxSize(FRAME_WIDTH,
                                                          FRAME_HEIGHT));

   m_IdBack = wxWindow::NewControlId();
   m_IdFwd = wxWindow::NewControlId();
   m_IdContents = wxWindow::NewControlId();
   m_IdCombo = wxWindow::NewControlId();
   m_IdSearch = wxWindow::NewControlId();

   wxButton *btn_back = new wxButton(this, m_IdBack, _("Back"));
   wxButton *btn_fwd = new wxButton(this, m_IdFwd, _("Forward"));
   wxButton *btn_contents = new wxButton(this, m_IdContents, _("Contents"));
   m_combo = new wxComboBox(this, m_IdCombo);
   wxButton *btn_search = new wxButton(this, m_IdSearch, _("Search"));
   
   m_filter = new wxForceHtmlFilter;

   wxLayoutConstraints *c;

   c = new wxLayoutConstraints;
   c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
   c->width.Absolute(BUTTON_WIDTH);
   c->top.SameAs(this, wxTop, 2*LAYOUT_Y_MARGIN);
   c->height.AsIs();
   btn_back->SetConstraints(c);

   c = new wxLayoutConstraints;
   c->left.SameAs(btn_back, wxRight, 2*LAYOUT_X_MARGIN);
   c->width.Absolute(BUTTON_WIDTH);
   c->top.SameAs(this, wxTop, 2*LAYOUT_Y_MARGIN);
   c->height.AsIs();
   btn_fwd->SetConstraints(c);
   
   c = new wxLayoutConstraints;
   c->left.SameAs(btn_fwd, wxRight, 2*LAYOUT_X_MARGIN);
   c->width.Absolute(BUTTON_WIDTH);
   c->top.SameAs(this, wxTop, 2*LAYOUT_Y_MARGIN);
   c->height.AsIs();
   btn_contents->SetConstraints(c);

   c = new wxLayoutConstraints;
   c->left.SameAs(btn_contents, wxRight, 2*LAYOUT_X_MARGIN);
   c->width.Absolute(3*BUTTON_WIDTH);
   c->top.SameAs(this, wxTop, 2*LAYOUT_Y_MARGIN);
   c->height.AsIs();
   m_combo->SetConstraints(c);

   c = new wxLayoutConstraints;
   c->left.SameAs(m_combo, wxRight, 2*LAYOUT_X_MARGIN);
   c->width.Absolute(BUTTON_WIDTH);
   c->top.SameAs(this, wxTop, 2*LAYOUT_Y_MARGIN);
   c->height.AsIs();
   btn_search->SetConstraints(c);


   c = new wxLayoutConstraints;
   c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
   c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
   c->top.SameAs(btn_back, wxBottom, 2*LAYOUT_Y_MARGIN);
   c->bottom.SameAs(this, wxBottom, 2*LAYOUT_Y_MARGIN);
   m_htmlwin->SetConstraints(c);
   SetAutoLayout(TRUE);
   CreateStatusBar();
 
   m_htmlwin->SetRelatedFrame(this, title);
   m_htmlwin->SetRelatedStatusBar(0);
   m_htmlwin->AddFilter(m_filter);

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

   SetFrameParameters(_("Help: %s"),
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
   m_Frame->Raise();
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

wxFrame *
wxHelpControllerHtml::GetFrameParameters(wxSize *size = NULL,
                                         wxPoint *pos = NULL,
                                         bool *newframe = NULL)
{
   if(size) *size = m_FrameSize;
   if(pos) *pos = m_FramePosition;
   if(newframe) *newframe = m_NewFrameEachTime;
   return m_Frame;
}

#endif // wxUSE_HTML
