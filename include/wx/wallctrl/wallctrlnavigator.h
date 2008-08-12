/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlnavigation.h
// Purpose:     The base class for all wxWallCtrl navigation objects
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     14/06/2008
// RCS-ID:      
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WALLCTRLNAVIGATOR_H
#define WX_WALLCTRLNAVIGATOR_H

#include "wx/wx.h"
#include "wx/wallctrl/wallctrlsurface.h"

class wxWallCtrlNavigator: 
	public wxEvtHandler
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrlNavigator)
public:
	wxWallCtrlNavigator();
	virtual ~wxWallCtrlNavigator();

	// Returns the associated surface
	virtual wxWallCtrlSurface * GetSurface() const = 0;

	// Sets a surface for the navigator
	virtual void SetSurface(wxWallCtrlSurface * surface) = 0;

};

#endif