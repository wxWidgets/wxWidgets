/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     23/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dyntbar.cpp"
#pragma interface "dyntbar.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

/*
#ifdef __BORLANDC__
#pragma hdrstop
#endif
*/

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "dyntbarhnd.h"

/***** Implementation for class cbDynToolBarDimHandler *****/

IMPLEMENT_DYNAMIC_CLASS( cbDynToolBarDimHandler, cbBarDimHandlerBase )

void cbDynToolBarDimHandler::OnChangeBarState(cbBarInfo* pBar, int newState )
{
	// nothing
}

void cbDynToolBarDimHandler::OnResizeBar( cbBarInfo* pBar, 
										  const wxSize& given, 
										  wxSize& preferred )
{
	wxASSERT( pBar->mpBarWnd ); // DBG:: should be present

	wxDynamicToolBar* pTBar = (wxDynamicToolBar*)pBar->mpBarWnd;

	pTBar->GetPreferredDim( given, preferred );
}
