#ifndef __SETTINGSDLG_G__
#define __SETTINGSDLG_G__

#include "wx/dialog.h"

class wxFrameLayout;

class SettingsDlg : public wxDialog
{
protected:

	// "nice thing" about wxWindows:

	wxCheckBox* mpRTU_Check;
	wxCheckBox* mpOPD_Check;
	wxCheckBox* mpEDP_Check;
	wxCheckBox* mpNDF_Check;
	wxCheckBox* mpSPB_Check;

	wxCheckBox* mpHAP_Check;
	wxCheckBox* mpGCU_Check;
	wxCheckBox* mpAFP_Check;
	wxCheckBox* mpCSP_Check;

	wxTextCtrl* mpRWInput;
	wxStaticText* mpRWLabel;
	wxTextCtrl* mpPTMInput;
	wxStaticText* mpPTMLabel;
	wxTextCtrl* mpPBMInput;
	wxStaticText* mpPBMLabel;
	wxTextCtrl* mpPLMInput;
	wxStaticText* mpPLMLabel;
	wxTextCtrl* mpPRMInput;
	wxStaticText* mpPRMLabel;

	wxTextCtrl* mpDCInput;
	wxStaticText* mpDCLabel;
	wxTextCtrl* mpLCInput;
	wxStaticText* mpLCLabel;
	wxTextCtrl* mpGCInput;
	wxStaticText* mpGCLabel;
	wxTextCtrl* mpBCInput;
	wxStaticText* mpBCLabel;

	// fields/properties

	bool mRealTimeUpdatesOn;
	bool mOutOfPaneDragOn;
	bool mExactDockingPredictionOn;
	bool mNonDestructFrictionOn;
	bool m3DShadesOn;
	bool mHintRectAnimationOn;
	bool mGCUpdatesMgrOn;
	bool mAntiflickerPluginOn;
	bool mCustomizationPluginOn;

	int mSashWidth;
	int mTopMargin;
	int mBottomMargin;
	int mLeftMargin;
	int mRightMargin;

	wxColour mDarkCol;
	wxColour mLightCol;
	wxColour mGrayCol;
	wxColour mBorderCol;

protected:

	bool mToDlg;

	// helpers

	void ExchgCheck( wxCheckBox* pChk, bool& value );
	void ExchgIntField( wxTextCtrl* pFld, int& value );
	void ExchgColourField( wxTextCtrl* pFld, wxColour& value );

	virtual bool TransferDataToWindow();
	virtual bool TransferDataFromWindow();

public:

	SettingsDlg( wxWindow* pParent );

	void ReadLayoutSettings( wxFrameLayout& fl );
	void ApplyLayoutSettings( wxFrameLayout& fl );

	void ExchangeFields( bool toDialog );

	void OnApply( wxCommandEvent& event );
	void OnNotes( wxCommandEvent& event );

	void OnHintAnimCheck( wxCommandEvent& event );
	void OnRTUpdatesCheck( wxCommandEvent& event );

	DECLARE_EVENT_TABLE();
};

#endif