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

#ifndef WX_WALLCTRLNAVIGATION_H
#define WX_WALLCTRLNAVIGATION_H

#include "wx/wx.h"
#include "wx/wallctrl/wallctrlsurface.h"

class wxWallCtrlNavigation: 
	public wxEvtHandler
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrlNavigation)
public:
	wxWallCtrlNavigation();
	virtual ~wxWallCtrlNavigation();

	virtual wxWallCtrlSurface * GetSurface() const = 0;

	virtual void SetSurface(wxWallCtrlSurface * surface) = 0;

};

#endif