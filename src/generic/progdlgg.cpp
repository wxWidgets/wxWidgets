/////////////////////////////////////////////////////////////////////////////
// Name:        progdlgg.h
// Purpose:     wxProgressDialog class
// Author:      Karsten Ballüder
// Modified by:
// Created:     09.05.1999
// RCS-ID:      $Id$
// Copyright:   (c) Karsten Ballüder
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "progdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/frame.h"
#include "wx/button.h"
#include "wx/stattext.h"
#include "wx/layout.h"
#include "wx/event.h"
#include "wx/gauge.h"
#include "wx/intl.h"
#endif

#include "wx/generic/progdlgg.h"

#define LAYOUT_X_MARGIN 8
#define LAYOUT_Y_MARGIN 8

// wxTextEntryDialog

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxProgressDialog, wxFrame)
   EVT_BUTTON(-1, wxProgressDialog::OnCancel)
   EVT_CLOSE(wxProgressDialog::OnClose)
END_EVENT_TABLE()

   IMPLEMENT_CLASS(wxProgressDialog, wxFrame)
#endif

wxProgressDialog::wxProgressDialog(wxString const &title,
                                 wxString const &message,
                                 int maximum,
                                 wxWindow *parent,
                                 bool parentOnly,
                                 bool abortButton)
{
   m_state = abortButton ? Continue : Uncancelable;
   m_disableParentOnly = parentOnly;
   m_parent = parent;
   
   int height = 70;     // FIXME arbitrary numbers
   if ( abortButton )
      height += 35;
   wxFrame::Create(m_parent, -1, title,
                   wxPoint(0, 0), wxSize(220, height),
                   wxDEFAULT_DIALOG_STYLE);

   wxLayoutConstraints *c;

   m_msg = new wxStaticText(this, -1, message);
   c = new wxLayoutConstraints;
   c->left.SameAs(this, wxLeft, 10);
   c->top.SameAs(this, wxTop, 10);
   c->width.AsIs();
   c->height.AsIs();
   m_msg->SetConstraints(c);

   if(maximum > 0)
   {
      m_gauge = new wxGauge(this, -1, maximum);
      c = new wxLayoutConstraints;
      c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
      c->top.Below(m_msg, 2*LAYOUT_Y_MARGIN);
      c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
      c->height.AsIs();
      m_gauge->SetConstraints(c);
      m_gauge->SetValue(0);
   }
   else
      m_gauge = NULL;
   
   if ( abortButton )
   {
      wxControl *ctrl = new wxButton(this, -1, _("Cancel"));
      c = new wxLayoutConstraints;
      c->centreX.SameAs(this, wxCentreX);
      if(m_gauge)
         c->top.Below(m_gauge, 2*LAYOUT_Y_MARGIN);
      else
         c->top.Below(ctrl, 2*LAYOUT_Y_MARGIN);
      c->width.AsIs();
      c->height.AsIs();
      ctrl->SetConstraints(c);
   }

   SetAutoLayout(TRUE);
   Show(TRUE);
   Centre(wxCENTER_FRAME | wxBOTH);

   if(m_disableParentOnly)
      m_parent->Enable(false);
   else
      wxEnableTopLevelWindows(false);
   Enable(true); // enable this window
   wxYield();
}


bool
wxProgressDialog::Update(int value, const wxString& newmsg)
{
   wxASSERT(value == -1 || m_gauge);
   if(m_gauge)
      m_gauge->SetValue(value);
   if(!newmsg.IsNull())
      m_msg->SetLabel(newmsg);
   wxYield();
   return m_state != Canceled;
}

void wxProgressDialog::OnClose(wxCloseEvent& event)
{
   if ( m_state == Uncancelable )
      event.Veto(TRUE);
   else
      m_state = Canceled;
}


wxProgressDialog::~wxProgressDialog()
{
   if(m_disableParentOnly)
      m_parent->Enable(true);
   else
      wxEnableTopLevelWindows(true);
}
