/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlplanesurface.h
// Purpose:     A concrete plane (flat) surface for wxWallCtrl
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     08/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WALLCTRLPLANESURFACE_H
#define WX_WALLCTRLPLANESURFACE_H

#include "wallctrlsurface.h"
#include "gl/glut.h"
#include "wx/wallctrl/matrix.h"
#include <iostream>
#include <sstream>
#include <string>
#define BYTES_PER_PIXEL 4

class wxWallCtrlLoadingThread;

// A map to hold the texture names for items that were previously cached
WX_DECLARE_HASH_MAP(int, int, wxIntegerHash, wxIntegerEqual, Int2IntMap);


const int wxWallCtrlPlaneSurfaceInvalidTexture = -10;

// This is a default surface implementation. This is where all the geometric manipulation and rendering take place
class wxWallCtrlPlaneSurface :
	public wxWallCtrlSurface
{
	// TODO: See if there is a better place for this struct
	struct wxRealRect
	{
		float GetTop()const
		{
			return m_top;
		}
		float GetBottom() const
		{
			return m_bottom;
		}
		float GetRight() const
		{
			return m_right;
		}
		float GetLeft() const
		{
			return m_left;
		}

		void SetTop(float top)
		{
			m_top = top;
		}
		void SetBottom(float bottom)
		{
			m_bottom = bottom;
		}
		void SetRight(float right)
		{
			m_right = right;
		}
		void SetLeft(float left)
		{
			m_left = left;
		}

		float m_top;
		float m_bottom;
		float m_right;
		float m_left;
	};

	DECLARE_DYNAMIC_CLASS(wxWallCtrlPlaneSurface)
public:
	// Precondition: dataSource must be valid
	wxWallCtrlPlaneSurface();
	virtual ~wxWallCtrlPlaneSurface(void);

	// Recalculates the item size from the number of items in the scope
	void UpdateItemSize();

	// Render the surface with all its items
	void Render(const wxSize & windowSize);

	// Initialize OpenGL for first use
	void InitializeGL();

	// This method renders all the items in the scope
	void RenderItems();

	// Sets the number of elements in a single frame (in both directions)
	void SetScopeSize(wxSize size);

	// Moves to the left with the specified delta factor
	void MoveLeft(float delta);

	// Moves to the right with the specified delta factor
	void MoveRight(float delta);

	// Moves closer to the wall
	void MoveIn(float delta);

	// Moves out far from the wall
	void MoveOut(float delta);

	virtual void SetDataSource(wxWallCtrlDataSource * dataSource);

	// Returns the point at the center of the specified item in OpenGL coordinates
	VectorType GetItemCenter(wxWallCtrlItemID itemID) const;

	void Seek(wxWallCtrlItemID itemID);
	
	void SeekLeft();
	void SeekRight();
	void SeekUp();
	void SeekDown();

	// TODO: Width & Height are redundant since the layer is squared

	// Spiral square layers are layers surrounding an item.
	// Returns the number of columns in the specified layer
	int GetLayerWidth(int layer) const
	{
		// We want the layers around a specific item
		return 2*layer + 1;//m_scopeSize.GetWidth();
	}
	// Returns the number of columns in the specified layer
	int GetLayerHeight(int layer) const
	{
		// We want the layers around a specific item
		return 2*layer + 1;//m_scopeSize.GetHeight();
	}

	// Returns the total number of elements in the specified layer, some of which may be invalid
	int GetLayerItemsCount(int layer) const
	{
		if (layer == 0)
			return 1;
		return 2*(GetLayerWidth(layer) + GetLayerHeight(layer))-4;
	}

	wxRect GetLayerRect(int layer) const
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

	bool NeedLoading() const
	{
		return m_loadingNeeded;
	}

	void LoadNextLayerItemTexture()
	{
		// TODO: We need to auto flag the m_loadingNeeded when no element is valid in the current layer

		// If we do not need to load, then stop
		if (!m_loadingNeeded)
		{
			return;
		}
		
		// If we finished this layer, move to the next one
		if (m_nextLayerItem >= GetLayerItemsCount(m_currentLayer))
		{
			m_currentLayer++;
			m_nextLayerItem = 0;
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

	void OnLoadingComplete()
	{
		m_loadingInProgress = false;
	}

	void CreateLoadingThread();

	void DestroyLoadingThread();

	unsigned GetMaxLoadingLayers() const
	{
		return m_maxLoadingLayers;
	}

	// Must be preceeded by a call to CreateLoadingThread
	void RunLoadingThread();
protected:
	// Loads a texture from the DC into the supplied array
	// Precondition: Texture must be at least Width * Height * BYTES_PER_PIXEL
	void CreateTextureFromDC(wxMemoryDC &dc, GLubyte * texture, const wxSize& dcSize);

	// Loads a bitmap into a texture array
	// Precondition: Texture must be at least Width * Height * BYTES_PER_PIXEL
	void CreateTextureFromBitmap(wxBitmap bitmap, GLubyte * texture);

	// Returns the texture ID for a specific item, and if it does not exist it loads it
	GLuint GetItemTexture(wxWallCtrlItemID itemID);

	// Returns true if the specified item has a loaded & cached texture
	bool IsItemTextureLoaded(wxWallCtrlItemID itemID)
	{
		return (m_texturesCache.find( itemID ) != m_texturesCache.end());
	}

	// Returns true if the specified coordinates make sense
	bool IsValidPosition(wxPoint pos)
	{
		return ((pos.x >= 0 && pos.x < m_colsCount) && (pos.y >= 0 && pos.y < m_rowsCount));
	}

	// Returns the logical position of a specific index in the current layer
	wxPoint GetLayerItemPosition(unsigned index) const
	{
		// TODO: This method is bugged
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

	// Maps an X coordinate to OpenGL space
	float MapX(float x) const;

	// Maps a Y coordinate to OpenGL space
	float MapY(float y) const;

	// Returns the index of the item at the specified logical position
	unsigned GetItemIndex(int x, int y) const;
	unsigned GetItemIndex(wxPoint position) const;


	// Returns the index of the item with the specified ID
	unsigned GetItemIndex(wxWallCtrlItemID itemID) const;


	// Returns the logical position of a specific index
	wxPoint GetItemPosition(unsigned index) const;

	// Returns the bounds of a rectangle with the correct aspect ratio centered in the original region
	// Precondition: right >= left and bottom >= top
	void AdjustCoordinates(wxRealRect &rect, const wxSize &itemSize) const;//(float & top, float & bottom, float & left, float & right, const wxSize & itemSize);

	// Attempts to move the scope so that the selected index is in its middle
	void AdjustScope()
	{
		// Find the position of the selected index
		wxPoint position = GetItemPosition(m_selectedIndex);

		// Try to center it in the scope. Note that we need only clip from the lower bounds.
		m_scopeOffsetX = wxMax(position.x - m_scopeSize.GetWidth()/2, 0);
		m_scopeOffsetY = wxMax(position.y - m_scopeSize.GetHeight()/2,0);
	}

	// Updates the camera each frame
	// TODO: Consider delta of time passed since last call
	void UpdateVectors();

	void UpdateCameraVector();

	// Updates the look vector to match its target (used to make the motion smooth)
	void UpdateLookVector();



	wxRealRect GetRawItemRect(int x, int y) const;

private:
	bool m_initialized;
	
	// The number of rows the surface will put the items in
	unsigned m_rowsCount;

	// The number of columns, this is deduced from m_rowsCount and the number of elements
	unsigned m_colsCount;

	// The index of the first item of the current scope. NOTE: Items are not sequential inside the scope
	unsigned m_firstItem;

	// This is the 2D window in number of items in each dimension
	wxSize m_scopeSize;

	// The coordinates of the scope's start
	unsigned m_scopeOffsetX;
	unsigned m_scopeOffsetY;

	// Layer variables used for partial loading and caching
	unsigned m_currentLayer;		// The current (square) layer that needs to be loaded
	unsigned m_nextLayerItem;		// The index of the next item to be loaded from the current layer
	wxRect m_currentLayerRect;		// The bounding rectangle of the current layer, the layer is at its perimeter
	wxPoint m_nextLayerItemPos;		// The position of the next item to be loaded.
	bool m_loadingNeeded;			// True when we need to load more items and cache them
	int m_renderCount;				// Used to load textures
	int m_rendersBeforeTextureLoad;	// The value after which an attempt to load a new texture will be made
	unsigned m_maxLoadingLayers;	// The max number of layers to pre-load around the currently selected item

	Int2IntMap m_texturesCache;		// Caches the IDs of loaded textures

	wxWallCtrlLoadingThread * m_loaderThread;	// This is the texture loading thread
	bool m_loadingInProgress;

	// The dimensions of each item as fractions of the unit size
	float m_itemWidth;
	float m_itemHeight;

	// Camera limits
	float m_nearLimit;
	float m_farLimit;

	// The position & direction vectors
	VectorType m_look;			// The look vector (where the camera is looking). This is a point not a direction
	VectorType m_targetLook;	// Where the look point should eventually be
	VectorType m_camera;		// The camera position
	VectorType m_targetCamera;	// Where the camera should eventually be
	VectorType m_up;			// A vector pointing to the up direction
	VectorType m_right;			// A vector looking to the right of the camera

	// Camera motion variables
	float m_lookDelta;
	float m_LookThreshold;

	float m_cameraDelta;
	float m_cameraThreshold;
	float m_cameraPanningDelta;

//	float m_cameraHzDelta;

	float m_defaultDistance;	// The default distance between camera and wall

	// Selection
	int m_selectedIndex;
	float m_selectionMargin;
	float m_selectionDepth;

	// Synchronization
	wxCriticalSection m_texturesCS;	// This CS guards the textures

};

// TODO: Move this class out
class wxWallCtrlLoadingThread: public wxThread
{
public:
	wxWallCtrlLoadingThread(wxWallCtrlPlaneSurface * surface): m_surface(surface)
	{		
	};
	// thread execution starts here
	virtual void *Entry()
	{
		while (m_surface->NeedLoading())
		{
			// Load one item
			m_surface->LoadNextLayerItemTexture();
			
			// See if we should die now
			if (TestDestroy())
			{
				// Loading is not complete, but we must stop here
				return NULL;
			}
		}
		m_surface->OnLoadingComplete();
		return NULL;
	}

	// called when the thread exits - whether it terminates normally or is
	// stopped with Delete() (but not when it is Kill()ed!)
	virtual void OnExit()
	{
		// Nothing here
	}

private:
	// Pointer to the surface where loading will occur
	//wxWallCtrlSurface * m_surface;
	// TODO: Use the abstract class instead
	wxWallCtrlPlaneSurface * m_surface;

};

#endif