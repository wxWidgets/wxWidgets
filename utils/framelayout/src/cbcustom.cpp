/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     06/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "cbcustom.h"
// #pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "cbcustom.h"

// helper class to receive menu customization event

class cbContextMenuHandler : public wxEvtHandler
{
public:
	cbSimpleCustomizationPlugin* mpBackRef;

public:
	void OnMenuCommand( wxCommandEvent& evt );

	void OnCommandEvents( wxCommandEvent& evt );

	DECLARE_EVENT_TABLE();
};

// FIXME:: is this "safe" ?

#define CB_CUSTOMIZE_MENU_FIRST_ITEM_ID 17500

/***** Implementation for helper class cbContextMenuHandler *****/

BEGIN_EVENT_TABLE( cbContextMenuHandler, wxEvtHandler )

	// FIXME:: what is the right range for these ids ? so that they 
	//         would not collide with user commands?

	EVT_COMMAND_RANGE( CB_CUSTOMIZE_MENU_FIRST_ITEM_ID,
					   CB_CUSTOMIZE_MENU_FIRST_ITEM_ID + 300, 
					   wxEVT_COMMAND_MENU_SELECTED,  
					   cbContextMenuHandler::OnCommandEvents )

END_EVENT_TABLE()

void cbContextMenuHandler::OnCommandEvents( wxCommandEvent& evt )
{
	//wxMessageBox("Wowwwww, Yeah!");

	mpBackRef->OnMenuItemSelected( evt );
}

/***** Implementation for class cbSimpleCustomizationPlugin *****/

IMPLEMENT_DYNAMIC_CLASS( cbSimpleCustomizationPlugin, cbPluginBase )

BEGIN_EVENT_TABLE( cbSimpleCustomizationPlugin, cbPluginBase )

	EVT_PL_CUSTOMIZE_BAR   ( cbSimpleCustomizationPlugin::OnCustomizeBar    )
	EVT_PL_CUSTOMIZE_LAYOUT( cbSimpleCustomizationPlugin::OnCustomizeLayout )

END_EVENT_TABLE()

cbSimpleCustomizationPlugin::cbSimpleCustomizationPlugin(void)
{}

cbSimpleCustomizationPlugin::cbSimpleCustomizationPlugin( wxFrameLayout* pPanel, int paneMask )

	: cbPluginBase( pPanel, paneMask )
{}

void cbSimpleCustomizationPlugin::OnCustomizeBar( cbCustomizeBarEvent& event )
{
	// ingnore bar customization, treat it
	// as layout-customization...ugly, eh?

	cbCustomizeLayoutEvent clEvt( event.mClickPos );

	OnCustomizeLayout( clEvt );
}

void cbSimpleCustomizationPlugin::OnCustomizeLayout( cbCustomizeLayoutEvent& event )
{
	wxString helpStr1 = "Select this item to show the corresponding control bar";
	wxString helpStr2 = "Select this itme to hide the corresponding control bar";

	int id = CB_CUSTOMIZE_MENU_FIRST_ITEM_ID;

	wxMenu* pMenu = new wxMenu();

	BarArrayT& bars = mpLayout->GetBars();

	for( size_t i = 0; i != bars.GetCount(); ++i )
	{
		cbBarInfo& bar = *bars[i];
		
		bool isHidden = ( bar.mState == wxCBAR_HIDDEN );

		wxString* pHelpStr = ( isHidden ) ? &helpStr1 : &helpStr2;

		pMenu->Append( id, bar.mName, *pHelpStr, TRUE );

		pMenu->Check( id, (isHidden == FALSE) );

		++id;
	}

	pMenu->AppendSeparator();
	pMenu->Append( id, "Customize...", "Show layout customization dialog", FALSE );
	mCustMenuItemId = id;

	cbContextMenuHandler* pHandler = new cbContextMenuHandler();
	pHandler->mpBackRef            = this;

	wxWindow* pFrm = &mpLayout->GetParentFrame();

	// FOR NOW FOR NOW:: to work-around wxFrame's (MSW) nasty event-handling bugs!!!

	wxWindow* pTmpWnd = new wxWindow( pFrm, -1, event.mClickPos, wxSize(0,0) );

	pMenu->SetEventHandler( pHandler );

	pTmpWnd->PopupMenu( pMenu, 0,0 );

	pTmpWnd->Destroy();

	delete pMenu;
	delete pHandler;

	// event is "eaten" by this plugin
}

void cbSimpleCustomizationPlugin::OnMenuItemSelected( wxCommandEvent& event )
{
	if ( event.m_commandInt == mCustMenuItemId )
	{
		wxMessageBox("Customization dialog box is not supproted by this plugin yet");

		return;
	}
	else
	{
		cbBarInfo* pBar = mpLayout->GetBars()[ event.m_commandInt - 
											   CB_CUSTOMIZE_MENU_FIRST_ITEM_ID
			                                 ];

		wxASSERT( pBar ); // DBG::

		// "inverse" bar-visibility of the selected bar

		int newState = 0;

		if ( pBar->mState == wxCBAR_HIDDEN )
		{
			if ( pBar->mAlignment == -1 )
			{
				pBar->mAlignment = 0;       // just remove "-1" marking
				newState = wxCBAR_FLOATING;
			}
			else
			if ( pBar->mAlignment == wxTOP ||
				 pBar->mAlignment == wxBOTTOM )

				newState = wxCBAR_DOCKED_HORIZONTALLY;
			else
				newState = wxCBAR_DOCKED_VERTICALLY;
		}
		else
		{
			newState = wxCBAR_HIDDEN;

			if ( pBar->mState == wxCBAR_FLOATING )

				pBar->mAlignment = -1;
		}

		mpLayout->SetBarState( pBar, newState, TRUE );

		if ( newState == wxCBAR_FLOATING )

			mpLayout->RepositionFloatedBar( pBar ); 
	}

	// menu-item-selected event is "eaten"
}

