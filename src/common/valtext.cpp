/////////////////////////////////////////////////////////////////////////////
// Name:        valtext.cpp
// Purpose:     wxTextValidator
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "valtext.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/textctrl.h"
#include "wx/utils.h"
#include "wx/msgbxdlg.h"
#endif

#include "wx/valtext.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxTextValidator, wxValidator)

BEGIN_EVENT_TABLE(wxTextValidator, wxValidator)
	EVT_CHAR(wxTextValidator::OnChar)
END_EVENT_TABLE()
#endif

wxTextValidator::wxTextValidator(long style, wxString *val)
{
	m_validatorStyle = style ;
	m_stringValue = val ;
/*
    m_refData = new wxVTextRefData;

	M_VTEXTDATA->m_validatorStyle = style ;
	M_VTEXTDATA->m_stringValue = val ;
*/
}

wxTextValidator::wxTextValidator(const wxTextValidator& val)
{
    Copy(val);
}

bool wxTextValidator::Copy(const wxTextValidator& val)
{
    wxValidator::Copy(val);

	m_validatorStyle = val.m_validatorStyle ;
	m_stringValue = val.m_stringValue ;

	wxNode *node = val.m_includeList.First() ;
	while ( node )
	{
		char *s = (char *)node->Data();
		m_includeList.Add(s);
		node = node->Next();
	}
	node = val.m_excludeList.First() ;
	while ( node )
	{
		char *s = (char *)node->Data();
		m_excludeList.Add(s);
		node = node->Next();
	}
    return TRUE;
}

wxTextValidator::~wxTextValidator()
{
}

static bool wxIsAlpha(const wxString& val)
{
	int i;
	for ( i = 0; i < (int)val.Length(); i++)
	{
		if (!isalpha(val[i]))
			return FALSE;
	}
	return TRUE;
}

static bool wxIsAlphaNumeric(const wxString& val)
{
	int i;
	for ( i = 0; i < (int)val.Length(); i++)
	{
		if (!isalnum(val[i]))
			return FALSE;
	}
	return TRUE;
}

// Called when the value in the window must be validated.
// This function can pop up an error message.
bool wxTextValidator::Validate(wxWindow *parent)
{
	if ( !m_validatorWindow )
		return FALSE;
	if ( !m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)) )
		return FALSE;
	if ( !m_stringValue )
		return FALSE;

	wxTextCtrl *control = (wxTextCtrl *) m_validatorWindow ;

	// If window is disabled, don't validate
	if ( !control->Enabled() )
		return FALSE;

	wxString val(control->GetValue());

	if ( m_validatorStyle & wxFILTER_INCLUDE_LIST )
	{
		if ( !m_includeList.Member(val) )
		{
			char buf[512];
			sprintf(buf, _("%s is invalid."), (const char *)val);
			wxMessageBox(buf, _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
			return FALSE;
		}
	}
	if ( m_validatorStyle & wxFILTER_EXCLUDE_LIST )
	{
		if ( m_excludeList.Member(val) )
		{
			char buf[512];
			sprintf(buf, _("%s is invalid."), (const char *)val);
			wxMessageBox(buf, _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
			return FALSE;
		}
	}
	if ( (m_validatorStyle & wxFILTER_ASCII) && !val.IsAscii() )
	{
			char buf[512];
			sprintf(buf, _("%s should only contain ASCII characters."), (const char *)val);
			wxMessageBox(buf, _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
			return FALSE;
	}
	if ( (m_validatorStyle & wxFILTER_ALPHA) && !wxIsAlpha(val) )
	{
			char buf[512];
			sprintf(buf, _("%s should only contain alphabetic characters."), (const char *)val);
			wxMessageBox(buf, _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
			return FALSE;
	}
	if ( (m_validatorStyle & wxFILTER_ALPHANUMERIC) && !wxIsAlphaNumeric(val))
	{
			char buf[512];
			sprintf(buf, _("%s should only contain alphabetic or numeric characters."), (const char *)val);
			wxMessageBox(buf,_("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
			return FALSE;
	}
	if ( (m_validatorStyle & wxFILTER_NUMERIC) && !val.IsNumber())
	{
			char buf[512];
			sprintf(buf, _("%s should be numeric."), (const char *)val);
			wxMessageBox(buf, _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
			return FALSE;
	}

	return TRUE ;
}

// Called to transfer data to the window
bool wxTextValidator::TransferToWindow(void)
{
	if ( !m_validatorWindow )
		return FALSE;
	if ( !m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)) )
		return FALSE;
	if ( !m_stringValue )
		return FALSE;

	wxTextCtrl *control = (wxTextCtrl *) m_validatorWindow ;
	control->SetValue(* m_stringValue) ;

	return TRUE;
}

// Called to transfer data to the window
bool wxTextValidator::TransferFromWindow(void)
{
	if ( !m_validatorWindow )
		return FALSE;
	if ( !m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)) )
		return FALSE;
	if ( !m_stringValue )
		return FALSE;

	wxTextCtrl *control = (wxTextCtrl *) m_validatorWindow ;
	* m_stringValue = control->GetValue() ;

	return TRUE;
}

void wxTextValidator::SetIncludeList(const wxStringList& list)
{
/*
	if ( !M_VTEXTDATA )
		return;
*/

	m_includeList.Clear();
	// TODO: replace with =
	wxNode *node = list.First() ;
	while ( node )
	{
		char *s = (char *)node->Data();
		m_includeList.Add(s);
		node = node->Next();
	}
}

void wxTextValidator::SetExcludeList(const wxStringList& list)
{
/*
	if ( !M_VTEXTDATA )
		return;
*/

	m_excludeList.Clear();
	// TODO: replace with =
	wxNode *node = list.First() ;
	while ( node )
	{
		char *s = (char *)node->Data();
		m_excludeList.Add(s);
		node = node->Next();
	}
}

void wxTextValidator::OnChar(wxKeyEvent& event)
{
/*
	if ( !M_VTEXTDATA )
		return;
*/

	if ( !m_validatorWindow )
		return;

	wxTextCtrl *textCtrl = (wxTextCtrl *)m_validatorWindow;

	int keyCode = event.KeyCode();
	if ( keyCode == WXK_DELETE || keyCode == WXK_RETURN || keyCode == WXK_BACK)
	{
		textCtrl->wxTextCtrl::OnChar(event);
		return ;
	}

	if ( (m_validatorStyle & wxFILTER_ASCII) && !isascii(keyCode) )
	{
		wxBell();
		return;
	}
	if ( (m_validatorStyle & wxFILTER_ALPHA) && !isalpha(keyCode) )
	{
		wxBell();
		return;
	}
	if ( (m_validatorStyle & wxFILTER_ALPHANUMERIC) && !isalnum(keyCode) )
	{
		wxBell();
		return;
	}
	if ( (m_validatorStyle & wxFILTER_NUMERIC) && !isdigit(keyCode) && keyCode != '.' )
	{
		wxBell();
		return;
	}

	textCtrl->wxTextCtrl::OnChar(event);
}


