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

class wxWallCtrlLoadingThread;

// This is a default surface implementation. This is where all the geometric manipulation and rendering take place
class wxWallCtrlPlaneSurface :
	public wxWallCtrlSurface
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrlPlaneSurface)
public:
	// Precondition: dataSource must be valid
	wxWallCtrlPlaneSurface(wxWallCtrl * parent);
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

	// Sets a data source for this surface
	virtual void SetDataSource(wxWallCtrlDataSource * dataSource);

	// Returns the point at the center of the specified item in OpenGL coordinates
	VectorType GetItemCenter(wxWallCtrlItemID itemID) const;

	// Seeking methods changes the selected item either by direct indexing or relative steps from current selection
	void Seek(wxWallCtrlItemID itemID);
	void SeekLeft();
	void SeekRight();
	void SeekUp();
	void SeekDown();

	// An internal handler called when texture loading is complete
	void OnLoadingComplete();

	// Spawns and runs the texture loading thread
	void CreateLoadingThread();

	// Destroys the texture loading thread
	void DestroyLoadingThread();

	// Returns the number of layers around the selected item that would be pre-loaded
	unsigned GetMaxLoadingLayers() const;

	// Must be preceeded by a call to CreateLoadingThread
	void RunLoadingThread();

protected:
	// Returns the number of columns in the specified layer
	int GetLayerWidth(int layer) const;
	
	// Returns the number of columns in the specified layer
	int GetLayerHeight(int layer) const;

	// Returns the total number of elements in the specified layer, some of which may be invalid
	int GetLayerItemsCount(int layer) const;

	wxRect GetLayerRect(int layer) const;

	// Returns true if the specified coordinates make sense
	bool IsValidPosition(wxPoint pos);

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
	void AdjustScope();


	// TODO: Consider using the delta of time passed since last call
	// Updates all the vectors each frame
	void UpdateVectors();

	// Updates the camera each frame
	void UpdateCameraVector();

	// Updates the look vector to match its target (used to make the motion smooth)
	void UpdateLookVector();

	// Returns the item rectangle without applying any translation to it
	wxRealRect GetRawItemRect(int x, int y) const;

	// Causes the control to reload all its content and reset its state
	virtual void Reload();

private:
	// Flag used to ensure OpenGL is initialized only once on first render.
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

	// The texture loading thread
	wxWallCtrlLoadingThread * m_loaderThread;	

	// True when we are still loading layers around the selected item
	bool m_loadingInProgress;

	// The dimensions of each item as fractions of the unit size
	float m_itemWidth;
	float m_itemHeight;

	// Camera depth limits
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
	float m_lookDelta;			// The value with which we update the look vector each time
	float m_LookThreshold;		// The comparison threshold for the look vector
	float m_cameraDelta;		// The value with which we update the camera vector each time
	float m_cameraThreshold;	// The comparison threshold for the camera vector
	float m_cameraPanningDelta;	// The value with which we update the X component of the target camera vector each time

	// The default distance between camera and wall
	float m_defaultDistance;	

	// Selection
	int m_selectedIndex;		// The selected item index
	float m_selectionMargin;	// How much will we stretch the selected item
	float m_selectionDepth;		// How far from the wall will we pull the selected item
};
#endif