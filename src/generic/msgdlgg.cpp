/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlgg.cpp
// Purpose:     wxGenericMessageDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "msgdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/stattext.h"
#include "wx/layout.h"
#include "wx/intl.h"
#endif

#include <stdio.h>
#include <string.h>

#include "wx/generic/msgdlgg.h"

///////////////////////////////////////////////////////////////////
// New dialog box implementations

// Split message, using constraints to position controls
void wxSplitMessage2(const char *message, wxList *messageList, wxWindow *parent, wxRowColSizer *sizer)
{
  char *copyMessage = copystring(message);
  size_t i = 0;
  size_t len = strlen(copyMessage);
  char *currentMessage = copyMessage;

//  wxWindow *lastWindow = parent;

  while (i < len) {
    while ((i < len) && (copyMessage[i] != '\n')) i++;
    if (i < len) copyMessage[i] = 0;
    wxStaticText *mess = new wxStaticText(parent, -1, currentMessage);

/*
    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->left.SameAs       	(parent, wxLeft, 10);
    c->top.SameAs        	(lastWindow, wxBottom, 5);
    c->right.AsIs      		();
    c->height.AsIs			();

    mess->SetConstraints(c);
*/
    sizer->AddSizerChild(mess);

    messageList->Append(mess);

    currentMessage = copyMessage + i + 1;
  }
  delete[] copyMessage;
}

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxGenericMessageDialog, wxDialog)
	EVT_BUTTON(wxID_YES, wxGenericMessageDialog::OnYes)
	EVT_BUTTON(wxID_NO, wxGenericMessageDialog::OnNo)
	EVT_BUTTON(wxID_CANCEL, wxGenericMessageDialog::OnCancel)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxGenericMessageDialog, wxDialog)
#endif

wxGenericMessageDialog::wxGenericMessageDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        long style, const wxPoint& pos):
	wxDialog(parent, -1, caption, pos,
#ifdef __WXMOTIF_
           wxSize(400, 300),
#else
           wxDefaultSize,
#endif
           wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL)
{
    m_dialogStyle = style;

    wxBeginBusyCursor();

    wxSizer *topSizer = new wxSizer(this, wxSizerShrink);
    topSizer->SetBorder(10, 10);

    wxRowColSizer *messageSizer = new wxRowColSizer(topSizer, wxSIZER_COLS, 100);
    messageSizer->SetName("messageSizer");

//    bool centre = ((style & wxCENTRE) == wxCENTRE);

    wxList messageList;
    wxSplitMessage2(message, &messageList, this, messageSizer);

    // Insert a spacer
    wxSpacingSizer *spacingSizer = new wxSpacingSizer(topSizer, wxBelow, messageSizer, 20);

    wxRowColSizer *buttonSizer = new wxRowColSizer(topSizer, wxSIZER_ROWS);
    buttonSizer->SetName("buttonSizer");

    // Specify constraints for the button sizer
    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->width.AsIs		();
    c->height.AsIs		();
    c->top.Below		(spacingSizer);
    c->centreX.SameAs	(spacingSizer, wxCentreX);
    buttonSizer->SetConstraints(c);

    wxButton *ok = (wxButton *) NULL;
    wxButton *cancel = (wxButton *) NULL;
    wxButton *yes = (wxButton *) NULL;
    wxButton *no = (wxButton *) NULL;

    if (style & wxYES_NO) {
       yes = new wxButton(this, wxID_YES, _("Yes"));
       no = new wxButton(this, wxID_NO, _("No"));

       buttonSizer->AddSizerChild(yes);
       buttonSizer->AddSizerChild(no);
    }

    if (style & wxOK) {
        ok = new wxButton(this, wxID_OK, _("OK"));
        buttonSizer->AddSizerChild(ok);
    }

    if (style & wxCANCEL) {
        cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
        buttonSizer->AddSizerChild(cancel);
    }

    if (ok)
    {
      ok->SetDefault();
      ok->SetFocus();
    }
    else if (yes)
    {
      yes->SetDefault();
      yes->SetFocus();
    }

    Layout();
    Centre(wxBOTH);

    wxEndBusyCursor();
}

void wxGenericMessageDialog::OnYes(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_YES);
}

void wxGenericMessageDialog::OnNo(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_NO);
}

void wxGenericMessageDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    // Allow cancellation via ESC/Close button except if
    // only YES and NO are specified.
    if ( (m_dialogStyle & wxYES_NO) != wxYES_NO || (m_dialogStyle & wxCANCEL) )
        EndModal(wxID_CANCEL);
}


int wxMessageBox(const wxString& message, const wxString& caption, long style,
                 wxWindow *parent, int WXUNUSED(x), int WXUNUSED(y) )
{
    wxMessageDialog dialog(parent, message, caption, style);

    int ans = dialog.ShowModal();
    switch ( ans )
    {
        case wxID_OK:
            return wxOK;
            break;
        case wxID_YES:
            return wxYES;
            break;
        case wxID_NO:
            return wxNO;
            break;
        default:
        case wxID_CANCEL:
            return wxCANCEL;
            break;
    }
    return ans;
}

