/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlsurface.h
// Purpose:     Base class for surfaces for wxWallCtrl
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     04/06/2008 04:12:40
// RCS-ID:      
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "WallCtrlDataSource.h"

class wxWallCtrlSurface
{
public:
	wxWallCtrlSurface(wxWallCtrlDataSource * dataSource):m_dataSource(dataSource)
	{

	}
	virtual ~wxWallCtrlSurface(void);

	virtual void Render(const wxSize & windowSize)=0;


protected:
	wxWallCtrlDataSource * m_dataSource;
};
