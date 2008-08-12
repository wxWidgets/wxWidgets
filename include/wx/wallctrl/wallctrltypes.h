/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrltypes.h
// Purpose:     Holds the data types for the wall control
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     12/08/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WALLCTRLTYPES_H
#define WX_WALLCTRLTYPES_H

// The wxWallCtrl Item ID type
typedef unsigned wxWallCtrlItemID;

// The rendering types
enum wxWallCtrlRenderType
{
	wxWallCtrlRenderBitmap,
	wxWallCtrlRenderDC
};

// The information structur of wxWallCtrl items
struct wxWallCtrlItem
{
	wxSize size;
};

// The floating point version of wxRect
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

#endif