/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlsurface.cpp
// Purpose:     Base class for surfaces for wxWallCtrl
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     04/06/2008 04:12:40   
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "wx/Wallctrl/WallCtrlSurface.h"

IMPLEMENT_CLASS(wxWallCtrlSurface, wxObject)

wxWallCtrlSurface::~wxWallCtrlSurface(void)
{
}

wxWallCtrlSurface::wxWallCtrlSurface()
{
	m_dataSource = NULL;
}

void wxWallCtrlSurface::SetDataSource( wxWallCtrlDataSource * dataSource )
{
	m_dataSource = dataSource;
}

wxWallCtrlDataSource * wxWallCtrlSurface::GetDataSource() const
{
	return m_dataSource;
}