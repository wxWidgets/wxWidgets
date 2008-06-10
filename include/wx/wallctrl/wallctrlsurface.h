#pragma once
#include "WallCtrlDataSource.h"

class wxWallCtrlSurface
{
public:
	wxWallCtrlSurface(wxWallCtrlDataSource * dataSource):m_dataSource(dataSource)
	{

	}
	virtual ~wxWallCtrlSurface(void);

	virtual void Render()=0;


protected:
	wxWallCtrlDataSource * m_dataSource;
};
