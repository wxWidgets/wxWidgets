/////////////////////////////////////////////////////////////////////////////
// Name:        settingsdlg.cpp
// Purpose:     Settings dialog for Frame Layout 
// Author:      Aleksandras Gluchovas
// Modified by:	
// Created:     05/11/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "settingsdlg.cpp"
#pragma interface "settingsdlg.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <stdlib.h>
#include "settingsdlg.h"

/***** Implementation for class SettingsDlg *****/

#define ID_NOTES           ( wxEVT_FIRST + 1000 )
#define ID_HINTANIM_CHECK  ( ID_NOTES  + 1      )
#define ID_RTUPDATES_CHECK ( ID_NOTES  + 2      )

BEGIN_EVENT_TABLE( SettingsDlg, wxDialog )

	EVT_BUTTON( wxID_APPLY, SettingsDlg::OnApply )
	EVT_BUTTON( ID_NOTES, SettingsDlg::OnNotes )

	EVT_CHECKBOX( ID_HINTANIM_CHECK,  SettingsDlg::OnHintAnimCheck  )
	EVT_CHECKBOX( ID_RTUPDATES_CHECK, SettingsDlg::OnRTUpdatesCheck )

END_EVENT_TABLE()

SettingsDlg::SettingsDlg( wxWindow* pParent )

	: wxDialog( pParent, -1, "Active Layout Settings...",
				wxDefaultPosition,
				wxSize( 325,585),
				wxDIALOG_MODAL | wxCAPTION )
{
	int curY        = 10;
	int lMargin     = 50;
	int lBoxMargin  = lMargin - 20;
	int checkHeight = 20;
	int labelHeight = 20;
	int boxWidth    = 260;
	int interBoxGap = 10;
	int lastItemGap = 10;

	int topY = curY;

	curY += labelHeight;

	mpRTU_Check = new wxCheckBox(  this, ID_RTUPDATES_CHECK, 
								   "&Real-time updates", 
		                           wxPoint( lMargin, curY ) );

	curY += checkHeight;

	mpOPD_Check = new wxCheckBox(  this, -1, "&Out of Pane drag", 
		                           wxPoint( lMargin, curY ) );

	curY += checkHeight;

	mpEDP_Check = new wxCheckBox(  this, -1, "&Exact docking prediction", 
		                           wxPoint( lMargin, curY ) );

	curY += checkHeight;

	mpNDF_Check = new wxCheckBox(  this, -1, "Non-destructive bar &friction", 
		                           wxPoint( lMargin, curY ) );

	curY += checkHeight;

	mpSPB_Check = new wxCheckBox(  this, -1, "&Shaded pane borders", 
		                           wxPoint( lMargin, curY ) );

	curY += checkHeight + lastItemGap;

	wxStaticBox* pDNDBox = new wxStaticBox( this, -1, "Drag && Drop settings",
								            wxPoint( lBoxMargin, topY ),
										    wxSize( boxWidth, curY - topY ) );

	curY += interBoxGap;

	////////////////////////////////////////////////////////////////////

	topY = curY;

	curY += labelHeight;

	mpHAP_Check = new wxCheckBox(  this, ID_HINTANIM_CHECK, 
								   "&Hint-Rect animation plugin", 
		                           wxPoint( lMargin, curY ) );

	curY += checkHeight;

	mpGCU_Check = new wxCheckBox(  this, -1, "\"Garbage collecting\" &Updates-Mgr.", 
		                           wxPoint( lMargin, curY ) );

	curY += checkHeight;

	mpAFP_Check = new wxCheckBox(  this, -1, "&Antiflicker plugin", 
		                           wxPoint( lMargin, curY ) );

	curY += checkHeight;

	mpCSP_Check = new wxCheckBox(  this, -1, "C&ustomization plugin", 
		                           wxPoint( lMargin, curY ) );

	curY += checkHeight + lastItemGap;

	wxStaticBox* pPBox =   new wxStaticBox( this, -1, "Plugins",
								            wxPoint( lBoxMargin, topY ),
										    wxSize( boxWidth, curY - topY ) );

	curY += interBoxGap;

	////////////////////////////////////////////////////////////////////

	wxSize fieldSz( 30,20 );
	int fieldHeight = 20;
	int fieldCapMargin = lMargin + fieldSz.x + 5;
	int fieldCapOfs = 4;

	topY = curY;

	curY += labelHeight;

	mpRWInput  = new wxTextCtrl  ( this, -1, "", 
		                       	   wxPoint( lMargin, curY ),
								   fieldSz );

	mpRWLabel = new wxStaticText ( this, -1, "Resizing sash width(height)",
								   wxPoint( fieldCapMargin, curY + fieldCapOfs ) );


	curY += fieldHeight;

	mpPTMInput  = new wxTextCtrl ( this, -1, "", 
		                       	   wxPoint( lMargin, curY ),
								   fieldSz );

	mpPTMLabel = new wxStaticText( this, -1, "Pene's top margin",
								   wxPoint( fieldCapMargin, curY + fieldCapOfs ) );


	curY += fieldHeight;


	mpPBMInput  = new wxTextCtrl ( this, -1, "",
		                       	   wxPoint( lMargin, curY ),
								   fieldSz );

	mpPBMLabel = new wxStaticText( this, -1, "Pene's bottom margin", 
								   wxPoint( fieldCapMargin, curY + fieldCapOfs ) );


	curY += fieldHeight;


	mpPLMInput  = new wxTextCtrl  ( this, -1, "", 
		                       	   wxPoint( lMargin, curY ),
								   fieldSz );

	mpPLMLabel = new wxStaticText( this, -1, "Pane's left margin",
								   wxPoint( fieldCapMargin, curY + fieldCapOfs ) );


	curY += fieldHeight;


	mpPRMInput  = new wxTextCtrl  ( this, -1, "", 
		                       	   wxPoint( lMargin, curY ),
								   fieldSz );

	mpPRMLabel = new wxStaticText( this, -1, "Pane's right margin",
								   wxPoint( fieldCapMargin, curY + fieldCapOfs ) );

	curY += fieldHeight + lastItemGap;

	wxStaticBox* pCPPBox = new wxStaticBox( this, -1, "Common Pane properties",
								            wxPoint( lBoxMargin, topY ),
										    wxSize( boxWidth, curY - topY ) );

	curY += interBoxGap;

	////////////////////////////////////////////////////////////////////

	topY = curY;

	curY += labelHeight;

	fieldSz.x = 65;
	fieldCapMargin = lMargin + fieldSz.x + 10;

	mpDCInput  = new wxTextCtrl  ( this, -1, "", 
		                       	   wxPoint( lMargin, curY ),
								   fieldSz );

	mpDCLabel = new wxStaticText ( this, -1, "Dark Color (hex-RGB)",
								   wxPoint( fieldCapMargin, curY + fieldCapOfs ) );

	curY += fieldHeight;

	mpLCInput  = new wxTextCtrl  ( this, -1, "", 
		                       	   wxPoint( lMargin, curY ),
								   fieldSz );

	mpLCLabel = new wxStaticText ( this, -1, "Light Color (hex-RGB)",
								   wxPoint( fieldCapMargin, curY + fieldCapOfs ) );

	curY += fieldHeight;

	mpGCInput  = new wxTextCtrl  ( this, -1, "", 
		                       	   wxPoint( lMargin, curY ),
								   fieldSz );

	mpGCLabel = new wxStaticText ( this, -1, "Gray Color (hex-RGB)",
								   wxPoint( fieldCapMargin, curY + fieldCapOfs ) );

	curY += fieldHeight;

	mpBCInput  = new wxTextCtrl  ( this, -1, "", 
		                       	   wxPoint( lMargin, curY ),
								   fieldSz );

	mpBCLabel = new wxStaticText ( this, -1, "Pane border Color (hex-RGB)",
								   wxPoint( fieldCapMargin, curY + fieldCapOfs ) );

	curY += fieldHeight + lastItemGap;

	wxStaticBox* pCSPBox = new wxStaticBox( this, -1, "Coluor sheme properties",
								            wxPoint( lBoxMargin, topY ),
										    wxSize( boxWidth, curY - topY ) );

	curY += interBoxGap; /*button ofs*/;

	////////////////////////////////////////////////////////////////////////////////

	int lBtnMargin = 35;
	int btnGap     = 20;
	int btnHeight  = 22;
	int btnWidth   = 70;

	wxButton* mpApplyBtn  = new wxButton( this, wxID_APPLY, "A&pply", 
										  wxPoint( lBtnMargin, curY ),
										  wxSize( btnWidth, btnHeight ) );

	wxButton* mpCancelBtn = new wxButton( this, wxID_CANCEL, "&Cancel", 
										  wxPoint( lBtnMargin + btnWidth + btnGap, curY ),
										  wxSize( btnWidth, btnHeight ) );

	wxButton* mpNotesBtn  = new wxButton( this, ID_NOTES, "&Notes...", 
										  wxPoint( lBtnMargin + 2*btnWidth + 2*btnGap, curY ),
								          wxSize( btnWidth, btnHeight ) );

	mpApplyBtn->SetDefault();
	mpApplyBtn->SetFocus();

	Center( wxBOTH );
}

void SettingsDlg::ExchangeFields( bool toDialog )
{
	mToDlg = toDialog;

	ExchgCheck( mpRTU_Check, mRealTimeUpdatesOn        );
	ExchgCheck( mpOPD_Check, mOutOfPaneDragOn          );
	ExchgCheck( mpEDP_Check, mExactDockingPredictionOn );
	ExchgCheck( mpNDF_Check, mNonDestructFrictionOn    );
	ExchgCheck( mpSPB_Check, m3DShadesOn               );
	           
	ExchgCheck( mpHAP_Check, mHintRectAnimationOn   );
	ExchgCheck( mpGCU_Check, mGCUpdatesMgrOn        );
	ExchgCheck( mpAFP_Check, mAntiflickerPluginOn   );
	ExchgCheck( mpCSP_Check, mCustomizationPluginOn );

	ExchgIntField( mpRWInput,  mSashWidth    ); 
	ExchgIntField( mpPTMInput, mTopMargin    ); 
	ExchgIntField( mpPBMInput, mBottomMargin ); 
	ExchgIntField( mpPLMInput, mLeftMargin   ); 
	ExchgIntField( mpPRMInput, mRightMargin  ); 

	ExchgColourField( mpDCInput, mDarkCol   );
	ExchgColourField( mpLCInput, mLightCol  );
	ExchgColourField( mpGCInput, mGrayCol   );
	ExchgColourField( mpBCInput, mBorderCol );
}

void SettingsDlg::OnApply( wxCommandEvent& event )
{
	ExchangeFields( FALSE );
	EndModal( wxID_APPLY );
}

void SettingsDlg::OnNotes( wxCommandEvent& event )
{
	wxMessageBox("Notes go here...(TBD)");
}

void SettingsDlg::OnRTUpdatesCheck( wxCommandEvent& event )
{
	if ( mpRTU_Check->GetValue() == TRUE )
	{
		// user probably wants to see how the real-time drag & drop
		// works -- so we "let 'im know" that animation is N/A when
		// real-time option is on

		mpHAP_Check->SetValue(FALSE);
		mpHAP_Check->Refresh();
	}
}

void SettingsDlg::OnHintAnimCheck( wxCommandEvent& event )
{
	if ( mpHAP_Check->GetValue() == TRUE )
	{
		// user probably wants to see some animation effects,
		// but he/she forgot to turn off "real-time updates"
		// setting -- so we do it for you :-)

		mpRTU_Check->SetValue(FALSE);
		mpRTU_Check->Refresh();
	}
}

void SettingsDlg::ExchgCheck( wxCheckBox* pChk, bool& value )
{
	if ( mToDlg ) pChk->SetValue( value );

			 else value = pChk->GetValue();
}

void SettingsDlg::ExchgIntField( wxTextCtrl* pFld, int& value )
{
	if ( mToDlg )
	{
		char buf[32];

		sprintf( buf, "%d", value );
		pFld->SetValue( buf );
	}
	else
	{
		wxString txt = pFld->GetLineText( 0 );
		value = atoi( txt );
	}
}

void SettingsDlg::ExchgColourField( wxTextCtrl* pFld, wxColour& value )
{
	int rgbVal;

	if ( mToDlg )
	{
		rgbVal = (  value.Red()           & 0x0000FF ) |
			     ( (value.Green() << 8  ) & 0x00FF00 ) | 
				 ( (value.Blue()  << 16 ) & 0xFF0000 );

		char buf[32];

		sprintf( buf, "0x%06X", rgbVal );

		pFld->SetValue( buf );
	}
	else
	{
		wxString txt = pFld->GetLineText( 0 );

		sscanf( txt, "0x%06X", &rgbVal );

		value.Set(   rgbVal         & 0xFF,
			       ( rgbVal >> 8  ) & 0xFF,
				   ( rgbVal >> 16 ) & 0xFF );
	}
}

bool SettingsDlg::TransferDataToWindow()
{
	ExchangeFields( TRUE );

	return TRUE;
}

bool SettingsDlg::TransferDataFromWindow()
{
	ExchangeFields( FALSE );

	return TRUE;
}

#include "controlbar.h"
#include "rowlayoutpl.h"
#include "antiflickpl.h"
#include "bardragpl.h"
#include "cbcustom.h"

#include "gcupdatesmgr.h"
#include "hintanimpl.h"

void SettingsDlg::ReadLayoutSettings( wxFrameLayout& fl )
{
	cbDockPane& pane = *fl.GetPane( wxTOP );
	cbCommonPaneProperties& props = pane.mProps;

	mRealTimeUpdatesOn        = props.mRealTimeUpdatesOn;
	mOutOfPaneDragOn          = props.mOutOfPaneDragOn;
	mExactDockingPredictionOn = props.mExactDockPredictionOn;
	mNonDestructFrictionOn	  = props.mNonDestructFirctionOn;
	m3DShadesOn	              = props.mShow3DPaneBorderOn;

	mHintRectAnimationOn   = fl.FindPlugin( CLASSINFO( cbHintAnimationPlugin       ) ) != NULL;
	mAntiflickerPluginOn   = fl.FindPlugin( CLASSINFO( cbAntiflickerPlugin         ) ) != NULL;
	mCustomizationPluginOn = fl.FindPlugin( CLASSINFO( cbSimpleCustomizationPlugin ) ) != NULL;
	mGCUpdatesMgrOn        = fl.GetUpdatesManager().GetClassInfo() 
							 == CLASSINFO( cbGCUpdatesMgr );

	mSashWidth = props.mResizeHandleSize;

	mTopMargin    = pane.mTopMargin;
	mBottomMargin = pane.mBottomMargin;
	mLeftMargin	  = pane.mLeftMargin;
	mRightMargin  = pane.mRightMargin;

	mDarkCol   = fl.mDarkPen.GetColour();
	mLightCol  = fl.mLightPen.GetColour();
	mGrayCol   = fl.mGrayPen.GetColour();
	mBorderCol = fl.mBorderPen.GetColour();
}

void SettingsDlg::ApplyLayoutSettings( wxFrameLayout& fl )
{
	cbCommonPaneProperties props;

	props.mRealTimeUpdatesOn     = mRealTimeUpdatesOn;
	props.mOutOfPaneDragOn 	     = mOutOfPaneDragOn;
	props.mExactDockPredictionOn = mExactDockingPredictionOn;
	props.mNonDestructFirctionOn = mNonDestructFrictionOn;
	props.mShow3DPaneBorderOn    = m3DShadesOn;

	props.mResizeHandleSize = mSashWidth;

	fl.SetPaneProperties( props, wxALL_PANES );

	if ( mHintRectAnimationOn   ) fl.AddPlugin   ( CLASSINFO( cbHintAnimationPlugin ) );
							 else fl.RemovePlugin( CLASSINFO( cbHintAnimationPlugin ) );

	if ( mAntiflickerPluginOn   ) fl.AddPlugin   ( CLASSINFO( cbAntiflickerPlugin ) );
						     else fl.RemovePlugin( CLASSINFO( cbAntiflickerPlugin ) );

	if ( mCustomizationPluginOn ) fl.AddPlugin   ( CLASSINFO( cbSimpleCustomizationPlugin ) );
							 else fl.RemovePlugin( CLASSINFO( cbSimpleCustomizationPlugin ) );

	// FOR NOW:: unfortunatelly, currently pane marin-information is currently
	//           placed into cbDockPane, instead of cbCommonPaneProperties

	fl.SetMargins( mTopMargin,  mBottomMargin, 
				   mLeftMargin,	mRightMargin,  wxALL_PANES );

	fl.mDarkPen.SetColour( mDarkCol );
	fl.mLightPen.SetColour( mLightCol );
	fl.mGrayPen.SetColour( mGrayCol );
	fl.mBorderPen.SetColour( mBorderCol );

	fl.RecalcLayout( TRUE );

	// NOTE:: currently it's bit tricky changing updates-manager
	//        in future, updates-manager will become a normal plugin
	//        and more convenient methods (Add/FindPlugin) will be used

	if ( mGCUpdatesMgrOn &&  
		 fl.GetUpdatesManager().GetClassInfo() != CLASSINFO( cbGCUpdatesMgr ) 
	   )

		 fl.SetUpdatesManager( new cbGCUpdatesMgr( &fl ) );
	else
	if ( !mGCUpdatesMgrOn && 
		 fl.GetUpdatesManager().GetClassInfo() == CLASSINFO( cbGCUpdatesMgr ) 
	   )

		 fl.SetUpdatesManager( new cbSimpleUpdatesMgr( &fl ) );
}
