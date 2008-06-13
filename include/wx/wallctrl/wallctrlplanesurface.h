/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlplanesurface.h
// Purpose:     A concrete plane (flat) surface for wxWallCtrl
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     08/06/2008
// RCS-ID:      
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "wallctrlsurface.h"
#include "gl/glut.h"
#include "wx/wallctrl/matrix.h"

#define BYTES_PER_PIXEL 4

// This is a default surface implementation. This is where all the geometric manipulation and rendering take place
class wxWallCtrlPlaneSurface :
	public wxWallCtrlSurface
{
public:
	// Precondition: dataSource must be valid
	wxWallCtrlPlaneSurface(wxWallCtrlDataSource * dataSource);
	virtual ~wxWallCtrlPlaneSurface(void);

	// Recalculates the item size from the number of items in the scope
	void UpdateItemSize();

	// Render the surface with all its items
	void Render(const wxSize & windowSize);

	// Loads a texture from the DC into the supplied array
	// Precondition: Texture must be at least Width * Height * BYTES_PER_PIXEL
	void CreateTextureFromDC(wxMemoryDC &dc, GLubyte * texture, const wxSize& dcSize);

	// Loads a bitmap into a texture array
	// Precondition: Texture must be at least Width * Height * BYTES_PER_PIXEL
	void CreateTextureFromBitmap(wxBitmap bitmap, GLubyte * texture);

	GLuint GetItemTexture(wxWallCtrlItemID itemID);

	// Initialize OpenGL for first use
	void InitializeGL();

	// This method renders all the items in the scope
	void RenderItems();

	// Sets the number of elements in a single frame (in both directions)
	void SetScopeSize(wxSize size);

	// Moves to the left with the specified delta factor
	void MoveLeft(float delta)
	{
		m_targetLook[0] -= delta;
	}
	// Moves to the right with the specified delta factor
	void MoveRight(float delta)
	{
		m_targetLook[0] += delta;
	}

	// TODO: Check if we need to put a distant limit
	void MoveIn(float delta)
	{
		m_targetCamera[2] -= delta;
		if (m_targetCamera[2] < m_nearLimit)
		{
			m_targetCamera[2] = m_nearLimit;
		}
	}

	// TODO: Check if we need to limit this
	void MoveOut(float delta)
	{
		m_targetCamera[2] += delta;
		if (m_targetCamera[2] > m_farLimit)
		{
			m_targetCamera[2] = m_farLimit;
		}
	}

protected:
	// Maps an X coordinate to OpenGL space
	float MapX(float x);

	// Maps a Y coordinate to OpenGL space
	float MapY(float y);

	// Returns the bounds of a rectangle with the correct aspect ratio centered in the original region
	// Precondition: right >= left and bottom >= top
	void AdjustCoordinates(float & top, float & bottom, float & left, float & right, const wxSize & itemSize);

	// Updates the camera each frame
	// TODO: Consider delta of time passed since last call
	void UpdateVectors()
	{
		// TODO: Move these into constructor
		m_lookHzDelta = 0.05;
		m_cameraHzDelta = 0.025;
		m_LookHzThreshold = m_cameraHzThreshold =0.1;
		m_cameraPanningDelta = 0.05;

		UpdateLookVector();

		// Update the camera based on the previous camera
		UpdateCameraVector();

	}

	void UpdateCameraVector()
	{
		// Check if we are far enough to require moving the look point. A threshold avoids oscillations
		if (m_look[0] < m_targetCamera[0] - m_LookHzThreshold)
		{
			m_targetCamera[0] -= m_cameraPanningDelta;
		}
		else if (m_look[0] > m_camera[0] + m_LookHzThreshold)
		{
			m_targetCamera[0] += m_cameraPanningDelta;
		}

		// Then update the camera vector to match its target
		// TODO: *Hz* should be replaced by something else
		for (int i=0; i < 3; ++i)
		{
			if (m_camera[i] < m_targetCamera[i] - m_cameraHzThreshold)
			{
				m_camera[i] += m_cameraHzDelta;
			}
			else if (m_camera[i] > m_targetCamera[i] + m_cameraHzThreshold)
			{
				m_camera[i] -= m_cameraHzDelta;
			}
		}

	}

	// Updates the look vector to match its target (used to make the motion smooth)
	void UpdateLookVector()
	{
		if (m_look[0] < m_targetLook[0] - m_LookHzThreshold)
		{
			m_look[0] += m_lookHzDelta;
		}
		else if (m_look[0] > m_targetLook[0] + m_LookHzThreshold)
		{
			m_look[0] -= m_lookHzDelta;
		}
	}
	


private:
	bool m_initialized;
	
	// The number of rows the surface will put the items in
	unsigned m_rowsCount;

	// The index of the first item of the current scope. NOTE: Items are not sequential inside the scope
	unsigned m_firstItem;

	// This is the 2D window in number of items in each dimension
	wxSize m_scopeSize;

	// A map to hold the texture names for items that were previously cached
	WX_DECLARE_HASH_MAP(int, int, wxIntegerHash, wxIntegerEqual, Int2IntMap);
	Int2IntMap texturesCache;

	// The dimensions of each item as fractions of the unit size
	float m_itemWidth;
	float m_itemHeight;

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
	float m_lookHzDelta;
	float m_LookHzThreshold;

	float m_cameraHzDelta;
	float m_cameraHzThreshold;
	float m_cameraPanningDelta;

//	float m_cameraHzDelta;

};
