/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlsurface.h
// Purpose:     Base class for surfaces for wxWallCtrl
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     04/06/2008 04:12:40   
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WALLCTRLSURFACE_H
#define WX_WALLCTRLSURFACE_H

#include "WallCtrlDataSource.h"

// TODO: Check if inheriting from wxObject is correct
class wxWallCtrlSurface:
	public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrlSurface)
public:
	wxWallCtrlSurface();
	virtual ~wxWallCtrlSurface(void);

	virtual void Render(const wxSize & windowSize)=0;

	virtual void SetDataSource(wxWallCtrlDataSource * dataSource);

	virtual wxWallCtrlDataSource * GetDataSource() const;


protected:
	wxWallCtrlDataSource * m_dataSource;
};
#endif