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

#define BYTES_PER_PIXEL 4

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

	// Loads a texture from the DC into the supplied array
	// Precondition: Texture must be at least Width * Height * BYTES_PER_PIXEL
	void CreateTextureFromDC(wxMemoryDC &dc, GLubyte * texture, const wxSize& dcSize);

	// Loads a bitmap into a texture array
	// Precondition: Texture must be at least Width * Height * BYTES_PER_PIXEL
	void CreateTextureFromBitmap(wxBitmap bitmap, GLubyte * texture);

	// Returns the texture ID for a specific item, and if it does not exist, it gets loaded
	GLuint GetItemTexture(wxWallCtrlItemID itemID);

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

protected:
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

	// The index of the first item of the current scope. NOTE: Items are not sequential inside the scope
	unsigned m_firstItem;

	// This is the 2D window in number of items in each dimension
	wxSize m_scopeSize;

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
	float m_lookDelta;
	float m_LookThreshold;

	float m_cameraDelta;
	float m_cameraThreshold;
	float m_cameraPanningDelta;

//	float m_cameraHzDelta;

	float m_defaultDistance;	// The default distance between camera and wallf

	// Selection
	int m_selectedIndex;
	float m_selectionMargin;
	float m_selectionDepth;

};

#endif