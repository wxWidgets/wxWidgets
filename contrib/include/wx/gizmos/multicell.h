/////////////////////////////////////////////////////////////////////////////
// Name:        multicell.h
// Purpose:     provide two new classes for layout, wxMultiCellSizer and wxMultiCellCanvas
// Author:      Jonathan Bayer
// Modified by:
// Created:
// RCS-ID:      $Id:
// Copyright:   (c) Jonathan Bayer
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// This was inspired by the gbsizer class written by Alex Andruschak


#ifndef __WX_MULTICELL_H__
#define __WX_MULTICELL_H__

#ifdef __GNUG__
	#pragma interface "multicell.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
// The classes are derived from wxSizer
#include "wx/sizer.h"


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------
enum wxResizable
{
	wxNOT_RESIZABLE =           0x00,
	wxHORIZENTAL_RESIZABLE =    0x01,
	wxVERTICAL_RESIZABLE =      0x10,
	wxRESIZABLE =               0x11
};

//---------------------------------------------------------------------------
// classes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// wxMultiCellItemHandle
//---------------------------------------------------------------------------

class WXDLLEXPORT wxMultiCellItemHandle: public wxObject
{
	DECLARE_CLASS(wxMultiCellItemHandle);
protected:
	int             m_column;
	int             m_row;
	int             m_width;
	int             m_height;
	wxResizable     m_style;
	wxSize          m_fixedSize;
	int             m_alignment;
	wxSize          m_weight;

public:
	wxMultiCellItemHandle( int row, int column, int height = 1, int width = 1, wxSize size = wxDefaultSize, wxResizable style = wxNOT_RESIZABLE, wxSize weight = wxSize(1,1), int align = wxALIGN_NOT);
	wxMultiCellItemHandle( int row, int column, wxSize size, wxResizable style = wxNOT_RESIZABLE, wxSize weight = wxSize(1,1), int align = wxALIGN_NOT);
	wxMultiCellItemHandle( int row, int column, wxResizable style, wxSize weight = wxSize(1,1), int align = wxALIGN_NOT);
	wxMultiCellItemHandle( int row, int column, int align);
	int             GetColumn();
	int             GetRow();
	int             GetWidth();
	int             GetHeight();
	wxResizable     GetStyle();
	wxSize          GetLocalSize();
	int             GetAlignment();
	wxSize          GetWeight();

private:
	void Initialize( int row, int column, int height = 1, int width = 1, wxSize size = wxDefaultSize, wxResizable style = wxNOT_RESIZABLE, wxSize weight = wxSize(1,1), int align = wxALIGN_NOT);

};

//---------------------------------------------------------------------------
// wxMultiCellSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxMultiCellSizer : virtual public wxSizer
{
	DECLARE_CLASS(wxMultiCellSizer);

protected:
	wxSize m_cell_count;

public:
	wxMultiCellSizer(wxSize & size);
	wxMultiCellSizer(int rows, int cols);
	~wxMultiCellSizer();

	virtual void 	RecalcSizes();
	virtual wxSize 	CalcMin();
	bool 			SetDefaultCellSize(wxSize size);
	bool 			SetColumnWidth(int column, int colSize = 5, bool expandable = FALSE);
	bool 			SetRowHeight(int row, int rowSize = 5, bool expandable = FALSE);
	bool			EnableGridLines(wxWindow *win);
	bool			SetGridPen(wxPen *pen);
	void 			OnPaint(wxDC& dc);

private:
	void 			GetMinimums();
	int 			Sum(int *array, int x);

private:
	int 			*m_maxHeight;
	int 			*m_maxWidth;
	int 			*m_rowStretch;
	int 			*m_colStretch;
	wxSize 			**m_weights;
	wxSize			**m_minSizes;
	int				m_maxWeights;
	wxSize			m_defaultCellSize;
	wxWindow		*m_win; // usually used for debugging
	wxPen			*m_pen;

	void			DrawGridLines(wxDC& dc);
	void			Initialize(wxSize size);
};


// wxCell is used internally, so we don't need to declare it here

class wxCell;

//---------------------------------------------------------------------------
// wxMultiCellCanvas
//---------------------------------------------------------------------------

class wxMultiCellCanvas : public wxFlexGridSizer
{
public:
	wxMultiCellCanvas(wxWindow *parent, int numRows = 2, int numCols = 2);
	void Add(wxWindow *win, unsigned int row, unsigned int col);

	void Resize(int numRows, int numCols);
	int MaxRows()
	{
		return m_maxRows;
	};
	int MaxCols()
	{
		return m_maxCols;
	};
	void CalculateConstraints();
	void SetMinCellSize(const wxSize size)
	{
		m_minCellSize = size;
	};

private:
	wxWindow            *m_parent;
	unsigned int         m_maxRows, m_maxCols;

	wxSize              m_minCellSize;
	wxCell              **m_cells;
};

#endif



/*** End of File ***/
