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

wxWallCtrlSurface::wxWallCtrlSurface(wxWallCtrl * parent)
{
	m_dataSource = NULL;
	m_loadingNeeded = false;
	m_currentLayer = 0;
	m_nextLayerItem = 0;
	SetParent(parent);
	
}

void wxWallCtrlSurface::SetDataSource( wxWallCtrlDataSource * dataSource )
{
	m_dataSource = dataSource;
}

wxWallCtrlDataSource * wxWallCtrlSurface::GetDataSource() const
{
	return m_dataSource;
}

bool wxWallCtrlSurface::IsItemTextureLoaded( wxWallCtrlItemID itemID )
{
	return (m_texturesCache.find( itemID ) != m_texturesCache.end());
}

bool wxWallCtrlSurface::TextureLoadingNeeded()
{
	return m_loadingNeeded;
}

void wxWallCtrlSurface::LoadNextLayerItemTexture()
{
	// TODO: We need to auto flag the m_loadingNeeded when no element is valid in the current layer

	// If we do not need to load, then stop
	if (!TextureLoadingNeeded())
	{
		return;
	}

	// If we finished this layer, move to the next one
	if (m_nextLayerItem >= GetLayerItemsCount(m_currentLayer))
	{
		m_currentLayer++;
		m_nextLayerItem = 0;

		// TODO: Rects apply only to some surfaces, make more general
		m_currentLayerRect = GetLayerRect(m_currentLayer);
		m_nextLayerItemPos.x = m_currentLayerRect.GetLeft();
		m_nextLayerItemPos.y = m_currentLayerRect.GetTop();
	}

	if (m_currentLayer > m_maxLoadingLayers)
	{
		m_loadingNeeded = false;
		return;
	}

	// How many invalid positions will we tolerate
	int threshold = GetLayerItemsCount(m_currentLayer) - m_nextLayerItem-1;

	wxWallCtrlItemID index = m_nextLayerItem;
	// Get the next item position
	do 
	{
		m_nextLayerItemPos = GetLayerItemPosition(m_nextLayerItem);
		if (IsValidPosition(m_nextLayerItemPos))
		{
			wxWallCtrlItemID index = GetItemIndex(m_nextLayerItemPos);
			if (index >= m_dataSource->GetCount())
			{
				// Move to the next one
				m_nextLayerItem++;
				continue;
			}
			if (!IsItemTextureLoaded(index))
			{
				// Load it, move to the next one, and return
				GetItemTexture(index);

				m_nextLayerItem++;
				return;
			}
		}
		else
		{
			threshold--;		
			if (threshold < 0)
			{
				// Do not proceed in this case, but increment the layer for next time
				m_currentLayer++;
				m_nextLayerItem = 0;
				m_currentLayerRect = GetLayerRect(m_currentLayer);
				m_nextLayerItemPos.x = m_currentLayerRect.GetLeft();
				m_nextLayerItemPos.y = m_currentLayerRect.GetRight();
				return;
			}
		}
		// Move to the next one
		m_nextLayerItem++;
	} while(!IsValidPosition(m_nextLayerItemPos) || IsItemTextureLoaded(index));
}

bool wxWallCtrlSurface::NeedLoading() const
{
	return m_loadingNeeded;
}

wxPoint wxWallCtrlSurface::GetLayerItemPosition( unsigned index ) const
{
	wxPoint point;
	if (index <= m_currentLayerRect.GetWidth() + m_currentLayerRect.GetHeight()-2)
	{
		if (index < GetLayerWidth(m_currentLayer))
		{
			// Top row
			point.x = m_currentLayerRect.GetLeft() + index;
			point.y = m_currentLayerRect.GetTop();
		}
		else
		{
			// Right column
			point.x = m_currentLayerRect.GetRight();
			point.y = m_currentLayerRect.GetTop() + (index - m_currentLayerRect.GetWidth()) + 1;
		}
	}
	else
	{
		if (index < GetLayerItemsCount(m_currentLayer) + 1 - m_currentLayerRect.GetHeight() )
		{
			// Bottom row
			point.x = m_currentLayerRect.GetRight() - (index - (m_currentLayerRect.GetHeight() + m_currentLayerRect.GetWidth()-2));
			point.y = m_currentLayerRect.GetBottom();
		}
		else
		{
			// Left column
			point.x = m_currentLayerRect.GetLeft();
			point.y = m_currentLayerRect.GetTop() + GetLayerItemsCount(m_currentLayer) - index;

		}
	}
	return point;
}

void wxWallCtrlSurface::CreateTextureFromDC( wxMemoryDC &dc, GLubyte * texture, const wxSize& dcSize )
{
	// TODO: This method may not be needed any more

	// Temp color variable
	wxColor color;

	// Copy each pixel color to the texture
	for (int y=0; y < dcSize.GetHeight(); ++y)
	{
		for (int x=0; x < dcSize.GetWidth(); ++x)
		{
			// Sequential pixel position
			unsigned pixel = y*dcSize.GetWidth() + x;

			dc.GetPixel(x, y, &color);

			*(texture + pixel*BYTES_PER_PIXEL + 0) = color.Red();
			*(texture + pixel*BYTES_PER_PIXEL + 1) = color.Green();
			*(texture + pixel*BYTES_PER_PIXEL + 2) = color.Blue();
			*(texture + pixel*BYTES_PER_PIXEL + 3) = 255;
		}
	}
}

void wxWallCtrlSurface::CreateTextureFromBitmap( wxBitmap bitmap, GLubyte * texture )
{
	typedef wxPixelData<wxBitmap, wxNativePixelFormat> PixelData;

	PixelData data(bitmap);
	if ( !data )
	{
		wxLogError(wxT("Could not load raw data from bitmap"));
		return;
	}

	PixelData::Iterator p(data);

	// Copy each pixel color to the texture
	for (int y=0; y < data.GetHeight(); ++y)
	{
		PixelData::Iterator rowStart = p;

		for (int x=0; x < data.GetWidth(); ++x)
		{
			// Sequential pixel position
			unsigned pixel = y*data.GetWidth() + x;

			// Set the texel
			*(texture + pixel*BYTES_PER_PIXEL + 0) = p.Red();
			*(texture + pixel*BYTES_PER_PIXEL + 1) = p.Green();
			*(texture + pixel*BYTES_PER_PIXEL + 2) = p.Blue();
			*(texture + pixel*BYTES_PER_PIXEL + 3) = 255;

			// Move to the next pixel
			++p;
		}
		// Go to the start of the next line
		p = rowStart;
		p.OffsetY(data, 1);
	}
}

GLuint wxWallCtrlSurface::GetItemTexture( wxWallCtrlItemID itemID )
{
	// Only one thread can get here at any point in time
	wxCriticalSectionLocker enter(m_texturesCS);

	// If that texture was previously loaded
	if (m_texturesCache.find( itemID ) != m_texturesCache.end())
	{
		// Then just return its cached name without reloading it
		return m_texturesCache[itemID];
	}

	// We will use the itemID as the texture identifier since item names are guaranteed to be unique
	GLuint texName = itemID + 1;	// We add one to make it 1-based

	// Query the item to get information about it
	wxWallCtrlItem info;
	m_dataSource->GetItemInfo(itemID, info);

	// Allocate enough space for the texture
	GLubyte * tex = new GLubyte[info.size.GetWidth()*info.size.GetHeight()*BYTES_PER_PIXEL];

	switch (m_dataSource->GetRenderType(itemID, wxSize(info.size.GetWidth(), info.size.GetHeight()), wxSize(info.size.GetWidth(), info.size.GetHeight())))
	{
	case wxWallCtrlRenderBitmap:
		CreateTextureFromBitmap(m_dataSource->GetBitmap(itemID), tex);
		break;

	case wxWallCtrlRenderDC:
		{
			// We must let the source render the texture for us, so we prepare a correctly sized DC for it
			wxMemoryDC tempDC;
			tempDC.SelectObject(wxBitmap (info.size.GetWidth(), info.size.GetHeight()));

			// Let the source draw the item
			m_dataSource->RenderItem(itemID, tempDC, info.size);
			CreateTextureFromDC(tempDC, tex, info.size);	// Why should this be used ?
		}

		break;
	default:
		// We do not know the render type
		wxLogError(wxT("Unknown rendering type specified"));
		// Delete the texture and return
		delete [] tex;
		return wxWallCtrlPlaneSurfaceInvalidTexture;
		break;
	}





	// Start creating the texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//glGenTextures(1, &texName);	// Already have a name so we don't need this

	// Select this texture
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);



	bool useMipmaps = true;

	if (useMipmaps)
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, info.size.GetWidth(), info.size.GetHeight(),	GL_RGBA, GL_UNSIGNED_BYTE, tex);
	}
	else
	{
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, info.size.GetWidth(), info.size.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
	}

	// We don't need the texture array any more.
	delete [] tex;

	// Cache the texture name before returning it
	return m_texturesCache[itemID]=texName;
}

void wxWallCtrlSurface::SetParent( wxWallCtrl * parent )
{
	m_parent = parent;
}

wxWallCtrl * wxWallCtrlSurface::GetParent()
{
	return m_parent;
}

void wxWallCtrlSurface::Reload()
{
	// Clear all loaded textures
	m_texturesCache.clear();

	m_currentLayer = 0;
	m_nextLayerItem = 0;

	if (m_dataSource)
	{
		m_loadingNeeded = true;
	}
	else
	{
		m_loadingNeeded = false;
	}
}