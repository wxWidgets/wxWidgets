/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     08/05/1999
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandars Gluchovas
// Licence:   	GNU General Public License 
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "finddlg.h"

/***** Implementation for class wxFindTextDialog *****/

//#define wxID_OK 3453453

BEGIN_EVENT_TABLE( wxFindTextDialog, wxDialog )

	// FIXME:: why OnOk() is not called??
	//EVT_BUTTON( wxID_OK, wxFindTextDialog::OnOK )

    EVT_CHAR_HOOK(wxFindTextDialog::OnKeyHook)

END_EVENT_TABLE()

wxString wxFindTextDialog::mLastExpr;
bool     wxFindTextDialog::mMatchCase = TRUE;
bool     wxFindTextDialog::mMatchWord = FALSE;
StrListT wxFindTextDialog::mExprList;  

// FIXME:: workaround for mystic. crashes wiht MSDev4.0

static wxComboBox* __gpCombo = NULL;

wxFindTextDialog::wxFindTextDialog( wxWindow* parent, const string& expr )

	: wxDialog( parent, -1, "Find",
	            wxDefaultPosition, wxSize( 335, 130 ),
				wxDIALOG_MODAL | wxCAPTION | wxTAB_TRAVERSAL |
				wxDEFAULT_DIALOG_STYLE
	          )
{
	mLastExpr = expr;

	int leftMargin = 20;
	int inputY = 20;
	int inputWidth = 200;

	wxStaticText* pStatic = 
		new wxStaticText( this, -1, "Fi&nd what:", 
						  wxPoint( leftMargin, inputY ) );

	int checkY = inputY + 25;

	mpWordCheck = new wxCheckBox( this, -1, "Match &whole word only",
							      wxPoint( leftMargin, checkY ) );

	mpCaseCheck = new wxCheckBox( this, -1, "Match &case",
							      wxPoint( leftMargin, checkY + 20 ) );

	mpCaseCheck->SetValue( mMatchCase );
	mpWordCheck->SetValue( mMatchWord );

	int btnX = inputWidth + leftMargin + 23;
	int btnY = inputY - 4;

	wxSize btnSize( 70, 25 );

	wxButton* pOkBtn = new wxButton( this, wxID_OK, "&Find", 
									 wxPoint( btnX, btnY ), btnSize );

	wxButton* pCancelBtn = new wxButton( this, wxID_CANCEL, "&Cancel", 
									     wxPoint( btnX, btnY  + 10 + btnSize.y ), btnSize );

	__gpCombo = new wxComboBox( this, -1, mLastExpr, 
					wxPoint( leftMargin + 60, inputY - 2 ), 
					wxSize( inputWidth - 50, 20 ) );

	for( size_t i = 0; i != mExprList.size(); ++i )

		__gpCombo->Append( mExprList[i] );

	pOkBtn->SetDefault();
	__gpCombo->SetFocus();

	Center( wxBOTH );
}

void wxFindTextDialog::SetExpr( const wxString& expr ) 
{ 
	mLastExpr = expr; 
	__gpCombo->SetValue( mLastExpr );

}

wxComboBox* wxFindTextDialog::GetCombo()
{
	return __gpCombo;
}

bool wxFindTextDialog::TransferDataFromWindow()
{
	mLastExpr  = GetCombo()->GetValue();
	mMatchCase = mpCaseCheck->GetValue();
	mMatchWord = mpWordCheck->GetValue();

	if ( mLastExpr != "" )
	{
		for( size_t i = 0; i != mExprList.size(); ++i )

			if ( mExprList[i] == mLastExpr )

				return TRUE;
	}

	if ( mExprList.size() > 20 ) 

		mExprList.pop_back();

	mExprList.push_back( mLastExpr );

	return TRUE;
}

void wxFindTextDialog::OnKeyHook( wxKeyEvent& event )
{
	if ( event.m_keyCode == WXK_RETURN )
	{
		TransferDataFromWindow();
		EndModal( wxID_OK );
	}
	else
		event.Skip();
}
