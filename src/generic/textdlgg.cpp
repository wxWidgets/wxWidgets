/////////////////////////////////////////////////////////////////////////////
// Name:        textdlgg.cpp
// Purpose:     wxTextEntryDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "textdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/stattext.h"
#include "wx/textctrl.h"
#include "wx/layout.h"
#include "wx/intl.h"
#endif

#include "wx/generic/textdlgg.h"

// wxTextEntryDialog

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxTextEntryDialog, wxDialog)
	EVT_BUTTON(wxID_OK, wxTextEntryDialog::OnOK)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxTextEntryDialog, wxDialog)
#endif

// Split message, using constraints to position controls
static void wxSplitMessage2(const char *message, wxList *messageList, wxWindow *parent, wxRowColSizer *sizer)
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

wxTextEntryDialog::wxTextEntryDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        const wxString& value, long style, const wxPoint& pos):
	wxDialog(parent, -1, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL)
{
	m_dialogStyle = style;
	m_value = value;

	wxBeginBusyCursor();

	wxSizer *topSizer = new wxSizer(this, wxSizerShrink);
	topSizer->SetBorder(10, 10);

	wxRowColSizer *messageSizer = new wxRowColSizer(topSizer, wxSIZER_COLS, 100);
	messageSizer->SetName("messageSizer");

//    bool centre = ((style & wxCENTRE) == wxCENTRE);

	wxList messageList;
	wxSplitMessage2(message, &messageList, this, messageSizer);

	// Insert a spacer
	wxSpacingSizer *spacingSizer = new wxSpacingSizer(topSizer, wxBelow, messageSizer, 10);

	wxTextCtrl *textCtrl = new wxTextCtrl(this, wxID_TEXT, value, wxPoint(-1, -1), wxSize(350, -1));

	wxRowColSizer *textSizer = new wxRowColSizer(topSizer, wxSIZER_ROWS);
	textSizer->AddSizerChild(textCtrl);
	textSizer->SetName("textSizer");

	// Create constraints for the text sizer
	wxLayoutConstraints *textC = new wxLayoutConstraints;
	textC->left.SameAs		(messageSizer, wxLeft);
	textC->top.Below		(spacingSizer);
	textSizer->SetConstraints(textC);

	// Insert another spacer
	wxSpacingSizer *spacingSizer2 = new wxSpacingSizer(topSizer, wxBelow, textSizer, 10);
	spacingSizer->SetName("spacingSizer2");

	// Insert a sizer for the buttons
	wxRowColSizer *buttonSizer = new wxRowColSizer(topSizer, wxSIZER_ROWS);
	buttonSizer->SetName("buttonSizer");

  	// Specify constraints for the button sizer
  	wxLayoutConstraints *c = new wxLayoutConstraints;
  	c->width.AsIs		();
  	c->height.AsIs		();
	c->top.Below		(spacingSizer2);
	c->centreX.SameAs	(textSizer, wxCentreX);
  	buttonSizer->SetConstraints(c);

    wxButton *ok = NULL;
  	wxButton *cancel = NULL;

  if (style & wxOK) {
    ok = new wxButton(this, wxID_OK, _("OK"));
	buttonSizer->AddSizerChild(ok);
  }

  if (style & wxCANCEL) {
    cancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
	buttonSizer->AddSizerChild(cancel);
  }

  if (ok)
    ok->SetDefault();

  Layout();
  Centre(wxBOTH);

  wxEndBusyCursor();
}

void wxTextEntryDialog::OnOK(wxCommandEvent& WXUNUSED(event) )
{
	wxTextCtrl *textCtrl = (wxTextCtrl *)FindWindow(wxID_TEXT);
	if ( textCtrl )
		m_value = textCtrl->GetValue();

	EndModal(wxID_OK);
}

