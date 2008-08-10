/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlplanesurface.cpp
// Purpose:     A concrete plane (flat) surface for wxWallCtrl
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     08/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "wx/wallctrl/WallCtrlPlaneSurface.h"


IMPLEMENT_CLASS(wxWallCtrlPlaneSurface, wxWallCtrlSurface)

wxWallCtrlPlaneSurface::~wxWallCtrlPlaneSurface(void)
{
	DestroyLoadingThread();
}

wxWallCtrlPlaneSurface::wxWallCtrlPlaneSurface()
{
	m_initialized = false;
	m_defaultDistance = 2;

	// Scope
	m_scopeSize.SetHeight(1);
	m_scopeSize.SetWidth(1);
	m_scopeOffsetX = 0;
	m_scopeOffsetY = 0;

	// Selection
	m_selectedIndex = 0;
	m_selectionMargin = 0.05;
	m_selectionDepth = 0.05;

	// Caching
	m_currentLayer = 0;
	m_nextLayerItem = 0;
	m_nextLayerItemPos.x = 0;
	m_nextLayerItemPos.y = 0;
	m_loadingNeeded = true;
	m_rendersBeforeTextureLoad = 25;
	m_renderCount = m_rendersBeforeTextureLoad;

	m_loaderThread = NULL;
	m_loadingInProgress = false;

	// Set the near and far limits
	// TODO: The far limit may be a function of the max number of visible item
	m_nearLimit = 0.1;
	m_farLimit = 10;

	// Initialize camera position
	m_camera.resize(3,0);
	m_camera[2] = m_defaultDistance;		// We start the camera at (0,0,2)
	
	m_targetCamera = m_camera;	// Both vectors should match initially
	
	// Initialize all direction vectors
	m_look.resize(3,0);	
	m_targetLook.resize(3,0);
	
	m_right.resize(3,0);
	m_right[0] = 1;
	
	m_up.resize(3,0);
	m_up[1] = 1;

	UpdateItemSize();

	// Actual initialization of the first item is done when a data source is selected
	m_firstItem = 0;
	m_rowsCount = 4;
}

void wxWallCtrlPlaneSurface::UpdateItemSize()
{
	// TODO: Take border size into consideration
	m_itemHeight = 1.0/m_scopeSize.GetHeight();
	m_itemWidth = 1.0/m_scopeSize.GetWidth();
}

void wxWallCtrlPlaneSurface::Render(const wxSize & windowSize)
{
	// Init OpenGL once. SetCurrent() must have been called before reaching this point
	if (!m_initialized)
	{
		InitializeGL();
		m_initialized = true;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0,(GLfloat)windowSize.GetWidth()/(GLfloat)windowSize.GetHeight(),0.01f,10000.0f);
	//glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 5.0f);

	glMatrixMode(GL_MODELVIEW);
	// Clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Place camera
	glLoadIdentity();
	//glTranslatef(0.0f, 0.0f, -1.0f);
	//glTranslatef(m_camera[0], m_camera[1], m_camera[2]);
	
	gluLookAt(m_camera[0], m_camera[1], m_camera[2], m_look[0], m_look[1], m_look[2], m_up[0], m_up[1], m_up[2]);

	UpdateVectors();

	// This will be replaced soon
	RenderItems();

	glFlush();
}




void wxWallCtrlPlaneSurface::InitializeGL()
{
	//SetCurrent();	// This is done outside now

	/* set viewing projection */
	glMatrixMode(GL_PROJECTION);
	glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
	//gluPerspective(45.0,(GLfloat)400/(GLfloat)400,0.01f,10000.0f);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
}


unsigned wxWallCtrlPlaneSurface::GetItemIndex(int x, int y) const
{
	return m_firstItem + x * m_rowsCount + y;
}

unsigned wxWallCtrlPlaneSurface::GetItemIndex( wxWallCtrlItemID itemID ) const
{
	// The ID is the same as the index and this time
	return itemID;
}

unsigned wxWallCtrlPlaneSurface::GetItemIndex( wxPoint position ) const
{
	return GetItemIndex(position.x, position.y);
}

void wxWallCtrlPlaneSurface::RenderItems()
{
	bool useThreads = false; 

	if (!m_dataSource)
	{
		// TODO: Signal an error here
		return;
	}

	// TODO: There might be a better place for this whole code chunk
	if ((m_dataSource->HasDataChanged() && !m_loadingInProgress))
	{
		if (m_dataSource->GetCount() > 0)
		{
			// If we have a data source with data, query its first item
			m_firstItem = m_dataSource->GetFirstItem();
			Seek(m_firstItem);
		}

		if (m_rowsCount == 0)
		{
			// We have nothing, so stop now
			return;
		}

		// Deduce the columns count. We need this each render loop since the bitmaps can change
		m_colsCount = m_dataSource->GetCount()/m_rowsCount + (m_dataSource->GetCount()%m_rowsCount == 0?0:1);

		if (m_colsCount == 0)
		{
			// We have nothing, so stop now
			return;
		}
		
		// There is new data and nobody is loading it
		m_loadingInProgress = true;
		m_loadingNeeded = true;

		m_maxLoadingLayers = wxMax(m_dataSource->GetCount()/m_rowsCount+1, m_dataSource->GetCount()/m_colsCount+1);

		if (useThreads)
		{
			CreateLoadingThread();
			RunLoadingThread();
		}
	}
	if (!useThreads)
	{
		// Deduce the columns count. We need this each render loop since the bitmaps can change
		//m_colsCount = m_dataSource->GetCount()/m_rowsCount + (m_dataSource->GetCount()%m_rowsCount == 0?0:1);
		m_renderCount--;
		if (m_renderCount <= 0)
		{
			m_renderCount = m_rendersBeforeTextureLoad;
			LoadNextLayerItemTexture();
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glPushMatrix();
	// Loop over the grid-like scope window
	for (int x=m_scopeOffsetX; x < wxMin(m_colsCount, m_scopeOffsetX + m_scopeSize.GetWidth()); ++x)
	{
		for (int y=m_scopeOffsetY; y < wxMin(m_rowsCount, m_scopeOffsetY + m_scopeSize.GetHeight()); ++y)
		{
			// Get the index of the item at this position
			int index = GetItemIndex(x, y);//m_firstItem + x * m_scopeSize.GetHeight() + y;

			// Check if we ran out of items
			if (index >= m_dataSource->GetCount())
			{
				// If so break out
				y = m_scopeSize.GetHeight();
				break;
			}

			
			// This will be used when the texture is not loaded
			// TODO: Replace this with a predefined texture
			GLuint itemTexture = -1;

			// See if the item we need is loaded
			if (IsItemTextureLoaded(index))
			{
				// Select the texture of this item
				itemTexture = GetItemTexture(index);
				if (itemTexture == wxWallCtrlPlaneSurfaceInvalidTexture)
				{
					continue;
				}
			}

			glBindTexture(GL_TEXTURE_2D, itemTexture);


			// Get the bounds of the quad
			wxRealRect rect = GetRawItemRect(x, y);
			
			// Adjust the bounds so that it maintains the aspect ratio
			wxWallCtrlItem info;
			m_dataSource->GetItemInfo(index, info);
			AdjustCoordinates(rect, info.size);
			//AdjustCoordinates(Top, Bottom, Left, Right, info.size);

			if (index == m_selectedIndex)
			{
				glPushMatrix();
				glTranslatef(0, 0, m_selectionDepth);

				// Draw this item on its own quad
				glBegin(GL_QUADS);					
				glTexCoord2f(1.0, 1.0);
				glVertex3f( rect.GetRight()+m_selectionMargin, rect.GetTop()-m_selectionMargin, 0.0f);			// Top Right
				glTexCoord2f(0.0, 1.0);
				glVertex3f(rect.GetLeft()-m_selectionMargin, rect.GetTop()-m_selectionMargin, 0.0f);			// Top Left
				glTexCoord2f(0.0, 0.0);
				glVertex3f(rect.GetLeft()-m_selectionMargin, rect.GetBottom()+m_selectionMargin, 0.0f);			// Bottom Left
				glTexCoord2f(1.0, 0.0);
				glVertex3f(rect.GetRight()+m_selectionMargin, rect.GetBottom()+m_selectionMargin, 0.0f);			// Bottom Right
				glEnd();

				glPopMatrix();
				
			}
			else
			{
				// Draw this item on its own quad
				glBegin(GL_QUADS);					
				glTexCoord2f(1.0, 1.0);
				glVertex3f( rect.GetRight(), rect.GetTop(), 0.0f);			// Top Right
				glTexCoord2f(0.0, 1.0);
				glVertex3f(rect.GetLeft(), rect.GetTop(), 0.0f);			// Top Left
				glTexCoord2f(0.0, 0.0);
				glVertex3f(rect.GetLeft(), rect.GetBottom(), 0.0f);			// Bottom Left
				glTexCoord2f(1.0, 0.0);
				glVertex3f(rect.GetRight(), rect.GetBottom(), 0.0f);			// Bottom Right
				glEnd();
			}


		}
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void wxWallCtrlPlaneSurface::SetScopeSize( wxSize size )
{
	if ((size.GetHeight() < 1) || (size.GetWidth() < 1) || (size.GetHeight() > m_rowsCount))
	{
		// TODO: This is a serious issue, can't proceed. Check how to signal errors
		assert(false);
		return;
	}
	m_scopeSize = size;
	UpdateItemSize();
}

float wxWallCtrlPlaneSurface::MapX( float x ) const
{
	return x*2 - 1;
}

float wxWallCtrlPlaneSurface::MapY( float y ) const
{
	return y*2 - 1;
}

// Right >= Left && Bottom >= Top
void wxWallCtrlPlaneSurface::AdjustCoordinates(wxRealRect &rect, const wxSize &itemSize) const//( float & top, float & bottom, float & left, float & right, const wxSize & itemSize)
{
	float width = rect.GetRight() - rect.GetLeft();
	float height = rect.GetBottom()- rect.GetTop();

	// The new dimensions
	float newWidth  = 0;
	float newHeight = 0;

	// The ratios
	float widthRatio = width/itemSize.GetWidth();
	float heightRatio = height/itemSize.GetHeight();

	// Check which dimension is the limiting one
	if (widthRatio < heightRatio)
	{
		// The width is the limiting dimension
		newWidth = itemSize.x * widthRatio;
		newHeight = itemSize.y * widthRatio;
	}
	else
	{
		// The height is the limiting dimension
		newWidth = itemSize.x * heightRatio;
		newHeight = itemSize.y * heightRatio;
	}
	
	// Use the new dimensions to recalculate the sizes. The new dimensions are less than or equal the supplied ones
	float hzMargin = (width - newWidth)/2;
	float vtMargin = (height- newHeight)/2;
	
	// Modify the bounds based on the margins
	rect.SetTop(rect.GetTop() + vtMargin);
	rect.SetBottom (rect.GetBottom() - vtMargin);
	rect.SetLeft(rect.GetLeft() + hzMargin);
	rect.SetRight(rect.GetRight() - hzMargin);
}

void wxWallCtrlPlaneSurface::MoveRight( float delta )
{
	m_targetLook[0] += delta;
}

void wxWallCtrlPlaneSurface::MoveLeft( float delta )
{
	m_targetLook[0] -= delta;
}

void wxWallCtrlPlaneSurface::MoveIn( float delta )
{
	m_targetCamera[2] -= delta;
	if (m_targetCamera[2] < m_nearLimit)
	{
		m_targetCamera[2] = m_nearLimit;
	}
}

void wxWallCtrlPlaneSurface::MoveOut( float delta )
{
	m_targetCamera[2] += delta;
	if (m_targetCamera[2] > m_farLimit)
	{
		m_targetCamera[2] = m_farLimit;
	}
}

void wxWallCtrlPlaneSurface::UpdateVectors()
{
	// TODO: Move these into constructor
	m_lookDelta = 0.05;
	m_cameraDelta = 0.025;
	m_LookThreshold = m_cameraThreshold =0.1;
	m_cameraPanningDelta = 0.05;

	UpdateLookVector();

	// Update the camera based on the previous camera
	UpdateCameraVector();
}

void wxWallCtrlPlaneSurface::UpdateCameraVector()
{
	// Check if we are far enough to require moving the look point. A threshold avoids oscillations
	if (m_look[0] < m_targetCamera[0] - m_LookThreshold)
	{
		m_targetCamera[0] -= m_cameraPanningDelta;
	}
	else if (m_look[0] > m_camera[0] + m_LookThreshold)
	{
		m_targetCamera[0] += m_cameraPanningDelta;
	}

	// Then update the camera vector to match its target
	// TODO: *Hz* should be replaced by something else
	for (int i=0; i < 3; ++i)
	{
		if (m_camera[i] < m_targetCamera[i] - m_cameraThreshold)
		{
			m_camera[i] += m_cameraDelta;
		}
		else if (m_camera[i] > m_targetCamera[i] + m_cameraThreshold)
		{
			m_camera[i] -= m_cameraDelta;
		}
	}
}

void wxWallCtrlPlaneSurface::UpdateLookVector()
{
	for (int i=0; i < 3; ++i)
	{
		if (m_look[i] < m_targetLook[i] - m_LookThreshold)
		{
			m_look[i] += m_lookDelta;
		}
		else if (m_look[i] > m_targetLook[i] + m_LookThreshold)
		{
			m_look[i] -= m_lookDelta;
		}
	}
}

wxPoint wxWallCtrlPlaneSurface::GetItemPosition( unsigned index ) const
{
	wxPoint P;
	P.x = index / m_rowsCount;
	P.y = index % m_rowsCount;

	return P;
}

wxWallCtrlPlaneSurface::wxRealRect wxWallCtrlPlaneSurface::GetRawItemRect( int x, int y ) const
{
	wxRealRect rect;
	rect.SetTop(MapY(m_itemHeight * y));
	rect.SetBottom(MapY(m_itemHeight * (y + 1)));
	rect.SetLeft(MapX(m_itemWidth * x));
	rect.SetRight(MapX(m_itemWidth * (x + 1)));

	return rect;
}



VectorType wxWallCtrlPlaneSurface::GetItemCenter( wxWallCtrlItemID itemID ) const
{
	VectorType pos;
	pos.resize(3, 0);

	wxPoint point = GetItemPosition(GetItemIndex(itemID));

	// Get the rect of the item
	wxRealRect rect = GetRawItemRect(point.x, point.y);

	// Query the item for more information
	wxWallCtrlItem info;
	m_dataSource->GetItemInfo(GetItemIndex(itemID), info);

	// Adjust the rect for aspect ratio
	AdjustCoordinates(rect, info.size);

	// Set the position of the center
	pos[0] = (rect.GetRight() + rect.GetLeft())/2.0;
	pos[1] = (rect.GetBottom() + rect.GetTop())/2.0;
	pos[2] = 0;

	return pos;
}

void wxWallCtrlPlaneSurface::SetDataSource( wxWallCtrlDataSource * dataSource )
{
	wxWallCtrlSurface::SetDataSource(dataSource);

	if (dataSource)
	{
		
		
		// Update the columns
		m_colsCount = m_dataSource->GetCount()/m_rowsCount + (m_dataSource->GetCount()%m_rowsCount == 0?0:1);

		if (dataSource->GetCount() > 0)
		{
			// If we have a data source with data, query its first item
			m_firstItem = dataSource->GetFirstItem();
			Seek(m_firstItem);
		}
	}
}

void wxWallCtrlPlaneSurface::Seek( wxWallCtrlItemID itemID )
{
	// Update the motion varaibles
	m_targetLook = GetItemCenter(itemID);
	m_targetCamera[0] = m_targetLook[0];
	m_targetCamera[1] = m_targetLook[1];
	m_targetCamera[2] = m_defaultDistance;

	// Update the logical selection
	m_selectedIndex = itemID;

	// Update the scope
	AdjustScope();

	// Reset the layers to start at this specific item
	m_currentLayer = 0;
	m_currentLayerRect = GetLayerRect(m_currentLayer);
	m_nextLayerItem = 0;
	m_nextLayerItemPos = GetItemPosition(itemID);

	//LoadNextLayerItemTexture();
	
}

void wxWallCtrlPlaneSurface::SeekLeft()
{
	wxPoint Pos = GetItemPosition(m_selectedIndex);

	Pos.x--;

	// Check if we ran out of items
	if (GetItemIndex(Pos) >= m_dataSource->GetCount())
	{
		// If so do nothing
		Pos.x++;
		return;
	}

	
	if (Pos.x < 0)
	{
		Pos.x = 0;
	}
	Seek(GetItemIndex(Pos));
}

void wxWallCtrlPlaneSurface::SeekRight()
{
	wxPoint Pos = GetItemPosition(m_selectedIndex);

	Pos.x++;

	// Check if we ran out of items
	if (GetItemIndex(Pos) >= m_dataSource->GetCount())
	{
		// If so do nothing
		Pos.x--;
		return;
	}

	if (Pos.x >= m_colsCount)
	{
		Pos.x = m_colsCount-1;
	}

	Seek(GetItemIndex(Pos));
}

void wxWallCtrlPlaneSurface::SeekUp()
{
	wxPoint Pos = GetItemPosition(m_selectedIndex);

	Pos.y++;

	// Check if we ran out of items
	if (GetItemIndex(Pos) >= m_dataSource->GetCount())
	{
		// If so do nothing
		Pos.y--;
		return;
	}

	
	if (Pos.y >= m_rowsCount)
	{
		Pos.y = m_rowsCount-1;
	}
	Seek(GetItemIndex(Pos));
}

void wxWallCtrlPlaneSurface::SeekDown()
{
	wxPoint Pos = GetItemPosition(m_selectedIndex);

	Pos.y--;
	// Check if we ran out of items
	if (GetItemIndex(Pos) >= m_dataSource->GetCount())
	{
		// If so do nothing
		Pos.y++;
		return;
	}

	
	if (Pos.y < 0)
	{
		Pos.y = 0;
	}
	Seek(GetItemIndex(Pos));


}

void wxWallCtrlPlaneSurface::CreateLoadingThread()
{
	// Destroy the previous thread if any
	DestroyLoadingThread();

	// Start a loading thread
	m_loaderThread = new wxWallCtrlLoadingThread(this);

	if ( m_loaderThread->Create() != wxTHREAD_NO_ERROR )
	{
		wxLogError(wxT("Can't create texture loading thread!"));
	}
}


void wxWallCtrlPlaneSurface::RunLoadingThread()
{
	if (!m_loaderThread)
	{
		// TODO: Signal an error, we should never get to this point
		return;
	}
	if (m_loaderThread->Run() != wxTHREAD_NO_ERROR )
	{
		wxLogError(wxT("Can't start thread!"));
	}
}

void wxWallCtrlPlaneSurface::DestroyLoadingThread()
{
	if (m_loaderThread)
	{
		m_loaderThread->Kill();
		delete (m_loaderThread);
		// TODO: We should destroy the thread more gracefuly, but something is wrong here
/*		if (!m_loaderThread->IsAlive())
		{
			m_loaderThread->Delete();
		}
		while (m_loaderThread->IsAlive())
		{
			// Just wait for the thread to exit
		}
		delete (m_loaderThread);*/
	}

}

int wxWallCtrlPlaneSurface::GetLayerWidth( int layer ) const
{
	// We want the layers around a specific item
	return 2*layer + 1;//m_scopeSize.GetWidth();
}

int wxWallCtrlPlaneSurface::GetLayerHeight( int layer ) const
{
	// We want the layers around a specific item
	return 2*layer + 1;//m_scopeSize.GetHeight();
}

int wxWallCtrlPlaneSurface::GetLayerItemsCount( int layer ) const
{
	if (layer == 0)
		return 1;
	return 2*(GetLayerWidth(layer) + GetLayerHeight(layer))-4;
}

wxRect wxWallCtrlPlaneSurface::GetLayerRect( int layer ) const
{
	/*return wxRect (m_scopeOffsetX - 1,
	m_scopeOffsetY - 1,
	GetLayerWidth(layer),
	GetLayerHeight(layer));*/
	wxPoint pos = GetItemPosition(m_selectedIndex);
	return wxRect (pos.x-layer,
		pos.y-layer,
		GetLayerWidth(layer),
		GetLayerHeight(layer));
}





void wxWallCtrlPlaneSurface::OnLoadingComplete()
{
	m_loadingInProgress = false;
}

unsigned wxWallCtrlPlaneSurface::GetMaxLoadingLayers() const
{
	return m_maxLoadingLayers;
}

bool wxWallCtrlPlaneSurface::IsValidPosition( wxPoint pos )
{
	return ((pos.x >= 0 && pos.x < m_colsCount) && (pos.y >= 0 && pos.y < m_rowsCount));
}


void wxWallCtrlPlaneSurface::AdjustScope()
{
	// Find the position of the selected index
	wxPoint position = GetItemPosition(m_selectedIndex);

	// Try to center it in the scope. Note that we need only clip from the lower bounds.
	m_scopeOffsetX = wxMax(position.x - m_scopeSize.GetWidth()/2, 0);
	m_scopeOffsetY = wxMax(position.y - m_scopeSize.GetHeight()/2,0);
}