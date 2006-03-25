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

#ifndef __DYNTBARHND_G__
#define __DYNTBARHND_G__

#include "controlbar.h"
#include "dyntbar.h"

class cbDynToolBarDimHandler : public cbBarDimHandlerBase
{
	DECLARE_DYNAMIC_CLASS( cbDynToolBarDimHandler )
public:
	void OnChangeBarState(cbBarInfo* pBar, int newState );
	void OnResizeBar( cbBarInfo* pBar, const wxSize& given, wxSize& preferred );
};

#endif