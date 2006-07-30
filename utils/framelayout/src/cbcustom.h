/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     28/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __CBCUSTOM_G__
#define __CBCUSTOM_G__

#ifdef __GNUG__
#pragma interface "cbcustom.h"
#endif

#include "controlbar.h"

class cbSimpleCustomizationPlugin : public cbPluginBase
{
public:
	DECLARE_DYNAMIC_CLASS( cbSimpleCustomizationPlugin )

	int mCustMenuItemId;
public:

	cbSimpleCustomizationPlugin(void);

	cbSimpleCustomizationPlugin( wxFrameLayout* pPanel, int paneMask = wxALL_PANES );

	// plugin-event handlers

	void OnCustomizeBar( cbCustomizeBarEvent& event );

	void OnCustomizeLayout( cbCustomizeLayoutEvent& event );

	// menu-event handler

	void OnMenuItemSelected( wxCommandEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif