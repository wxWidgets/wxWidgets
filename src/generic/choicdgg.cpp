/////////////////////////////////////////////////////////////////////////////
// Name:        choicdgg.cpp
// Purpose:     Choice dialogs
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "choicdgg.h"
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
#include "wx/layout.h"
#include "wx/intl.h"
#endif

#include "wx/generic/choicdgg.h"

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

wxString wxGetSingleChoice( const wxString& message, const wxString& caption, int n, 
                            const wxString *choices, wxWindow *parent,
			    int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(centre), 
			    int WXUNUSED(width), int WXUNUSED(height) )
{
	wxSingleChoiceDialog dialog(parent, message, caption, n, choices);
	if ( dialog.ShowModal() == wxID_OK )
	{
		return dialog.GetStringSelection();
	}
	else
		return "";
}

// Overloaded for backward compatibility
wxString wxGetSingleChoice( const wxString& message, const wxString& caption, int n, 
                            char *choices[], wxWindow *parent,
			    int x, int y, bool centre, 
			    int width, int height )
{
	wxString *strings = new wxString[n];
	int i;
	for ( i = 0; i < n; i++)
	{
		strings[i] = choices[i];
	}
	wxString ans(wxGetSingleChoice(message, caption, n, (const wxString *)strings, parent,
		x, y, centre, width, height));
	delete[] strings;
	return ans;
}

int wxGetSingleChoiceIndex( const wxString& message, const wxString& caption, int n, 
                            const wxString *choices, wxWindow *parent,
			    int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(centre), 
			    int WXUNUSED(width), int WXUNUSED(height) )
{
	wxSingleChoiceDialog dialog(parent, message, caption, n, choices);
	if ( dialog.ShowModal() == wxID_OK )
	{
		return dialog.GetSelection();
	}
	else
		return -1;
}

// Overloaded for backward compatibility
int wxGetSingleChoiceIndex( const wxString& message, const wxString& caption, int n, 
                            char *choices[], wxWindow *parent,
			    int x, int y, bool centre, 
			    int width, int height )
{
	wxString *strings = new wxString[n];
	int i;
	for ( i = 0; i < n; i++)
	{
		strings[i] = choices[i];
	}
	int ans = wxGetSingleChoiceIndex(message, caption, n, (const wxString *)strings, parent,
		x, y, centre, width, height);
	delete[] strings;
	return ans;
}

char *wxGetSingleChoiceData( const wxString& message, const wxString& caption, int n,
                             const wxString *choices, char **client_data, wxWindow *parent,
			     int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(centre), 
			     int WXUNUSED(width), int WXUNUSED(height) )
{
	wxSingleChoiceDialog dialog(parent, message, caption, n, choices, client_data);
	if ( dialog.ShowModal() == wxID_OK )
	{
		return dialog.GetSelectionClientData();
	}
	else
		return NULL;
}

// Overloaded for backward compatibility
char *wxGetSingleChoiceData( const wxString& message, const wxString& caption, int n, 
                             char *choices[], char **client_data, wxWindow *parent,
			     int x, int y, bool centre, 
			     int width, int height )
{
	wxString *strings = new wxString[n];
	int i;
	for ( i = 0; i < n; i++)
	{
		strings[i] = choices[i];
	}
	char *data = wxGetSingleChoiceData(message, caption, n, (const wxString *)strings, client_data, parent,
		x, y, centre, width, height);
	delete[] strings;
	return data;
}


/* Multiple choice dialog contributed by Robert Cowell
 *

The new data passed are in the "int nsel" and "int * selection"

The idea is to make a multiple selection from list of strings.
The returned value is the total number selected. initialily there
are nsel selected, with indices stored in
selection[0],...,selection[nsel-1] which appear highlighted to
begin with. On exit with value i
selection[0..i-1] contains the indices of the selected items.
(Some prior selectecions might be deselected.)
Thus selection must be as big as choices, in case all items are
selected.

*/
/*
int wxGetMultipleChoice(const wxString& message, const wxString& caption,
			  int n, const wxString *choices,
			  int nsel, int * selection,
			  wxWindow *parent , int x , int y, bool centre,
			  int width, int height)
{
	return -1;
}
*/

// wxSingleChoiceDialog

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxSingleChoiceDialog, wxDialog)
	EVT_BUTTON(wxID_OK, wxSingleChoiceDialog::OnOK)
	EVT_LISTBOX_DCLICK(wxID_LISTBOX, wxSingleChoiceDialog::OnListBoxDClick)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxSingleChoiceDialog, wxDialog)
#endif

wxSingleChoiceDialog::wxSingleChoiceDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        int n, const wxString *choices, char **clientData, long style, const wxPoint& pos):
	  wxDialog(parent, -1, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL)
{
        Create(parent, message, caption, n, choices, clientData, style);
}

wxSingleChoiceDialog::wxSingleChoiceDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        const wxStringList& choices, char **clientData, long style, const wxPoint& pos):
	  wxDialog(parent, -1, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL)
{
        Create(parent, message, caption, choices, clientData, style);
}

bool wxSingleChoiceDialog::Create(wxWindow *parent, const wxString& message, const wxString& caption,
        const wxStringList& choices, char **clientData, long style, const wxPoint& pos)
{
	wxString *strings = new wxString[choices.Number()];
	int i;
	for ( i = 0; i < choices.Number(); i++)
	{
		strings[i] = (char *)choices.Nth(i)->Data();
	}
	bool ans = Create(parent, message, caption, choices.Number(), strings, clientData, style, pos);
	delete[] strings;
	return ans;
}

bool wxSingleChoiceDialog::Create( wxWindow *WXUNUSED(parent), const wxString& message, 
                                   const wxString& WXUNUSED(caption), int n, 
			           const wxString *choices, char **clientData, long style,
				   const wxPoint& WXUNUSED(pos) )
{
	m_dialogStyle = style;
	m_selection = 0;
	m_stringSelection = "";
	m_clientData = NULL;

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

	wxListBox *listBox = new wxListBox(this, wxID_LISTBOX, wxPoint(-1, -1), wxSize(240, 160),
		n, choices);
    listBox->SetSelection(m_selection);
	if ( clientData )
	{
		int i;
		for ( i = 0; i < n; i++)
		{
				listBox->SetClientData(i, clientData[i]);
		}
	}

	wxRowColSizer *listBoxSizer = new wxRowColSizer(topSizer, wxSIZER_ROWS);
	listBoxSizer->AddSizerChild(listBox);
	listBoxSizer->SetName("listBoxSizer");

	// Create constraints for the text sizer
	wxLayoutConstraints *textC = new wxLayoutConstraints;
	textC->left.SameAs		(messageSizer, wxLeft);
	textC->top.Below		(spacingSizer);
	listBoxSizer->SetConstraints(textC);

	// Insert another spacer
	wxSpacingSizer *spacingSizer2 = new wxSpacingSizer(topSizer, wxBelow, listBoxSizer, 10);
	spacingSizer->SetName("spacingSizer2");

	// Insert a sizer for the buttons
	wxRowColSizer *buttonSizer = new wxRowColSizer(topSizer, wxSIZER_ROWS);
	buttonSizer->SetName("buttonSizer");
	buttonSizer->SetSpacing(12,0);

  	// Specify constraints for the button sizer
  	wxLayoutConstraints *c = new wxLayoutConstraints;
  	c->width.AsIs		();
  	c->height.AsIs		();
	c->top.Below		(spacingSizer2);
	c->centreX.SameAs	(listBoxSizer, wxCentreX);
  	buttonSizer->SetConstraints(c);

    wxButton *ok = NULL;
  	wxButton *cancel = NULL;

  if (style & wxOK) {
    ok = new wxButton(this, wxID_OK, _("OK"), wxDefaultPosition, wxSize(75,-1) );
	buttonSizer->AddSizerChild(ok);
  }

  if (style & wxCANCEL) {
    cancel = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxSize(75,-1));
	buttonSizer->AddSizerChild(cancel);
  }

  if (ok)
  {
    ok->SetDefault();
    ok->SetFocus();
  }

  Layout();
  Centre(wxBOTH);

  wxEndBusyCursor();

  return TRUE;
}

// Set the selection
void wxSingleChoiceDialog::SetSelection(int sel)
{
	wxListBox *listBox = (wxListBox *)FindWindow(wxID_LISTBOX);
    if (listBox)
    {
        listBox->SetSelection(sel);
    }
    m_selection = sel;
}

void wxSingleChoiceDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
	wxListBox *listBox = (wxListBox *)FindWindow(wxID_LISTBOX);
	if ( listBox )
	{
		m_selection = listBox->GetSelection();
		m_stringSelection = listBox->GetStringSelection();
		m_clientData = (char*)listBox->GetClientData(m_selection);
	}

	EndModal(wxID_OK);
}

void wxSingleChoiceDialog::OnListBoxDClick(wxCommandEvent& WXUNUSED(event))
{
	wxListBox *listBox = (wxListBox *)FindWindow(wxID_LISTBOX);
	if ( listBox )
	{
		m_selection = listBox->GetSelection();
		m_stringSelection = listBox->GetStringSelection();
		m_clientData = (char*)listBox->GetClientData(m_selection);
	}

	EndModal(wxID_OK);
}


