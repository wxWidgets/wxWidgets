/////////////////////////////////////////////////////////////////////////////
// Name:        progdlgg.h
// Purpose:     wxProgressDialog class
// Author:      Karsten Ballüder
// Modified by:
// Created:     09.05.1999
// RCS-ID:      $Id$
// Copyright:   (c) Karsten Ballüder
// Licence:     wxWindows license
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
    #include "wx/settings.h"
    #include "wx/dcclient.h"
#endif

#if wxUSE_PROGRESSDLG

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
                                 int style)
{
   wxWindow *lastWindow = NULL;
   bool hasAbortButton = (style & wxPD_CAN_ABORT) != 0;
   m_state = hasAbortButton ? Continue : Uncancelable;
   m_disableParentOnly = (style & wxPD_APP_MODAL) == 0;
   m_parent = parent;
   m_maximum = maximum;

   m_elapsed = m_estimated = m_remaining = NULL;
   if ((style & (wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME)) != 0) m_time = new wxTime;
   else m_time = NULL;

   wxFrame::Create(m_parent, -1, title, wxDefaultPosition,
                   wxDefaultSize, wxDEFAULT_DIALOG_STYLE);
   SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));

   wxLayoutConstraints *c;

   wxClientDC dc(this);
   dc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
   long widthText;
   dc.GetTextExtent(message, &widthText, NULL);

   m_msg = new wxStaticText(this, -1, message);
   c = new wxLayoutConstraints;
   c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
   c->top.SameAs(this, wxTop, 2*LAYOUT_Y_MARGIN);
   c->width.AsIs();
   c->height.AsIs();
   m_msg->SetConstraints(c);
   lastWindow = m_msg;

   if ( maximum > 0 )
   {
      m_gauge = new wxGauge(this, -1, maximum,
                            wxDefaultPosition, wxDefaultSize,
                            wxGA_HORIZONTAL | wxRAISED_BORDER);
      c = new wxLayoutConstraints;
      c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
      c->top.Below(m_msg, 2*LAYOUT_Y_MARGIN);
      c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
      c->height.AsIs();
      m_gauge->SetConstraints(c);
      m_gauge->SetValue(0);
      lastWindow = m_gauge;
   }
   else
      m_gauge = (wxGauge *)NULL;

   
   if ( style & wxPD_ELAPSED_TIME )
   {
      m_elapsed = new wxStaticText(this, -1, "");
      c = new wxLayoutConstraints;
      c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
      c->top.Below(lastWindow, LAYOUT_Y_MARGIN);
      c->width.Absolute(60);
      c->height.AsIs();
      m_elapsed->SetConstraints(c);

      wxStaticText *dummy = new wxStaticText(this, -1, _T("Elapsed time : "));
      c = new wxLayoutConstraints;
      c->right.LeftOf(m_elapsed);
      c->top.SameAs(m_elapsed, wxTop, 0);
      c->width.AsIs();
      c->height.AsIs();
      dummy->SetConstraints(c);      

      lastWindow = m_elapsed;
   }

   if ( style & wxPD_ESTIMATED_TIME )
   {
      m_estimated = new wxStaticText(this, -1, "");
      c = new wxLayoutConstraints;
      c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
      c->top.Below(lastWindow, 0);
      c->width.Absolute(60);
      c->height.AsIs();
      m_estimated->SetConstraints(c);

      wxStaticText *dummy = new wxStaticText(this, -1, _T("Estimated time : "));
      c = new wxLayoutConstraints;
      c->right.LeftOf(m_estimated);
      c->top.SameAs(m_estimated, wxTop, 0);
      c->width.AsIs();
      c->height.AsIs();
      dummy->SetConstraints(c);      

      lastWindow = m_estimated;
   }

   if ( style & wxPD_REMAINING_TIME )
   {
      m_remaining = new wxStaticText(this, -1, "");
      c = new wxLayoutConstraints;
      c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
      c->top.Below(lastWindow, 0);
      c->width.Absolute(60);
      c->height.AsIs();
      m_remaining->SetConstraints(c);

      wxStaticText *dummy = new wxStaticText(this, -1, _T("Remaining time : "));
      c = new wxLayoutConstraints;
      c->right.LeftOf(m_remaining);
      c->top.SameAs(m_remaining, wxTop, 0);
      c->width.AsIs();
      c->height.AsIs();
      dummy->SetConstraints(c);      

      lastWindow = m_remaining;
   }

   if ( hasAbortButton )
   {
      m_btnAbort = new wxButton(this, -1, _("Cancel"));
      c = new wxLayoutConstraints;
      c->centreX.SameAs(this, wxCentreX);
      c->top.Below(lastWindow, 2*LAYOUT_Y_MARGIN);
      c->width.AsIs();
      c->height.AsIs();
      m_btnAbort->SetConstraints(c);
   }
   else
      m_btnAbort = (wxButton *)NULL;

   SetAutoLayout(TRUE);
   Layout();

   // calc the height of the dialog
   Fit();
   // and set the width from it - unfortunately, Fit() makes the dialog way too
   // wide under Windows, so try to find a reasonable value for the width, not
   // too big and not too small
   wxSize size = GetClientSize();
   size.x = wxMax(3*widthText/2, 2*size.y);
   SetClientSize(size);

   Show(TRUE);
   Centre(wxCENTER_FRAME | wxBOTH);

   if(m_disableParentOnly)
   {
      if(m_parent)  m_parent->Enable(FALSE);
   }
   else
      wxEnableTopLevelWindows(FALSE);

   Enable(TRUE); // enable this window
   wxYield();
}


bool
wxProgressDialog::Update(int value, const wxString& newmsg)
{
   wxASSERT_MSG( value == -1 || m_gauge, _T("cannot update non existent dialog") );
   wxASSERT_MSG( value <= m_maximum, _T("invalid progress value") );


   if( m_gauge )
      m_gauge->SetValue(value + 1);

   if( !newmsg.IsEmpty() )
      m_msg->SetLabel(newmsg);

   if ( (m_elapsed || m_remaining || m_estimated) && (value != 0) )
   {
      wxTime timenow;
      wxTime diff = timenow -  *m_time;
      unsigned long secs = diff.GetSecond() + 60 * diff.GetMinute() + 60 * 60 * diff.GetHour();
      unsigned long estim = secs * m_maximum / value; 
      unsigned long remai = estim - secs; 
      wxString s;

      if (m_elapsed) 
      {
         s.Printf(_T("%i:%02i:%02i"), diff.GetHour(), diff.GetMinute(), diff.GetSecond());
         if (s != m_elapsed->GetLabel()) m_elapsed->SetLabel(s);
      }
      if (m_estimated) 
      {
         s.Printf(_T("%i:%02i:%02i"), estim / (60 * 60), (estim / 60) % 60, estim % 60);
         if (s != m_estimated->GetLabel()) m_estimated->SetLabel(s);
      }
      if (m_remaining) 
      {
         s.Printf(_T("%i:%02i:%02i"), remai / (60 * 60), (remai / 60) % 60, remai % 60);
         if (s != m_remaining->GetLabel()) m_remaining->SetLabel(s);
      }
   }

   if ( (value == m_maximum - 1) && !(GetWindowStyleFlag() & wxPD_AUTO_HIDE) )
   {
       if ( m_btnAbort )
       {
           // tell the user what he should do...
           m_btnAbort->SetLabel(_("Close"));
       }

/*I think the default should be the other way round. If the
  application wants to set a "Done." message at the end, it should
  supply it. Any serious objections to this change? Makes the
  application programmers' work a little easier.
  if ( !newmsg )
       {
           // also provide the finishing message if the application didn't
           m_msg->SetLabel(_("Done."));
       }
*/
       m_state = Finished;

       // so that we return TRUE below
       m_state = Finished;
   }
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
   if ( m_disableParentOnly )
   {
      if(m_parent) m_parent->Enable(TRUE);
   }
   else
      wxEnableTopLevelWindows(TRUE);
   if (m_time) delete m_time;
}

#endif
