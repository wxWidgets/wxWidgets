/////////////////////////////////////////////////////////////////////////////
// Name:        gridg.cpp
// Purpose:     wxGenericGrid
// Author:      Julian Smart
// Modified by: Michael Bedward
//                Added edit in place facility, 20 Apr 1999
//                Added cursor key control, 29 Jun 1999
//              Gerhard Gruber
//                Added keyboard navigation, client data, other fixes
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "gridg.h"
    #pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/textctrl.h"
    #include "wx/settings.h"
#endif

#include <string.h>

#include "wx/string.h"

#include "wx/generic/gridg.h"

// Set to zero to use no double-buffering
#ifdef __WXMSW__
    #define wxUSE_DOUBLE_BUFFERING 1
#else
    #define wxUSE_DOUBLE_BUFFERING 0
#endif

#define wxGRID_DRAG_NONE       0
#define wxGRID_DRAG_LEFT_RIGHT 1
#define wxGRID_DRAG_UP_DOWN    2

IMPLEMENT_DYNAMIC_CLASS(wxGenericGrid, wxPanel)
IMPLEMENT_DYNAMIC_CLASS(wxGridEvent, wxEvent)

BEGIN_EVENT_TABLE(wxGenericGrid, wxPanel)
    EVT_SIZE(wxGenericGrid::OnSize)
    EVT_PAINT(wxGenericGrid::OnPaint)
    EVT_ERASE_BACKGROUND(wxGenericGrid::OnEraseBackground)
    EVT_MOUSE_EVENTS(wxGenericGrid::OnMouseEvent)
    EVT_TEXT(wxGRID_TEXT_CTRL, wxGenericGrid::OnText)
    EVT_TEXT(wxGRID_EDIT_IN_PLACE_TEXT_CTRL, wxGenericGrid::OnTextInPlace)
    EVT_TEXT_ENTER(wxGRID_TEXT_CTRL, wxGenericGrid::OnTextEnter)
    EVT_TEXT_ENTER(wxGRID_EDIT_IN_PLACE_TEXT_CTRL, wxGenericGrid::OnTextInPlaceEnter)
    EVT_COMMAND_SCROLL(wxGRID_HSCROLL, wxGenericGrid::OnGridScroll)
    EVT_COMMAND_SCROLL(wxGRID_VSCROLL, wxGenericGrid::OnGridScroll)

    // default wxGridEvent handlers
    EVT_GRID_SELECT_CELL(wxGenericGrid::_OnSelectCell)
    EVT_GRID_CREATE_CELL(wxGenericGrid::_OnCreateCell)
    EVT_GRID_CHANGE_LABELS(wxGenericGrid::_OnChangeLabels)
    EVT_GRID_CHANGE_SEL_LABEL(wxGenericGrid::_OnChangeSelectionLabel)
    EVT_GRID_CELL_CHANGE(wxGenericGrid::_OnCellChange)
    EVT_GRID_CELL_LCLICK(wxGenericGrid::_OnCellLeftClick)
    EVT_GRID_CELL_RCLICK(wxGenericGrid::_OnCellRightClick)
    EVT_GRID_LABEL_LCLICK(wxGenericGrid::_OnLabelLeftClick)
    EVT_GRID_LABEL_RCLICK(wxGenericGrid::_OnLabelRightClick)

END_EVENT_TABLE()


wxGenericGrid::wxGenericGrid()
{
  m_viewWidth = 0;
  m_viewHeight = 0;
  m_batchCount = 0;
  m_hScrollBar = (wxScrollBar *) NULL;
  m_vScrollBar = (wxScrollBar *) NULL;
  m_cellTextColour = *wxBLACK;
  m_cellBackgroundColour = *wxWHITE;
  m_labelTextColour = *wxBLACK;
//  m_labelBackgroundColour = *wxLIGHT_GREY;
  m_labelBackgroundColour = wxSystemSettings::GetSystemColour( wxSYS_COLOUR_BTNFACE );
  m_labelBackgroundBrush = wxNullBrush;
  m_labelTextFont = wxNullFont;
  m_cellTextFont = wxNullFont;
  m_textItem = (wxTextCtrl *) NULL;
  m_currentRectVisible = FALSE;
  m_editable = TRUE;

  m_editInPlace = TRUE;
  m_inOnTextInPlace = FALSE;

#if defined(__WIN95__)
  m_scrollWidth = wxSystemSettings::GetSystemMetric(wxSYS_VSCROLL_X);
#elif defined(__WXGTK__)
  m_scrollWidth = wxSystemSettings::GetSystemMetric(wxSYS_VSCROLL_X);
#else
  m_scrollWidth = 16;
#endif
  m_dragStatus = wxGRID_DRAG_NONE;
  m_dragRowOrCol = 0;
  m_dragStartPosition = 0;
  m_dragLastPosition = 0;
  m_divisionPen = wxNullPen;
  m_leftOfSheet = wxGRID_DEFAULT_SHEET_LEFT;
  m_topOfSheet = wxGRID_DEFAULT_SHEET_TOP;
  m_cellHeight = wxGRID_DEFAULT_CELL_HEIGHT;
  m_totalGridWidth = 0;
  m_totalGridHeight = 0;
  m_colWidths = (short *) NULL;
  m_rowHeights = (short *) NULL;
  m_verticalLabelWidth = wxGRID_DEFAULT_VERTICAL_LABEL_WIDTH;
  m_horizontalLabelHeight = wxGRID_DEFAULT_HORIZONAL_LABEL_HEIGHT;
  m_verticalLabelAlignment = wxCENTRE;
  m_horizontalLabelAlignment = wxCENTRE;
  m_editControlPosition.x = wxGRID_DEFAULT_EDIT_X;
  m_editControlPosition.y = wxGRID_DEFAULT_EDIT_Y;
  m_editControlPosition.width = wxGRID_DEFAULT_EDIT_WIDTH;
  m_editControlPosition.height = wxGRID_DEFAULT_EDIT_HEIGHT;
  m_wCursorRow = 0;
  m_wCursorColumn = 0;
  m_scrollPosX = 0;
  m_scrollPosY = 0;
  m_editCreated = FALSE;
  m_totalRows = 0;
  m_totalCols = 0;
  m_gridCells = (wxGridCell ***) NULL;
  m_rowLabelCells = (wxGridCell **) NULL;
  m_colLabelCells = (wxGridCell **) NULL;
  m_textItem = (wxTextCtrl *) NULL;
}

bool wxGenericGrid::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
{
  m_viewWidth = 0;
  m_viewHeight = 0;
  m_batchCount = 0;
  m_editingPanel = (wxPanel *) NULL;
  m_hScrollBar = (wxScrollBar *) NULL;
  m_vScrollBar = (wxScrollBar *) NULL;
  m_cellTextColour = *wxBLACK;
  m_cellBackgroundColour = *wxWHITE;
  m_labelTextColour = *wxBLACK;
//  m_labelBackgroundColour = *wxLIGHT_GREY;
  m_labelBackgroundColour = wxSystemSettings::GetSystemColour( wxSYS_COLOUR_BTNFACE );
  m_labelBackgroundBrush = wxNullBrush;
  m_labelTextFont = * wxTheFontList->FindOrCreateFont(10, wxSWISS, wxNORMAL, wxBOLD);
  m_cellTextFont = * wxTheFontList->FindOrCreateFont(10, wxSWISS, wxNORMAL, wxNORMAL);
  m_textItem = (wxTextCtrl *) NULL;
  m_currentRectVisible = FALSE;
  m_editable = TRUE;
#if defined(__WIN95__)
  m_scrollWidth = wxSystemSettings::GetSystemMetric(wxSYS_VSCROLL_X);
#elif defined(__WXGTK__)
  m_scrollWidth = wxSystemSettings::GetSystemMetric(wxSYS_VSCROLL_X);
#else
  m_scrollWidth = 16;
#endif
  m_dragStatus = wxGRID_DRAG_NONE;
  m_dragRowOrCol = 0;
  m_dragStartPosition = 0;
  m_dragLastPosition = 0;
  m_divisionPen = * wxThePenList->FindOrCreatePen("LIGHT GREY", 1, wxSOLID);
  m_doubleBufferingBitmap = (wxBitmap *) NULL;

  if (!m_horizontalSashCursor.Ok())
  {
    m_horizontalSashCursor = wxCursor(wxCURSOR_SIZEWE);
    m_verticalSashCursor = wxCursor(wxCURSOR_SIZENS);
  }

  SetLabelBackgroundColour(m_labelBackgroundColour);

  m_leftOfSheet = wxGRID_DEFAULT_SHEET_LEFT;
  m_topOfSheet = wxGRID_DEFAULT_SHEET_TOP;
  m_cellHeight = wxGRID_DEFAULT_CELL_HEIGHT;
  m_totalGridWidth = 0;
  m_totalGridHeight = 0;
  m_colWidths = (short *) NULL;
  m_rowHeights = (short *) NULL;

  m_verticalLabelWidth = wxGRID_DEFAULT_VERTICAL_LABEL_WIDTH;
  m_horizontalLabelHeight = wxGRID_DEFAULT_HORIZONAL_LABEL_HEIGHT;
  m_verticalLabelAlignment = wxCENTRE;
  m_horizontalLabelAlignment = wxCENTRE;
  m_editControlPosition.x = wxGRID_DEFAULT_EDIT_X;
  m_editControlPosition.y = wxGRID_DEFAULT_EDIT_Y;
  m_editControlPosition.width = wxGRID_DEFAULT_EDIT_WIDTH;
  m_editControlPosition.height = wxGRID_DEFAULT_EDIT_HEIGHT;

  m_wCursorRow = 0;
  m_wCursorColumn = 0;

  m_scrollPosX = 0;
  m_scrollPosY = 0;

  /* Store the rect. coordinates for the current cell */
  SetCurrentRect(m_wCursorRow, m_wCursorColumn);

  m_editCreated = FALSE;

  m_totalRows = 0;
  m_totalCols = 0;
  m_gridCells = (wxGridCell ***) NULL;
  m_rowLabelCells = (wxGridCell **) NULL;
  m_colLabelCells = (wxGridCell **) NULL;
  m_textItem = (wxTextCtrl *) NULL;

  wxPanel::Create(parent, id, pos, size, style, name);

  m_editingPanel = new wxPanel(this);

  m_textItem = new wxTextCtrl(m_editingPanel, wxGRID_TEXT_CTRL, "",
                              wxPoint(m_editControlPosition.x, m_editControlPosition.y),
                              wxSize(m_editControlPosition.width, -1),
                              wxTE_PROCESS_ENTER);
  m_textItem->Show(TRUE);
  m_textItem->SetFocus();
  int controlW, controlH;

  m_textItem->GetSize(&controlW, &controlH);
  m_editControlPosition.height = controlH;

  m_topOfSheet = m_editControlPosition.y + controlH + 2;

  m_editCreated = TRUE;

  m_hScrollBar = new wxScrollBar(this, wxGRID_HSCROLL, wxPoint(0, 0), wxSize(20, 100), wxHORIZONTAL);
  m_vScrollBar = new wxScrollBar(this, wxGRID_VSCROLL, wxPoint(0, 0), wxSize(100, 20), wxVERTICAL);

//  SetSize(pos.x, pos.y, size.x, size.y);

  m_inPlaceTextItem = new wxTextCtrl( (wxPanel*)this, wxGRID_EDIT_IN_PLACE_TEXT_CTRL, "",
                                      wxPoint( m_currentRect.x-2, m_currentRect.y-2 ),
                                      wxSize( m_currentRect.width+4, m_currentRect.height+4 ),
                                      wxNO_BORDER | wxTE_PROCESS_ENTER );
  m_inPlaceTextItem->Show(TRUE);
  m_inPlaceTextItem->SetFocus();

  return TRUE;
}

wxGenericGrid::~wxGenericGrid()
{
  ClearGrid();
}

void wxGenericGrid::ClearGrid()
{
  int i,j;
  if (m_gridCells)
  {
    for (i = 0; i < m_totalRows; i++)
    {
      for (j = 0; j < m_totalCols; j++)
        if (m_gridCells[i][j])
          delete m_gridCells[i][j];
      delete[] m_gridCells[i];
    }
    delete[] m_gridCells;
    m_gridCells = (wxGridCell ***) NULL;
  }
  if (m_colWidths)
    delete[] m_colWidths;
  m_colWidths = (short *) NULL;
  if (m_rowHeights)
    delete[] m_rowHeights;
  m_rowHeights = (short *) NULL;

  if (m_rowLabelCells)
  {
    for (i = 0; i < m_totalRows; i++)
      delete m_rowLabelCells[i];
    delete[] m_rowLabelCells;
    m_rowLabelCells = (wxGridCell **) NULL;
  }
  if (m_colLabelCells)
  {
    for (i = 0; i < m_totalCols; i++)
      delete m_colLabelCells[i];
    delete[] m_colLabelCells;
    m_colLabelCells = (wxGridCell **) NULL;
  }
  if (m_doubleBufferingBitmap)
  {
    delete m_doubleBufferingBitmap;
    m_doubleBufferingBitmap = (wxBitmap *) NULL;
  }
}

bool wxGenericGrid::CreateGrid(int nRows, int nCols, wxString **cellValues, short *widths,
     short defaultWidth, short defaultHeight)
{
  m_totalRows = nRows;
  m_totalCols = nCols;

  int i,j;
  m_colWidths = new short[nCols];
  m_rowHeights = new short[nRows];
  for (i = 0; i < nCols; i++)
    if (widths)
      m_colWidths[i] = widths[i];
    else
      m_colWidths[i] = defaultWidth;
  for (i = 0; i < nRows; i++)
    m_rowHeights[i] = defaultHeight;

  m_gridCells = new wxGridCell **[nRows];

  for (i = 0; i < nRows; i++)
    m_gridCells[i] = new wxGridCell *[nCols];

  for (i = 0; i < nRows; i++)
    for (j = 0; j < nCols; j++)
      if (cellValues)
      {
        //m_gridCells[i][j] = OnCreateCell();
        wxGridEvent g_evt(GetId(), wxEVT_GRID_CREATE_CELL, this, i, j);
        GetEventHandler()->ProcessEvent(g_evt);
        m_gridCells[i][j] = g_evt.m_cell;
        m_gridCells[i][j]->SetTextValue(cellValues[i][j]);
      }
      else
        m_gridCells[i][j] = (wxGridCell *) NULL;

  m_rowLabelCells = new wxGridCell *[nRows];
  for (i = 0; i < nRows; i++)
    m_rowLabelCells[i] = new wxGridCell(this);
  m_colLabelCells = new wxGridCell *[nCols];
  for (i = 0; i < nCols; i++)
    m_colLabelCells[i] = new wxGridCell(this);

  m_wCursorRow = m_wCursorColumn = 0;
  SetCurrentRect(0, 0);

  // Need to determine various dimensions
  UpdateDimensions();

  // Number of 'lines'
  int objectSizeX = m_totalCols;
  int pageSizeX = 1;
  int viewLengthX = m_totalCols;

/*
  m_hScrollBar->SetViewLength(viewLengthX);
  m_hScrollBar->SetObjectLength(objectSizeX);
  m_hScrollBar->SetPageSize(pageSizeX);
*/
  m_hScrollBar->SetScrollbar(m_hScrollBar->GetThumbPosition(), pageSizeX, objectSizeX, viewLengthX);

  int objectSizeY = m_totalRows;
  int pageSizeY = 1;
  int viewLengthY = m_totalRows;

/*
  m_vScrollBar->SetViewLength(viewLengthY);
  m_vScrollBar->SetObjectLength(objectSizeY);
  m_vScrollBar->SetPageSize(pageSizeY);
*/

  m_vScrollBar->SetScrollbar(m_vScrollBar->GetThumbPosition(), pageSizeY, objectSizeY, viewLengthY);

  AdjustScrollbars();

  //OnChangeLabels();
  wxGridEvent g_evt(GetId(), wxEVT_GRID_CHANGE_LABELS, this);
  GetEventHandler()->ProcessEvent(g_evt);

  //OnChangeSelectionLabel();
  wxGridEvent g_evt2(GetId(), wxEVT_GRID_CHANGE_SEL_LABEL, this);
  GetEventHandler()->ProcessEvent(g_evt2);

  return TRUE;
}

// Need to determine various dimensions
void wxGenericGrid::UpdateDimensions()
{
  int canvasWidth, canvasHeight;
  GetSize(&canvasWidth, &canvasHeight);

  if (m_editCreated && m_editable)
  {
    int controlW, controlH;
    GetTextItem()->GetSize(&controlW, &controlH);
    m_topOfSheet = m_editControlPosition.y + controlH + 2;
  }
  else
    m_topOfSheet = 0;
  m_rightOfSheet = m_leftOfSheet + m_verticalLabelWidth;
  int i;
  for (i = m_scrollPosX; i < m_totalCols; i++)
  {
    if (m_rightOfSheet > canvasWidth)
      break;
    else
      m_rightOfSheet += m_colWidths[i];
  }
  m_bottomOfSheet = m_topOfSheet + m_horizontalLabelHeight;
  for (i = m_scrollPosY; i < m_totalRows; i++)
  {
    if (m_bottomOfSheet > canvasHeight)
      break;
    else
      m_bottomOfSheet += m_rowHeights[i];
  }

  m_totalGridWidth = m_leftOfSheet + m_verticalLabelWidth;
  for (i = 0; i < m_totalCols; i++)
  {
    m_totalGridWidth += m_colWidths[i];
  }
  m_totalGridHeight = m_topOfSheet + m_horizontalLabelHeight;
  for (i = 0; i < m_totalRows; i++)
  {
    m_totalGridHeight += m_rowHeights[i];
  }
}

wxGridCell *wxGenericGrid::GetCell(int row, int col) const
{
  if (!m_gridCells)
    return (wxGridCell *) NULL;

  if ((row >= m_totalRows) || (col >= m_totalCols))
    return (wxGridCell *) NULL;

  wxGridCell *cell = m_gridCells[row][col];
  if (!cell)
  {
    // m_gridCells[row][col] = OnCreateCell();
    wxGridEvent g_evt(GetId(), wxEVT_GRID_CREATE_CELL, (wxGenericGrid*) this, row, col);
    GetEventHandler()->ProcessEvent(g_evt);
    m_gridCells[row][col] = g_evt.m_cell;
    return m_gridCells[row][col];
  }
  else
    return cell;
}

void wxGenericGrid::SetGridClippingRegion(wxDC *dc)
{
  int m_scrollWidthHoriz = 0;
  int m_scrollWidthVert = 0;
  int cw, ch;
  GetClientSize(&cw, &ch);

  if (m_hScrollBar && m_hScrollBar->IsShown())
    m_scrollWidthHoriz = m_scrollWidth;
  if (m_vScrollBar && m_vScrollBar->IsShown())
    m_scrollWidthVert = m_scrollWidth;

  // Don't paint over the scrollbars
  dc->SetClippingRegion(m_leftOfSheet, m_topOfSheet,
     cw - m_scrollWidthVert - m_leftOfSheet, ch - m_scrollWidthHoriz - m_topOfSheet);
}

void wxGenericGrid::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  int w, h;
  GetClientSize(&w, &h);

  bool useDoubleBuffering = (bool) wxUSE_DOUBLE_BUFFERING;
  if (useDoubleBuffering)
  {
    // Reuse the old bitmap if possible

    if (!m_doubleBufferingBitmap ||
       (m_doubleBufferingBitmap->GetWidth() < w || m_doubleBufferingBitmap->GetHeight() < h))
    {
        if (m_doubleBufferingBitmap)
            delete m_doubleBufferingBitmap;
        m_doubleBufferingBitmap = new wxBitmap(w, h);
    }
    if (!m_doubleBufferingBitmap || !m_doubleBufferingBitmap->Ok())
    {
        // If we couldn't create a new bitmap, perhaps because resources were low,
        // then don't complain, just don't double-buffer
        if (m_doubleBufferingBitmap)
            delete m_doubleBufferingBitmap;
        m_doubleBufferingBitmap = (wxBitmap *) NULL;
        useDoubleBuffering = FALSE;
    }
  }

  if (useDoubleBuffering)
  {
    wxPaintDC paintDC(this);
    wxMemoryDC dc(& paintDC);
    dc.SelectObject(* m_doubleBufferingBitmap);

    PaintGrid(dc);

    int vertScrollBarWidth = m_scrollWidth;
    int horizScrollBarHeight = m_scrollWidth;
    if (m_vScrollBar && !m_vScrollBar->IsShown())
      vertScrollBarWidth = 0;
    if (m_hScrollBar && !m_hScrollBar->IsShown())
      horizScrollBarHeight = 0;

    paintDC.Blit(m_leftOfSheet, m_topOfSheet, w - vertScrollBarWidth - m_leftOfSheet, h - horizScrollBarHeight - m_topOfSheet,
      &dc, m_leftOfSheet, m_topOfSheet, wxCOPY);

    dc.SelectObject(wxNullBitmap);
  }
  else
  {
    wxPaintDC dc(this);
    PaintGrid(dc);
  }
}

void wxGenericGrid::PaintGrid(wxDC& dc)
{
    dc.BeginDrawing();
    dc.SetOptimization(FALSE);

    SetGridClippingRegion(& dc);

    DrawLabelAreas(& dc);

    DrawEditableArea(& dc);
    DrawColumnLabels(& dc);
    DrawRowLabels(& dc);
    DrawCells(& dc);
    DrawGridLines(& dc);

    /* Hilight present cell */
    SetCurrentRect(m_wCursorRow, m_wCursorColumn);
    if (m_currentRectVisible && !(m_editable && m_editInPlace))
      HighlightCell(& dc);

    dc.DestroyClippingRegion();
    dc.SetOptimization(TRUE);
    dc.EndDrawing();
}

// Erase (some of) the background.
// Currently, a Windows-only optimisation.
void wxGenericGrid::OnEraseBackground(wxEraseEvent& WXUNUSED(event) )
{
    wxClientDC dc(this);
    dc.BeginDrawing();
    dc.SetOptimization(FALSE);

    int w, h;
    GetClientSize(& w, & h);
    dc.SetBrush(*wxLIGHT_GREY_BRUSH);
    dc.SetPen(*wxLIGHT_GREY_PEN);

    if (m_hScrollBar && m_hScrollBar->IsShown() && m_vScrollBar && m_vScrollBar->IsShown())
    {
        dc.DrawRectangle(w - m_scrollWidth, h - m_scrollWidth, m_scrollWidth, m_scrollWidth);
    }

    dc.SetOptimization(TRUE);
    dc.EndDrawing();
}


void wxGenericGrid::DrawLabelAreas(wxDC *dc)
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  dc->SetPen(*wxTRANSPARENT_PEN);
//  dc->SetBrush(*dc->GetBackground());

  // Should blank out any area which isn't going to be painted over.
//  dc->DrawRectangle(m_leftOfSheet, m_bottomOfSheet, cw - m_leftOfSheet, ch - m_bottomOfSheet);
//  dc->DrawRectangle(m_rightOfSheet, m_topOfSheet, cw - m_rightOfSheet, ch - m_topOfSheet);

  // Paint the label areas
  dc->SetBrush(m_labelBackgroundBrush);
//  dc->DrawRectangle(m_leftOfSheet, m_topOfSheet, m_rightOfSheet - m_leftOfSheet + 1, m_horizontalLabelHeight + 1);
  dc->DrawRectangle(m_leftOfSheet, m_topOfSheet, cw-m_leftOfSheet, m_horizontalLabelHeight + 1);
//  dc->DrawRectangle(m_leftOfSheet, m_topOfSheet, m_verticalLabelWidth + 1, m_bottomOfSheet - m_topOfSheet + 1);
  dc->DrawRectangle(m_leftOfSheet, m_topOfSheet, m_verticalLabelWidth + 1, ch-m_topOfSheet);
}

void wxGenericGrid::DrawEditableArea(wxDC *dc)
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  dc->SetPen(*wxTRANSPARENT_PEN);
  dc->SetBrush(*wxTheBrushList->FindOrCreateBrush(m_cellBackgroundColour, wxSOLID));
//  dc->DrawRectangle(m_leftOfSheet+m_verticalLabelWidth, m_topOfSheet+m_horizontalLabelHeight,
//      m_rightOfSheet-(m_leftOfSheet+m_verticalLabelWidth) + 1, m_bottomOfSheet - (m_topOfSheet+m_horizontalLabelHeight) + 1);
  dc->DrawRectangle(m_leftOfSheet+m_verticalLabelWidth, m_topOfSheet+m_horizontalLabelHeight,
      cw-(m_leftOfSheet+m_verticalLabelWidth), ch - (m_topOfSheet+m_horizontalLabelHeight));
}

void wxGenericGrid::DrawGridLines(wxDC *dc)
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  int i;

  if (m_divisionPen.Ok())
  {
    dc->SetPen(m_divisionPen);

    int heightCount = m_topOfSheet + m_horizontalLabelHeight;

    // Draw horizontal grey lines for cells
    for (i = m_scrollPosY; i < (m_totalRows+1); i++)
    {
      if (heightCount > ch)
        break;
      else
      {
        dc->DrawLine(m_leftOfSheet, heightCount,
                   cw, heightCount);
        if (i < m_totalRows)
          heightCount += m_rowHeights[i];
      }
    }
  }

  if (m_verticalLabelWidth > 0)
  {
    dc->SetPen(*wxBLACK_PEN);

    // Draw horizontal black lines for row labels
    int heightCount = m_topOfSheet + m_horizontalLabelHeight;
    for (i = m_scrollPosY; i < (m_totalRows+1); i++)
    {
      if (heightCount > ch)
        break;
      else
      {
        dc->DrawLine(m_leftOfSheet, heightCount,
                     m_verticalLabelWidth, heightCount);
        if (i < m_totalRows)
          heightCount += m_rowHeights[i];
      }
    }
    // Draw a black vertical line for row number cells
    dc->DrawLine(m_leftOfSheet + m_verticalLabelWidth, m_topOfSheet,
      m_leftOfSheet + m_verticalLabelWidth, ch);
    // First vertical line
    dc->DrawLine(m_leftOfSheet, m_topOfSheet, m_leftOfSheet, ch);

    dc->SetPen(*wxWHITE_PEN);

    // Draw highlights on row labels
    heightCount = m_topOfSheet + m_horizontalLabelHeight;
    for (i = m_scrollPosY; i < m_totalRows; i++)
    {
      if (heightCount > ch)
        break;
      else
      {
        dc->DrawLine(m_leftOfSheet+1, heightCount+1,
                     m_verticalLabelWidth, heightCount+1);
        dc->DrawLine(m_leftOfSheet+1, heightCount+1,
                     m_leftOfSheet+1, heightCount + m_rowHeights[i] - 1);
        heightCount += m_rowHeights[i];
      }
    }
    // Last one - down to the floor.
    dc->DrawLine(m_leftOfSheet+1, heightCount+1,
                 m_verticalLabelWidth, heightCount+1);
    dc->DrawLine(m_leftOfSheet+1, heightCount+1,
                   m_leftOfSheet+1, ch);

  }

  if (m_divisionPen.Ok())
  {
    dc->SetPen(m_divisionPen);

    // Draw vertical grey lines for cells
    int widthCount = m_leftOfSheet + m_verticalLabelWidth;
    for (i = m_scrollPosX; i < m_totalCols; i++)
    {
      if (widthCount > cw)
        break;
      else
      {
        // Skip the first one
        if (i != m_scrollPosX)
        {
         dc->DrawLine(widthCount, m_topOfSheet + m_horizontalLabelHeight,
                       widthCount, m_bottomOfSheet);
        }
        widthCount += m_colWidths[i];
      }
    }
    // Last one
    dc->DrawLine(widthCount, m_topOfSheet + m_horizontalLabelHeight,
                    widthCount, m_bottomOfSheet);
  }

  dc->SetPen(*wxBLACK_PEN);

  // Draw two black horizontal lines for column number cells
  dc->DrawLine(
          m_leftOfSheet, m_topOfSheet,
          cw, m_topOfSheet);
  dc->DrawLine(m_leftOfSheet,  m_topOfSheet + m_horizontalLabelHeight,
               cw, m_topOfSheet + m_horizontalLabelHeight);

  if (m_horizontalLabelHeight > 0)
  {
    int widthCount = m_leftOfSheet + m_verticalLabelWidth;

    // Draw black vertical lines for column number cells
    for (i = m_scrollPosX; i < m_totalCols; i++)
    {
      if (widthCount > cw)
        break;
      else
      {
        dc->DrawLine(widthCount, m_topOfSheet,
                      widthCount, m_topOfSheet + m_horizontalLabelHeight);
        widthCount += m_colWidths[i];
      }
    }

    // Last one
    dc->DrawLine(widthCount, m_topOfSheet,
                    widthCount, m_topOfSheet + m_horizontalLabelHeight);

    // Draw highlights
    dc->SetPen(*wxWHITE_PEN);
    widthCount = m_leftOfSheet + m_verticalLabelWidth;

    for (i = m_scrollPosX; i < m_totalCols; i++)
    {
      if (widthCount > cw)
        break;
      else
      {
        dc->DrawLine(widthCount+1, m_topOfSheet+1,
                     widthCount+m_colWidths[i], m_topOfSheet+1);
        dc->DrawLine(widthCount+1, m_topOfSheet+1,
                     widthCount+1, m_topOfSheet+m_horizontalLabelHeight);
        widthCount += m_colWidths[i];
      }
    }
    // Last one - to the right side of the canvas.
    dc->DrawLine(widthCount+1, m_topOfSheet+1,
                   cw, m_topOfSheet+1);
    dc->DrawLine(widthCount+1, m_topOfSheet+1,
                   widthCount+1, m_topOfSheet+m_horizontalLabelHeight);

  }
}

void wxGenericGrid::DrawColumnLabels(wxDC *dc)
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  if (m_horizontalLabelHeight == 0)
    return;

  int i;
  wxRect rect;

  // Draw letters for columns
  rect.y = m_topOfSheet + 1;
  rect.height = m_horizontalLabelHeight - 1;

  dc->SetTextBackground(m_labelBackgroundColour);
  dc->SetBackgroundMode(wxTRANSPARENT);
//  dc->SetTextForeground(m_labelTextColour);

  int widthCount = m_leftOfSheet + m_verticalLabelWidth;
  for (i = m_scrollPosX; i < m_totalCols; i++)
  {
     if (widthCount > cw)
       break;
     else
     {
       rect.x = 1 + widthCount;
       rect.width = m_colWidths[i];
       DrawColumnLabel(dc, &rect, i);

       widthCount += m_colWidths[i];
     }
  }
}

void wxGenericGrid::DrawColumnLabel(wxDC *dc, wxRect *rect, int col)
{
  wxGridCell *cell = GetLabelCell(wxHORIZONTAL, col);
  if (cell)
  {
    wxRect rect2;
    rect2 = *rect;
    rect2.x += 3;
    rect2.y += 2;
    rect2.width -= 5;
    rect2.height -= 4;
    dc->SetTextForeground(GetLabelTextColour());
    dc->SetFont(GetLabelTextFont());
        if ( !cell->GetTextValue().IsNull() )
            DrawTextRect(dc, cell->GetTextValue(), &rect2, GetLabelAlignment(wxHORIZONTAL));
  }
}

void wxGenericGrid::DrawRowLabels(wxDC *dc)
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  if (m_verticalLabelWidth == 0)
    return;

  int i;
  wxRect rect;

  // Draw numbers for rows
  rect.x = m_leftOfSheet;
  rect.width = m_verticalLabelWidth;

  int heightCount = m_topOfSheet + m_horizontalLabelHeight;

  dc->SetTextBackground(m_labelBackgroundColour);
  dc->SetBackgroundMode(wxTRANSPARENT);

  for (i = m_scrollPosY; i < m_totalRows; i++)
  {
     if (heightCount > ch)
       break;
     else
     {
       rect.y = 1 + heightCount;
       rect.height = m_rowHeights[i];
       DrawRowLabel(dc, &rect, i);

       heightCount += m_rowHeights[i];
     }
  }
}

void wxGenericGrid::DrawRowLabel(wxDC *dc, wxRect *rect, int row)
{
  wxGridCell *cell = GetLabelCell(wxVERTICAL, row);
  if (cell)
  {
    wxRect rect2;
    rect2 = *rect;
    rect2.x += 3;
    rect2.y += 2;
    rect2.width -= 5;
    rect2.height -= 4;
    dc->SetTextForeground(GetLabelTextColour());
    dc->SetFont(GetLabelTextFont());
        if ( !cell->GetTextValue().IsNull() )
            DrawTextRect(dc, cell->GetTextValue(), &rect2, GetLabelAlignment(wxVERTICAL));
  }
}

void wxGenericGrid::DrawCells(wxDC *dc)
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  int i,j;

  // Draw value corresponding to each cell
  for (i = m_scrollPosY; i < m_totalRows; i++)
  {
    for (j = m_scrollPosX; j < m_totalCols; j++)
    {
      SetCurrentRect(i, j, cw, ch);
      if (m_currentRectVisible)
      {
        DrawCellBackground(dc, &m_currentRect, i, j);
        DrawCellValue(dc, &m_currentRect, i, j);
      }
      if (m_currentRect.x > cw)
        break;
    }
    if (m_currentRect.y > ch)
      break;
  }
  dc->SetBackgroundMode(wxSOLID);
  dc->SetPen(*wxBLACK_PEN);
}

void wxGenericGrid::DrawCellBackground(wxDC *dc, wxRect *rect, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    dc->SetBrush(cell->GetBackgroundBrush());
    dc->SetPen(*wxTRANSPARENT_PEN);

#if 0    // In wxWin 2.0 the dc code is exact. RR.
#ifdef __WXMOTIF__
    dc->DrawRectangle(rect->x+1, rect->y+1, rect->width-1, rect->height-1);
#else
    dc->DrawRectangle(rect->x+1, rect->y+1, rect->width, rect->height);
#endif
#endif

    dc->DrawRectangle(rect->x+1, rect->y+1, rect->width-1, rect->height-1);

    dc->SetPen(*wxBLACK_PEN);
  }
}

void wxGenericGrid::DrawCellValue(wxDC *dc, wxRect *rect, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    wxBitmap *bitmap = cell->GetCellBitmap();
    wxRect rect2;
    rect2 = *rect;
    rect2.x += 3;
    rect2.y += 2;
    rect2.width -= 5;
    rect2.height -= 4;

    if (bitmap)
    {
      DrawBitmapRect(dc, bitmap, &rect2, cell->GetAlignment());
    }
    else
    {
      dc->SetBackgroundMode(wxTRANSPARENT);
      dc->SetTextForeground(cell->GetTextColour());
      dc->SetFont(cell->GetFont());

          if ( !cell->GetTextValue().IsNull() )
              DrawTextRect(dc, cell->GetTextValue(), &rect2, cell->GetAlignment());
    }
  }
}

void wxGenericGrid::AdjustScrollbars()
{
  int cw, ch;
  GetClientSize(&cw, &ch);

  // We find the view size by seeing how many rows/cols fit on
  // the current view.
  // BUT... this means that the scrollbar should be adjusted every time
  // it's scrolled, as well as when sized, because with variable size rows/cols,
  // the number of rows/col visible on the view differs according to what bit
  // you're looking at. The object length is always the same, but the
  // view length differs.

  // Since this may not be known until the end of this function, we should probably call AdjustScrollbars
  // twice.
  int vertScrollBarWidth = m_scrollWidth;
  int horizScrollBarHeight = m_scrollWidth;
  if (m_vScrollBar && !m_vScrollBar->IsShown())
    vertScrollBarWidth = 0;
  if (m_hScrollBar && !m_hScrollBar->IsShown())
    horizScrollBarHeight = 0;

  int noHorizSteps = 0;
  int noVertSteps = 0;

  if (m_totalGridWidth + vertScrollBarWidth > cw)
  {
    int widthCount = 0;

    int i;
    int nx = 0;
    for (i = m_scrollPosX ; i < m_totalCols; i++)
    {
      widthCount += m_colWidths[i];
      // A partial bit doesn't count, we still have to scroll to see the
      // rest of it
      if (widthCount + m_leftOfSheet + m_verticalLabelWidth > (cw-vertScrollBarWidth))
        break;
      else
           nx ++;
    }

    noHorizSteps += nx;
  }
  m_viewWidth = noHorizSteps;

  if (m_totalGridHeight + horizScrollBarHeight > ch)
  {
    int heightCount = 0;

    int i;
    int ny = 0;
    for (i = m_scrollPosY ; i < m_totalRows; i++)
    {
      heightCount += m_rowHeights[i];
      // A partial bit doesn't count, we still have to scroll to see the
      // rest of it
      if (heightCount + m_topOfSheet + m_horizontalLabelHeight > (ch-horizScrollBarHeight))
        break;
      else
        ny ++;
    }

    noVertSteps += ny;
  }

  m_viewHeight = noVertSteps;

  if (m_totalGridWidth + vertScrollBarWidth <= cw)
  {
    if ( m_hScrollBar )
        m_hScrollBar->Show(FALSE);
    SetScrollPosX(0);
  }
  else
  {
      if ( m_hScrollBar )
          m_hScrollBar->Show(TRUE);
  }

  if (m_totalGridHeight + horizScrollBarHeight <= ch)
  {
      if ( m_vScrollBar )
          m_vScrollBar->Show(FALSE);
      SetScrollPosY(0);
  }
  else
  {
      if ( m_vScrollBar )
          m_vScrollBar->Show(TRUE);
  }

  UpdateDimensions(); // Necessary in case m_scrollPosX/Y changed

  vertScrollBarWidth = m_scrollWidth;
  horizScrollBarHeight = m_scrollWidth;
  if (m_vScrollBar && !m_vScrollBar->IsShown())
    vertScrollBarWidth = 0;
  if (m_hScrollBar && !m_hScrollBar->IsShown())
    horizScrollBarHeight = 0;

  if (m_hScrollBar && m_hScrollBar->IsShown())
  {
    int nCols = GetCols();
    m_hScrollBar->SetScrollbar(m_hScrollBar->GetThumbPosition(), wxMax(noHorizSteps, 1), (noHorizSteps == 0) ? 1 : nCols, wxMax(noHorizSteps, 1));

/*
    m_hScrollBar->SetSize(m_leftOfSheet, ch - m_scrollWidth -2,   // why -2 ? Robert.
      cw - vertScrollBarWidth - m_leftOfSheet, m_scrollWidth);
*/
    m_hScrollBar->SetSize(m_leftOfSheet, ch - m_scrollWidth,
      cw - vertScrollBarWidth - m_leftOfSheet, m_scrollWidth);

  }

  if (m_vScrollBar && m_vScrollBar->IsShown())
  {
    int nRows = GetRows();

    m_vScrollBar->SetScrollbar(m_vScrollBar->GetThumbPosition(), wxMax(noVertSteps, 1), (noVertSteps == 0) ? 1 : nRows, wxMax(noVertSteps, 1));
    m_vScrollBar->SetSize(cw - m_scrollWidth, m_topOfSheet,
       m_scrollWidth, ch - m_topOfSheet - horizScrollBarHeight);
  }
}

void wxGenericGrid::OnSize(wxSizeEvent& WXUNUSED(event) )
{
  if (!m_vScrollBar || !m_hScrollBar)
          return;

  AdjustScrollbars();

  int cw, ch;
  GetClientSize(&cw, &ch);

  if (m_editCreated && m_editingPanel && GetTextItem() && GetTextItem()->IsShown())
  {
    m_editingPanel->SetSize(0, 0, cw, m_editControlPosition.height + m_editControlPosition.y + 2);
    GetTextItem()->SetSize(m_editControlPosition.x, m_editControlPosition.y,
      cw - 2*m_editControlPosition.x, m_editControlPosition.height);
  }
}

bool wxGenericGrid::CellHitTest(int x, int y, int *row, int *col)
{
      // Find the selected cell and call OnSelectCell
      if (x >= (m_leftOfSheet + m_verticalLabelWidth) && y >= (m_topOfSheet + m_horizontalLabelHeight) &&
          x <= m_rightOfSheet && y <= m_bottomOfSheet)
      {
         // Calculate the cell number from x and y
         x -= (m_verticalLabelWidth + m_leftOfSheet);
         y -= (m_topOfSheet + m_horizontalLabelHeight);

         int i;

         // Now we need to do a hit test for which row we're on
         int currentHeight = 0;
         for (i = m_scrollPosY; i < m_totalRows; i++)
         {
            if (y >= currentHeight && y <= (currentHeight + m_rowHeights[i]))
            {
              *row = i;
              break;
            }
            currentHeight += m_rowHeights[i];
         }

         // Now we need to do a hit test for which column we're on
         int currentWidth = 0;
         for (i = m_scrollPosX; i < m_totalCols; i++)
         {
            if (x >= currentWidth && x <= (currentWidth + m_colWidths[i]))
            {
              *col = i;
              break;
            }
            currentWidth += m_colWidths[i];
         }
         return TRUE;
       }
  return FALSE;
}

bool wxGenericGrid::LabelSashHitTest(int x, int y, int *orientation, int *rowOrCol, int *startPos)
{
  int i;
  int tolerance = 3;

  if (x >= (m_leftOfSheet + m_verticalLabelWidth) && y >= m_topOfSheet &&
      x <= m_rightOfSheet && y <= (m_topOfSheet + m_horizontalLabelHeight))
  {
    // We may be on a column label sash.
    int currentWidth = m_leftOfSheet + m_verticalLabelWidth;
    for (i = m_scrollPosX; i < m_totalCols; i++)
    {
      if (x >= (currentWidth + m_colWidths[i] - tolerance) && x <= (currentWidth + m_colWidths[i] + tolerance))
      {
        *orientation = wxHORIZONTAL;
        *rowOrCol = i;
        *startPos = currentWidth;
        return TRUE;
      }
      currentWidth += m_colWidths[i];
    }
    return FALSE;
  }
  else if (x >= m_leftOfSheet && y >= (m_topOfSheet + m_horizontalLabelHeight) &&
      x <= (m_leftOfSheet + m_verticalLabelWidth) && y <= m_bottomOfSheet)
  {
    // We may be on a row label sash.
    int currentHeight = m_topOfSheet + m_horizontalLabelHeight;
    for (i = m_scrollPosY; i < m_totalRows; i++)
    {
      if (y >= (currentHeight + m_rowHeights[i] - tolerance) && y <= (currentHeight + m_rowHeights[i] + tolerance))
      {
        *orientation = wxVERTICAL;
        *rowOrCol = i;
        *startPos = currentHeight;
        return TRUE;
      }
      currentHeight += m_rowHeights[i];
    }
    return FALSE;
  }
  return FALSE;
}

bool wxGenericGrid::LabelHitTest(int x, int y, int *row, int *col)
{
      // Find the selected label
      if (x >= m_leftOfSheet && y >= m_topOfSheet &&
          x <= m_rightOfSheet && y <= m_bottomOfSheet)
      {
         // Calculate the cell number from x and y
         x -= m_leftOfSheet;
         y -= m_topOfSheet;

         int i;

         // Now we need to do a hit test for which row we're on
         int currentHeight = m_horizontalLabelHeight;
         for (i = m_scrollPosY; i < m_totalRows; i++)
         {
            if (y >= currentHeight && y <= (currentHeight + m_rowHeights[i]))
            {
              *row = i;
              break;
            }
            currentHeight += m_rowHeights[i];
         }
         if (y >= 0 && y <= m_horizontalLabelHeight)
         {
             *row = -1;
         }

         // Now we need to do a hit test for which column we're on
         int currentWidth = m_verticalLabelWidth;
         for (i = m_scrollPosX; i < m_totalCols; i++)
         {
            if (x >= currentWidth && x <= (currentWidth + m_colWidths[i]))
            {
              *col = i;
              break;
            }
            currentWidth += m_colWidths[i];
         }
         if (x >= 0 && x <= m_verticalLabelWidth)
         {
             *col = -1;
         }

         if ((*col == -1) || (*row == -1))
         {
             return TRUE;
         }
       }
  return FALSE;
}

void wxGenericGrid::OnMouseEvent(wxMouseEvent& ev)
{
  if (ev.LeftDown())
  {
    wxClientDC dc(this);
    dc.BeginDrawing();

    int row, col;
    if (CellHitTest((int)ev.GetX(), (int)ev.GetY(), &row, &col))
    {
      OnSelectCellImplementation(& dc, row, col);

      //OnCellLeftClick(row, col, (int)ev.GetX(), (int)ev.GetY(), ev.ControlDown(), ev.ShiftDown());
      wxGridEvent g_evt(GetId(), wxEVT_GRID_CELL_LCLICK, this,
                        row, col, (int)ev.GetX(), (int)ev.GetY(),
                        ev.ControlDown(), ev.ShiftDown());
      GetEventHandler()->ProcessEvent(g_evt);

    }
    if (LabelHitTest((int)ev.GetX(), (int)ev.GetY(), &row, &col))
    {
      //OnLabelLeftClick(row, col, (int)ev.GetX(), (int)ev.GetY(), ev.ControlDown(), ev.ShiftDown());
      wxGridEvent g_evt(GetId(), wxEVT_GRID_LABEL_LCLICK, this,
                        row, col, (int)ev.GetX(), (int)ev.GetY(),
                        ev.ControlDown(), ev.ShiftDown());
      GetEventHandler()->ProcessEvent(g_evt);

    }
    dc.EndDrawing();
  }
  else if (ev.Dragging() && ev.LeftIsDown())
  {
    switch (m_dragStatus)
    {
      case wxGRID_DRAG_NONE:
      {
        int orientation;
        if (LabelSashHitTest((int)ev.GetX(), (int)ev.GetY(), &orientation, &m_dragRowOrCol, &m_dragStartPosition))
        {
          if (orientation == wxHORIZONTAL)
          {
            m_dragStatus = wxGRID_DRAG_LEFT_RIGHT;
            SetCursor(m_horizontalSashCursor);
            m_dragLastPosition = (int)ev.GetX();
          }
          else
          {
            m_dragStatus = wxGRID_DRAG_UP_DOWN;
            SetCursor(m_verticalSashCursor);
            m_dragLastPosition = (int)ev.GetY();
          }
          wxClientDC dc(this);
          dc.BeginDrawing();
          dc.SetLogicalFunction(wxINVERT);
          if (orientation == wxHORIZONTAL)
            dc.DrawLine((int)ev.GetX(), m_topOfSheet, (int)ev.GetX(), m_bottomOfSheet);
          else
            dc.DrawLine(m_leftOfSheet, (int)ev.GetY(), m_rightOfSheet, (int)ev.GetY());
          dc.EndDrawing();

          CaptureMouse();
        }
        break;
      }
      case wxGRID_DRAG_LEFT_RIGHT:
      {
        wxClientDC dc(this);
        dc.BeginDrawing();
        dc.SetLogicalFunction(wxINVERT);
        dc.DrawLine(m_dragLastPosition, m_topOfSheet, m_dragLastPosition, m_bottomOfSheet);

        dc.DrawLine((int)ev.GetX(), m_topOfSheet, (int)ev.GetX(), m_bottomOfSheet);
        dc.EndDrawing();

        m_dragLastPosition = (int)ev.GetX();
        SetCursor(m_horizontalSashCursor);
        break;
      }
      case wxGRID_DRAG_UP_DOWN:
      {
        wxClientDC dc(this);
        dc.BeginDrawing();
        dc.SetLogicalFunction(wxINVERT);
        dc.DrawLine(m_leftOfSheet, m_dragLastPosition, m_rightOfSheet, m_dragLastPosition);

        dc.DrawLine(m_leftOfSheet, (int)ev.GetY(), m_rightOfSheet, (int)ev.GetY());
        dc.EndDrawing();

        m_dragLastPosition = (int)ev.GetY();
        SetCursor(m_verticalSashCursor);
        break;
      }
    }
  }
  else if (ev.Moving())
  {
    int rowOrCol, orientation, startPos;
    if (LabelSashHitTest((int)ev.GetX(), (int)ev.GetY(), &orientation, &rowOrCol, &startPos))
    {
      if (orientation == wxHORIZONTAL)
        SetCursor(m_horizontalSashCursor);
       else
        SetCursor(m_verticalSashCursor);
    }
    else
      SetCursor(*wxSTANDARD_CURSOR);
  }
  else if (ev.LeftUp())
  {
    switch (m_dragStatus)
    {
      case wxGRID_DRAG_LEFT_RIGHT:
      {
        wxClientDC dc(this);
        dc.BeginDrawing();
        dc.SetLogicalFunction(wxINVERT);
        dc.DrawLine(m_dragLastPosition, m_topOfSheet, m_dragLastPosition, m_bottomOfSheet);
        dc.SetLogicalFunction(wxCOPY);
        dc.EndDrawing();

        ReleaseMouse();
        if (ev.GetX() > m_dragStartPosition)
        {
          m_colWidths[m_dragRowOrCol] = (short)(ev.GetX() - m_dragStartPosition);
          UpdateDimensions();
          AdjustScrollbars();
          Refresh();
        }
        SetCursor(*wxSTANDARD_CURSOR);
        int cw, ch;
        GetClientSize(&cw, &ch);
                wxSizeEvent evt;
        OnSize(evt);
        break;
      }
      case wxGRID_DRAG_UP_DOWN:
      {
        wxClientDC dc(this);
        dc.BeginDrawing();
        dc.SetLogicalFunction(wxINVERT);
        dc.DrawLine(m_leftOfSheet, m_dragLastPosition, m_rightOfSheet, m_dragLastPosition);
        dc.SetLogicalFunction(wxCOPY);
        dc.EndDrawing();

        ReleaseMouse();
        if (ev.GetY() > m_dragStartPosition)
        {
          m_rowHeights[m_dragRowOrCol] = (short)(ev.GetY() - m_dragStartPosition);
          UpdateDimensions();
          AdjustScrollbars();
          Refresh();
        }
        SetCursor(*wxSTANDARD_CURSOR);
        break;
      }
    }
    m_dragStatus = wxGRID_DRAG_NONE;
  }
  else if (ev.RightDown())
  {
    int row, col;
    if (CellHitTest((int)ev.GetX(), (int)ev.GetY(), &row, &col))
    {
      //OnCellRightClick(row, col, (int)ev.GetX(), (int)ev.GetY(), ev.ControlDown(), ev.ShiftDown());
      wxGridEvent g_evt(GetId(), wxEVT_GRID_CELL_RCLICK, this,
                        row, col, (int)ev.GetX(), (int)ev.GetY(),
                        ev.ControlDown(), ev.ShiftDown());
      GetEventHandler()->ProcessEvent(g_evt);

    }
    if (LabelHitTest((int)ev.GetX(), (int)ev.GetY(), &row, &col))
    {
      //OnLabelRightClick(row, col, (int)ev.GetX(), (int)ev.GetY(), ev.ControlDown(), ev.ShiftDown());
      wxGridEvent g_evt(GetId(), wxEVT_GRID_LABEL_RCLICK, this,
                        row, col, (int)ev.GetX(), (int)ev.GetY(),
                        ev.ControlDown(), ev.ShiftDown());
      GetEventHandler()->ProcessEvent(g_evt);
    }
  }
}

void wxGenericGrid::OnSelectCellImplementation(wxDC *dc, int row, int col)
{
  m_wCursorColumn = col;
  m_wCursorRow = row;

  //OnChangeSelectionLabel();
  wxGridEvent g_evt(GetId(), wxEVT_GRID_CHANGE_SEL_LABEL, this);
  GetEventHandler()->ProcessEvent(g_evt);

  SetGridClippingRegion(dc);

  // Remove the highlight from the old cell
  if ( m_currentRectVisible && !(m_editable && m_editInPlace) )
    {
      HighlightCell(dc);
    }


  // Highlight the new cell and copy its content to the edit control
  SetCurrentRect(m_wCursorRow, m_wCursorColumn);
  wxGridCell *cell = GetCell(m_wCursorRow, m_wCursorColumn);
  if (cell)
  {
        if ( cell->GetTextValue().IsNull() )
                m_textItem->SetValue("");
        else
            m_textItem->SetValue(cell->GetTextValue());
  }

  SetGridClippingRegion(dc);


  if ( m_editable && m_editInPlace )
    {
      m_inPlaceTextItem->SetSize( m_currentRect.x-2, m_currentRect.y-2,
                                  m_currentRect.width+4, m_currentRect.height+4 );

      if ( cell )
        {
          if ( cell->GetTextValue().IsNull() )
            {
              m_inPlaceTextItem->SetValue( "" );
            }
          else
            {
              m_inPlaceTextItem->SetFont( cell->GetFont() );
              m_inPlaceTextItem->SetValue( cell->GetTextValue() );
            }
        }

      m_inPlaceTextItem->Show(TRUE);
      m_inPlaceTextItem->SetFocus();
    }
  else
    {
      // 1) Why isn't this needed for Windows??
      // Probably because of the SetValue?? JS.
      // 2) Arrrrrgh. This isn't needed anywhere,
      // of course. One hour of debugging... RR.
      //
      // 3) It *is* needed for Motif - michael
      //
#ifdef __WXMOTIF__
      if ( !(m_editable && m_editInPlace) )
          HighlightCell(dc);
#endif
    }

  dc->DestroyClippingRegion();

  OnSelectCell(row, col);
  wxGridEvent g_evt2(GetId(), wxEVT_GRID_SELECT_CELL, this, row, col);
  GetEventHandler()->ProcessEvent(g_evt2);
}

wxGridCell *wxGenericGrid::OnCreateCell()
{
  return new wxGridCell(this);
}

void wxGenericGrid::OnChangeLabels()
{
  char buf[100];
  int i;
  for (i = 0; i < m_totalRows; i++)
  {
    sprintf(buf, "%d", i+1);
    SetLabelValue(wxVERTICAL, buf, i);
  }
  // A...Z,AA...ZZ,AAA...ZZZ, etc.
  for (i = 0; i < m_totalCols; i++)
  {
    int j;
    int noTimes = (i/26 + 1);
    int ch = (i % 26) + 65;
    buf[0] = 0;
    for (j = 0; j < noTimes; j++)
    {
      char buf2[20];
      sprintf(buf2, "%c", (char)ch);
      strcat(buf, buf2);
    }
    SetLabelValue(wxHORIZONTAL, buf, i);
  }
}

void wxGenericGrid::OnChangeSelectionLabel()
{
  if (!GetEditable())
    return;

  wxString rowLabel(GetLabelValue(wxVERTICAL, GetCursorRow()));
  wxString colLabel(GetLabelValue(wxHORIZONTAL, GetCursorColumn()));

  wxString newLabel = colLabel + rowLabel;
  if ((newLabel.Length() > 0) && (newLabel.Length() <= 8) && GetTextItem())
  {
//    GetTextItem()->SetLabel(newLabel);
  }
}

void wxGenericGrid::HighlightCell(wxDC *dc)
{
  dc->SetLogicalFunction(wxINVERT);

  // Top
  dc->DrawLine( m_currentRect.x + 1,
                m_currentRect.y + 1,
                m_currentRect.x + m_currentRect.width - 1,
                m_currentRect.y + 1);
  // Right
  dc->DrawLine( m_currentRect.x + m_currentRect.width - 1,
                m_currentRect.y + 1,
                m_currentRect.x + m_currentRect.width - 1,
                m_currentRect.y +m_currentRect.height - 1 );
  // Bottom
  dc->DrawLine( m_currentRect.x + m_currentRect.width - 1,
                m_currentRect.y + m_currentRect.height - 1,
                m_currentRect.x + 1,
                m_currentRect.y + m_currentRect.height - 1);
  // Left
  dc->DrawLine( m_currentRect.x + 1,
                m_currentRect.y + m_currentRect.height - 1,
                m_currentRect.x + 1,
                m_currentRect.y + 1);

  dc->SetLogicalFunction(wxCOPY);
}

void wxGenericGrid::DrawCellText()
{
  if (!m_currentRectVisible)
    return;

  wxGridCell *cell = GetCell(GetCursorRow(), GetCursorColumn());
  if (!cell)
    return;

  wxClientDC dc(this);
  dc.BeginDrawing();

  SetGridClippingRegion(& dc);

  dc.SetBackgroundMode(wxTRANSPARENT);
  dc.SetBrush(cell->GetBackgroundBrush());

  wxString editValue = m_textItem->GetValue();

  wxRect rect;
  rect = m_currentRect;
  rect.x += 3;
  rect.y += 2;
  rect.width -= 5;
  rect.height -= 4;

  // FIXME: what's this string of spaces supposed to represent?
  DrawTextRect(& dc, "                                    ", &rect, wxLEFT);
  DrawTextRect(& dc, editValue, &rect, cell->GetAlignment());

  dc.DestroyClippingRegion();

  dc.SetBackgroundMode(wxSOLID);

  dc.EndDrawing();
}

void wxGenericGrid::SetCurrentRect(int Row, int Column, int canvasW, int canvasH)
{
  int currentWidth = m_leftOfSheet + m_verticalLabelWidth;
  int i;
  for (i = m_scrollPosX; i < Column; i++)
    currentWidth += m_colWidths[i];

  int currentHeight = m_topOfSheet + m_horizontalLabelHeight;
  for (i = m_scrollPosY; i < Row; i++)
    currentHeight += m_rowHeights[i];

  m_currentRect.x = currentWidth;
  m_currentRect.y = currentHeight;
  m_currentRect.width = m_colWidths ? (m_colWidths[Column]) : 0;
  m_currentRect.height = m_rowHeights ? (m_rowHeights[Row]) : 0;

  if (Row < m_scrollPosY || Column < m_scrollPosX)
    m_currentRectVisible = FALSE;
  else if ((canvasW != -1 && canvasH != -1) && (m_currentRect.x > canvasW || m_currentRect.y > canvasH))
    m_currentRectVisible = FALSE;
  else m_currentRectVisible = TRUE;
}

static bool wxRectIntersection(wxRect *rect1, wxRect *rect2, wxRect *rect3)
{
  int x2_1 = rect1->x + rect1->width;
  int y2_1 = rect1->y + rect1->height;

  int x2_2 = rect2->x + rect2->width;
  int y2_2 = rect2->y + rect2->height;

  int x2_3, y2_3;

  // Check for intersection
  if ((rect1->x > x2_2) || (rect2->x > x2_1) ||
      (rect1->y > y2_2) || (rect2->y > y2_1))
  {
    // No intersection
    rect3->x = rect3->y = rect3->width = rect3->height = 0;
    return FALSE;
  }

  if (rect1->x > rect2->x)
    rect3->x = rect1->x;
  else
    rect3->x = rect2->x;
  if (rect1->y > rect2->y)
    rect3->y = rect1->y;
  else
    rect3->y = rect2->y;

  if (x2_1 > x2_2)
    x2_3 = x2_2;
  else
    x2_3 = x2_1;
  if (y2_1 > y2_2)
    y2_3 = y2_2;
  else
    y2_3 = y2_1;

  rect3->width = (int)(x2_3 - rect3->x);
  rect3->height = (int)(y2_3 - rect3->y);
  return TRUE;
}

void wxGenericGrid::DrawTextRect(wxDC *dc, const wxString& text, wxRect *rect, int flag)
{
  dc->BeginDrawing();

  // Ultimately, this functionality should be built into wxWindows,
  // and optimized for each platform. E.g. on Windows, use DrawText
  // passing a clipping rectangle, so that the wxWindows clipping region
  // does not have to be used to implement this.

  // If we're already clipping, we need to find the intersection
  // between current clipping area and text clipping area.

  wxRect clipRect;
  wxRect clipRect2;
  long clipX, clipY, clipW, clipH;
  dc->GetClippingBox(&clipX, &clipY, &clipW, &clipH);
  clipRect.x = (int)clipX; clipRect.y = (int)clipY;
  clipRect.width = (int)clipW; clipRect.height = (int)clipH;

  bool alreadyClipping = TRUE;

  if (clipRect.x == 0 && clipRect.y == 0 && clipRect.width == 0 && clipRect.height == 0)
  {
    alreadyClipping = FALSE;
    clipRect2.x = rect->x; clipRect2.y = rect->y;
    clipRect2.width = rect->width; clipRect2.height = rect->height;
  }
  else
  {
    // Find intersection.
    if (!wxRectIntersection(rect, &clipRect, &clipRect2))
      return;
  }

  if (alreadyClipping)
    dc->DestroyClippingRegion();

  dc->SetClippingRegion(clipRect2.x, clipRect2.y, clipRect2.width, clipRect2.height);
  long textWidth, textHeight;

  dc->GetTextExtent(text, &textWidth, &textHeight);

  // Do alignment
  float x,y;
  switch (flag)
  {
    case wxRIGHT:
    {
      x = (rect->x + rect->width - textWidth - 1.0);
      y = (rect->y + (rect->height - textHeight)/2.0);
      break;
    }
    case wxCENTRE:
    {
      x = (rect->x + (rect->width - textWidth)/2.0);
      y = (rect->y + (rect->height - textHeight)/2.0);
      break;
    }
    case wxLEFT:
    default:
    {
      x = (rect->x + 1.0);
      y = (rect->y + (rect->height - textHeight)/2.0);
      break;
    }
  }
  dc->DrawText(text, (long)x, (long)y );

  dc->DestroyClippingRegion();

  // Restore old clipping
  if (alreadyClipping)
    dc->SetClippingRegion(clipRect.x, clipRect.y, clipRect.width, clipRect.height);

  dc->EndDrawing();
}

void wxGenericGrid::DrawBitmapRect(wxDC *dc, wxBitmap *bitmap, wxRect *rect, int flag)
{
  dc->BeginDrawing();

  // Ultimately, this functionality should be built into wxWindows,
  // and optimized for each platform. E.g. on Windows, use DrawText
  // passing a clipping rectangle, so that the wxWindows clipping region
  // does not have to be used to implement this.

  // If we're already clipping, we need to find the intersection
  // between current clipping area and text clipping area.

  wxRect clipRect;
  wxRect clipRect2;
  long clipX, clipY, clipW, clipH;
  dc->GetClippingBox(&clipX, &clipY, &clipW, &clipH);
  clipRect.x = (int)clipX; clipRect.y = (int)clipY;
  clipRect.width = (int)clipW; clipRect.height = (int)clipH;

  bool alreadyClipping = TRUE;

  if (clipRect.x == 0 && clipRect.y == 0 && clipRect.width == 0 && clipRect.height == 0)
  {
    alreadyClipping = FALSE;
    clipRect2.x = rect->x; clipRect2.y = rect->y;
    clipRect2.width = rect->width; clipRect2.height = rect->height;
  }
  else
  {
    // Find intersection.
    if (!wxRectIntersection(rect, &clipRect, &clipRect2))
      return;
  }

  if (alreadyClipping)
    dc->DestroyClippingRegion();

  dc->SetClippingRegion(clipRect2.x, clipRect2.y, clipRect2.width, clipRect2.height);
  float bitmapWidth, bitmapHeight;

  bitmapWidth = bitmap->GetWidth();
  bitmapHeight = bitmap->GetHeight();

  // Do alignment
  long x,y;
  switch (flag)
  {
    case wxRIGHT:
    {
      x = (long)(rect->x + rect->width - bitmapWidth - 1);
      y = (long)(rect->y + (rect->height - bitmapHeight)/2.0);
      break;
    }
    case wxCENTRE:
    {
      x = (long)(rect->x + (rect->width - bitmapWidth)/2.0);
      y = (long)(rect->y + (rect->height - bitmapHeight)/2.0);
      break;
    }
    case wxLEFT:
    default:
    {
      x = (long)(rect->x + 1);
      y = (long)(rect->y + (rect->height - bitmapHeight)/2.0);
      break;
    }
  }
  wxMemoryDC dcTemp;
  dcTemp.SelectObject(*bitmap);

  dc->Blit( (long)x, (long)y, (long)bitmapWidth, (long)bitmapHeight, &dcTemp, 0, 0);
  dcTemp.SelectObject(wxNullBitmap);

  dc->DestroyClippingRegion();

  // Restore old clipping
  if (alreadyClipping)
    dc->SetClippingRegion(clipRect.x, clipRect.y, clipRect.width, clipRect.height);

  dc->EndDrawing();
}

void wxGenericGrid::OnActivate(bool active)
{
  if (active)
  {
    // Edit control should always have the focus
    if (GetTextItem() && GetEditable())
    {
      GetTextItem()->SetFocus();
      wxGridCell *cell = GetCell(GetCursorRow(), GetCursorColumn());
      if (cell)
        GetTextItem()->SetValue(cell->GetTextValue());
    }
  }
}

void wxGenericGrid::SetCellValue(const wxString& val, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    cell->SetTextValue(val);

    RefreshCell(row, col, TRUE);
  }
}

void wxGenericGrid::RefreshCell(int row, int col, bool setText)
{
    // Don't refresh within a pair of batch brackets
    if (GetBatchCount() > 0)
        return;

    int cw, ch;
    GetClientSize(&cw, &ch);

    SetCurrentRect(row, col, cw, ch);
    if (m_currentRectVisible)
    {
      wxGridCell *cell = GetCell(row, col);

      bool currentPos = FALSE;
      if (row == m_wCursorRow && col == m_wCursorColumn && GetTextItem() && GetTextItem()->IsShown() && setText)
      {
        GetTextItem()->SetValue(cell->GetTextValue());
        currentPos = TRUE;
      }
      // Gets refreshed anyway in MSW
#ifdef __WXMSW__
      if (!currentPos)
#endif
      {
        wxClientDC dc(this);
        dc.BeginDrawing();
        DrawCellBackground(& dc, &m_currentRect, row, col);
        DrawCellValue(& dc, &m_currentRect, row, col);
        dc.EndDrawing();
      }
    }
}

wxString& wxGenericGrid::GetCellValue(int row, int col) const
{
  static wxString emptyString("");

  wxGridCell *cell = GetCell(row, col);
  if (cell)
    return cell->GetTextValue();
  else
    return emptyString;
}

void wxGenericGrid::SetColumnWidth(int col, int width)
{
  if (col <= m_totalCols)
    m_colWidths[col] = width;
}

int wxGenericGrid::GetColumnWidth(int col) const
{
  if (col <= m_totalCols)
    return m_colWidths[col];
  else
    return 0;
}

void wxGenericGrid::SetRowHeight(int row, int height)
{
  if (row <= m_totalRows)
    m_rowHeights[row] = height;
}

int wxGenericGrid::GetRowHeight(int row) const
{
  if (row <= m_totalRows)
    return m_rowHeights[row];
  else
    return 0;
}

void wxGenericGrid::SetLabelSize(int orientation, int sz)
{
  if (orientation == wxHORIZONTAL)
    m_horizontalLabelHeight = sz;
  else
    m_verticalLabelWidth = sz;
  UpdateDimensions();
  SetCurrentRect(GetCursorRow(), GetCursorColumn());
}

int wxGenericGrid::GetLabelSize(int orientation) const
{
  if (orientation == wxHORIZONTAL)
    return m_horizontalLabelHeight;
  else
    return m_verticalLabelWidth;
}

wxGridCell *wxGenericGrid::GetLabelCell(int orientation, int pos) const
{
  if (orientation == wxHORIZONTAL)
  {
    if (m_colLabelCells && pos < m_totalCols)
      return m_colLabelCells[pos];
    else
      return (wxGridCell *) NULL;
  }
  else
  {
    if (m_rowLabelCells && pos < m_totalRows)
      return m_rowLabelCells[pos];
    else
      return (wxGridCell *) NULL;
  }
}

void wxGenericGrid::SetLabelValue(int orientation, const wxString& val, int pos)
{
  wxGridCell *cell = GetLabelCell(orientation, pos);
  if (cell)
    cell->SetTextValue(val);
}

wxString& wxGenericGrid::GetLabelValue(int orientation, int pos) const
{
 static wxString emptyString = "";
  wxGridCell *cell = GetLabelCell(orientation, pos);
  if (cell)
    return cell->GetTextValue();
  else
    return emptyString;
}

void wxGenericGrid::SetLabelAlignment(int orientation, int align)
{
  if (orientation == wxHORIZONTAL)
    m_horizontalLabelAlignment = align;
  else
    m_verticalLabelAlignment = align;
  UpdateDimensions();
  SetCurrentRect(GetCursorRow(), GetCursorColumn());
}

int wxGenericGrid::GetLabelAlignment(int orientation) const
{
  if (orientation == wxHORIZONTAL)
    return m_horizontalLabelAlignment;
  else
    return m_verticalLabelAlignment;
}

void wxGenericGrid::SetLabelTextColour(const wxColour& colour)
{
  m_labelTextColour = colour;

}

void wxGenericGrid::SetLabelBackgroundColour(const wxColour& colour)
{
  m_labelBackgroundColour = colour;
  m_labelBackgroundBrush = * wxTheBrushList->FindOrCreateBrush(m_labelBackgroundColour, wxSOLID);
}

void wxGenericGrid::SetEditable(bool edit)
{
  m_editable = edit;
  if (edit)
  {
    int controlW, controlH;
    m_textItem->GetSize(&controlW, &controlH);
    m_editControlPosition.height = controlH;

    m_topOfSheet = m_editControlPosition.x + controlH + 2;
    if (m_textItem)
    {
      m_editingPanel->Show(TRUE);
      m_textItem->Show(TRUE);
      m_textItem->SetFocus();
    }

    if (m_inPlaceTextItem)
      {
        m_inPlaceTextItem->Show(TRUE);
        m_inPlaceTextItem->SetFocus();
      }
  }
  else
  {
    m_topOfSheet = 0;
    if (m_textItem)
    {
      m_textItem->Show(FALSE);
      m_editingPanel->Show(FALSE);
    }

    if ( m_inPlaceTextItem )
      {
        m_inPlaceTextItem->Show(FALSE);
      }
  }
  UpdateDimensions();
  SetCurrentRect(GetCursorRow(), GetCursorColumn());

  int cw, ch;
  GetClientSize(&cw, &ch);
  wxSizeEvent evt;
  OnSize(evt);
/*
  int cw, ch;
  int m_scrollWidth = 16;
  GetClientSize(&cw, &ch);

  if (m_vScrollBar)
    m_vScrollBar->SetSize(cw - m_scrollWidth, m_topOfSheet,
       m_scrollWidth, ch - m_topOfSheet - m_scrollWidth);
*/
}


void wxGenericGrid::SetEditInPlace(bool edit)
{
  if ( m_editInPlace != edit )
    {
      m_editInPlace = edit;

      if ( m_editInPlace )  // switched on
        {
          if ( m_currentRectVisible && m_editable )
            {
              m_inPlaceTextItem->SetSize( m_currentRect.x-2, m_currentRect.y-2,
                                          m_currentRect.width+4, m_currentRect.height+4 );

              wxGridCell *cell = GetCell(m_wCursorRow, m_wCursorColumn);

              if ( cell )
                {
                  if ( cell->GetTextValue().IsNull() )
                    {
                      m_inPlaceTextItem->SetValue( "" );
                    }
                  else
                    {
                      m_inPlaceTextItem->SetFont( cell->GetFont() );
                      m_inPlaceTextItem->SetValue( cell->GetTextValue() );
                    }
                }

              m_inPlaceTextItem->Show( TRUE );
              m_inPlaceTextItem->SetFocus();
            }
        }
      else  // switched off
        {
          m_inPlaceTextItem->Show( FALSE );
        }
    }
}


void wxGenericGrid::SetCellAlignment(int flag, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
    cell->SetAlignment(flag);
}

int wxGenericGrid::GetCellAlignment(int row, int col) const
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
    return cell->GetAlignment();
  else
    return m_cellAlignment;
}

void wxGenericGrid::SetCellAlignment(int flag)
{
  m_cellAlignment = flag;
  int i,j;
  for (i = 0; i < GetRows(); i++)
    for (j = 0; j < GetCols(); j++)
      if (GetCell(i, j))
        GetCell(i, j)->SetAlignment(flag);
}

int wxGenericGrid::GetCellAlignment(void) const
{
  return m_cellAlignment;
}

void wxGenericGrid::SetCellBackgroundColour(const wxColour& col)
{
  m_cellBackgroundColour = col;
  int i,j;
  for (i = 0; i < GetRows(); i++)
    for (j = 0; j < GetCols(); j++)
      if (GetCell(i, j))
        GetCell(i, j)->SetBackgroundColour(col);
}

void wxGenericGrid::SetCellBackgroundColour(const wxColour& val, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    cell->SetBackgroundColour(val);
    RefreshCell(row, col);
  }
}

wxColour& wxGenericGrid::GetCellBackgroundColour(int row, int col) const
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
    return cell->GetBackgroundColour();
  else
    return (wxColour&) m_cellBackgroundColour;
}

void wxGenericGrid::SetCellTextColour(const wxColour& val, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    cell->SetTextColour(val);
    RefreshCell(row, col);
  }
}

void wxGenericGrid::SetCellTextFont(const wxFont& fnt, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    cell->SetFont(fnt);
    RefreshCell(row, col);
  }
}

wxFont& wxGenericGrid::GetCellTextFont(int row, int col) const
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
    return (wxFont&) cell->GetFont();
  else
    return (wxFont&) m_cellTextFont;
}

wxColour& wxGenericGrid::GetCellTextColour(int row, int col) const
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
    return (wxColour&) cell->GetTextColour();
  else
    return (wxColour&) m_cellTextColour;
}

void wxGenericGrid::SetCellTextColour(const wxColour& val)
{
  m_cellTextColour = val;
  int i,j;
  for (i = 0; i < GetRows(); i++)
    for (j = 0; j < GetCols(); j++)
      if (GetCell(i, j))
        GetCell(i, j)->SetTextColour(val);
}

void wxGenericGrid::SetCellTextFont(const wxFont& fnt)
{
  m_cellTextFont = fnt;
  int i,j;
  for (i = 0; i < GetRows(); i++)
    for (j = 0; j < GetCols(); j++)
      if (GetCell(i, j))
        GetCell(i, j)->SetFont(fnt);
}

void wxGenericGrid::SetCellBitmap(wxBitmap *bitmap, int row, int col)
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    cell->SetCellBitmap(bitmap);
    RefreshCell(row, col);
  }
}

wxBitmap *wxGenericGrid::GetCellBitmap(int row, int col) const
{
  wxGridCell *cell = GetCell(row, col);
  if (cell)
  {
    return cell->GetCellBitmap();
  }
  else
    return (wxBitmap *) NULL;
}

bool wxGenericGrid::InsertCols(int pos, int n, bool updateLabels)
{
  if (pos > m_totalCols)
    return FALSE;

  if (!m_gridCells)
    return CreateGrid(1, n);
  else
  {
    int i, j;
    // Cells
    for (i = 0; i < m_totalRows; i++)
    {
      wxGridCell **cols = m_gridCells[i];
      wxGridCell **newCols = new wxGridCell *[m_totalCols + n];
      for (j = 0; j < pos; j++)
        newCols[j] = cols[j];
      for (j = pos; j < pos + n; j++)
        newCols[j] = new wxGridCell(this);
      for (j = pos + n; j < m_totalCols + n; j++)
        newCols[j] = cols[j - n];

      delete[] cols;
      m_gridCells[i] = newCols;
    }

    // Column widths
    short *newColWidths = new short[m_totalCols + n];
    for (j = 0; j < pos; j++)
      newColWidths[j] = m_colWidths[j];
    for (j = pos; j < pos + n; j++)
      newColWidths[j] = wxGRID_DEFAULT_CELL_WIDTH;
    for (j = pos + n; j < m_totalCols + n; j++)
      newColWidths[j] = m_colWidths[j - n];
    delete[] m_colWidths;
    m_colWidths = newColWidths;

    // Column labels
    wxGridCell **newLabels = new wxGridCell *[m_totalCols + n];
    for (j = 0; j < pos; j++)
      newLabels[j] = m_colLabelCells[j];
    for (j = pos; j < pos + n; j++)
      newLabels[j] = new wxGridCell(this);
    for (j = pos + n; j < m_totalCols + n; j++)
      newLabels[j] = m_colLabelCells[j - n];

    delete[] m_colLabelCells;
    m_colLabelCells = newLabels;

    m_totalCols += n;

    if (updateLabels) {
        //OnChangeLabels();
        wxGridEvent g_evt(GetId(), wxEVT_GRID_CHANGE_LABELS, this);
        GetEventHandler()->ProcessEvent(g_evt);
    }

    UpdateDimensions();
    AdjustScrollbars();
    return TRUE;
  }
}

bool wxGenericGrid::InsertRows(int pos, int n, bool updateLabels)
{
  if (pos > m_totalRows)
    return FALSE;

  if (!m_gridCells)
    return CreateGrid(n, 1);
  else
  {
    int i, j;

    wxGridCell ***rows = new wxGridCell **[m_totalRows + n];

    // Cells
    for (i = 0; i < pos; i++)
      rows[i] = m_gridCells[i];

    for (i = pos; i < pos + n; i++)
    {
      rows[i] = new wxGridCell *[m_totalCols];
      for (j = 0; j < m_totalCols; j++)
        rows[i][j] = new wxGridCell(this);
    }

    for (i = pos + n; i < m_totalRows + n; i++)
      rows[i] = m_gridCells[i - n];

    delete[] m_gridCells;
    m_gridCells = rows;

    // Row heights
    short *newRowHeights = new short[m_totalRows + n];
    for (i = 0; i < pos; i++)
      newRowHeights[i] = m_rowHeights[i];
    for (i = pos; i < pos + n; i++)
      newRowHeights[i] = wxGRID_DEFAULT_CELL_HEIGHT;
    for (i = pos + n; i < m_totalRows + n; i++)
      newRowHeights[i] = m_rowHeights[i - n];
    delete[] m_rowHeights;
    m_rowHeights = newRowHeights;

    // Column labels
    wxGridCell **newLabels = new wxGridCell *[m_totalRows + n];
    for (i = 0; i < pos; i++)
      newLabels[i] = m_rowLabelCells[i];
    for (i = pos; i < pos + n; i++)
      newLabels[i] = new wxGridCell(this);
    for (i = pos + n; i < m_totalRows + n; i++)
      newLabels[i] = m_rowLabelCells[i - n];

    delete[] m_rowLabelCells;
    m_rowLabelCells = newLabels;

    m_totalRows += n;

    if (updateLabels) {
        //OnChangeLabels();
          wxGridEvent g_evt(GetId(), wxEVT_GRID_CHANGE_LABELS, this);
          GetEventHandler()->ProcessEvent(g_evt);
    }

    UpdateDimensions();
    AdjustScrollbars();
    return TRUE;
  }
}

bool wxGenericGrid::AppendCols(int n, bool updateLabels)
{
  return InsertCols(GetCols(), n, updateLabels);
}

bool wxGenericGrid::AppendRows(int n, bool updateLabels)
{
  return InsertRows(GetRows(), n, updateLabels);
}

bool wxGenericGrid::DeleteRows(int pos, int n, bool updateLabels)
{
  if (pos > m_totalRows)
    return FALSE;
  if (!m_gridCells)
    return FALSE;

  int i;

  wxGridCell ***rows = new wxGridCell **[m_totalRows - n];

  // Cells
  for (i = 0; i < pos; i++)
    rows[i] = m_gridCells[i];

  for (i = pos + n; i < m_totalRows; i++)
    rows[i-n] = m_gridCells[i];

  delete[] m_gridCells;
  m_gridCells = rows;

  // Row heights
  short *newRowHeights = new short[m_totalRows - n];
  for (i = 0; i < pos; i++)
    newRowHeights[i] = m_rowHeights[i];
  for (i = pos + n; i < m_totalRows; i++)
    newRowHeights[i-n] = m_rowHeights[i];
  delete[] m_rowHeights;
  m_rowHeights = newRowHeights;

  // Column labels
  wxGridCell **newLabels = new wxGridCell *[m_totalRows - n];
  for (i = 0; i < pos; i++)
    newLabels[i] = m_rowLabelCells[i];
  for (i = pos + n; i < m_totalRows; i++)
    newLabels[i-n] = m_rowLabelCells[i];

  delete[] m_rowLabelCells;
  m_rowLabelCells = newLabels;

  m_totalRows -= n;

  if (updateLabels){
        //OnChangeLabels();
        wxGridEvent g_evt(GetId(), wxEVT_GRID_CHANGE_LABELS, this);
        GetEventHandler()->ProcessEvent(g_evt);
    }
  UpdateDimensions();
  AdjustScrollbars();
  return TRUE;
}

bool wxGenericGrid::DeleteCols(int pos, int n, bool updateLabels)
{
  if (pos + n > m_totalCols)
    return FALSE;
  if (!m_gridCells)
    return FALSE;

  int i, j;

  // Cells
  for (i = 0; i < m_totalRows; i++)
  {
      wxGridCell **cols = m_gridCells[i];
      wxGridCell **newCols = new wxGridCell *[m_totalCols - n];
      for (j = 0; j < pos; j++)
        newCols[j] = cols[j];
      for (j = pos; j < pos + n; j++)
        delete cols[j];
      for (j = pos + n; j < m_totalCols; j++)
        newCols[j-n] = cols[j];

      delete[] cols;
      m_gridCells[i] = newCols;
  }

  // Column widths
  short *newColWidths = new short[m_totalCols - n];
  for (j = 0; j < pos; j++)
    newColWidths[j] = m_colWidths[j];
  for (j = pos + n; j < m_totalCols; j++)
    newColWidths[j-n] = m_colWidths[j];
  delete[] m_colWidths;
  m_colWidths = newColWidths;

  // Column labels
  wxGridCell **newLabels = new wxGridCell *[m_totalCols - n];
  for (j = 0; j < pos; j++)
    newLabels[j] = m_colLabelCells[j];
  for (j = pos + n; j < m_totalCols; j++)
    newLabels[j-n] = m_colLabelCells[j];

  delete[] m_colLabelCells;
  m_colLabelCells = newLabels;

  m_totalCols -= n;

  if (updateLabels) {
        //OnChangeLabels();
        wxGridEvent g_evt(GetId(), wxEVT_GRID_CHANGE_LABELS, this);
        GetEventHandler()->ProcessEvent(g_evt);
    }
  UpdateDimensions();
  AdjustScrollbars();
  return TRUE;
}

void wxGenericGrid::SetGridCursor(int row, int col)
{
  if (row >= m_totalRows || col >= m_totalCols)
    return;

  if (row == GetCursorRow() && col == GetCursorColumn())
    return;

  wxClientDC dc(this);
  dc.BeginDrawing();

  SetGridClippingRegion(& dc);

  if (m_currentRectVisible && !(m_editable && m_editInPlace) )
    HighlightCell(& dc);

  m_wCursorRow = row;
  m_wCursorColumn = col;

  int cw, ch;
  GetClientSize(&cw, &ch);

  SetCurrentRect(row, col, cw, ch);

  if (m_currentRectVisible && !(m_editable && m_editInPlace) )
    HighlightCell(& dc);

  dc.DestroyClippingRegion();
  dc.EndDrawing();
}

// ----------------------------------------------------------------------------
// Grid cell
// ----------------------------------------------------------------------------

wxGridCell::wxGridCell(wxGenericGrid *window)
{
  cellBitmap = (wxBitmap *) NULL;
  font = wxNullFont;
  backgroundBrush = wxNullBrush;
  if (window)
    textColour = window->GetCellTextColour();
  else
    textColour.Set(0,0,0);
  if (window)
    backgroundColour = window->GetCellBackgroundColour();
  else
    backgroundColour.Set(255,255,255);

  if (window)
    font = window->GetCellTextFont();
  else
    font = * wxTheFontList->FindOrCreateFont(12, wxSWISS, wxNORMAL, wxNORMAL);

  SetBackgroundColour(backgroundColour);

  if (window)
    alignment = window->GetCellAlignment();
  else
    alignment = wxLEFT;

  cellData = (void *)NULL;
}

wxGridCell::~wxGridCell()
{
}

void wxGridCell::SetBackgroundColour(const wxColour& colour)
{
  backgroundColour = colour;
  backgroundBrush = * wxTheBrushList->FindOrCreateBrush(backgroundColour, wxSOLID);
}

void wxGenericGrid::OnText(wxCommandEvent& WXUNUSED(ev) )
{
  // michael - added this conditional to prevent change to
  // grid cell text when edit control is hidden but still has
  // focus
  //
  if ( m_editable )
    {
      wxGenericGrid *grid = this;
      wxGridCell *cell = grid->GetCell(grid->GetCursorRow(), grid->GetCursorColumn());
      if (cell && grid->CurrentCellVisible())
        {
          cell->SetTextValue(grid->GetTextItem()->GetValue());
          if ( m_editInPlace && !m_inOnTextInPlace )
            {
              m_inPlaceTextItem->SetValue( grid->GetTextItem()->GetValue() );
            }

          wxClientDC dc(grid);

          dc.BeginDrawing();
          grid->SetGridClippingRegion(& dc);
          grid->DrawCellBackground(& dc, &grid->GetCurrentRect(), grid->GetCursorRow(), grid->GetCursorColumn());
          grid->DrawCellValue(& dc, &grid->GetCurrentRect(), grid->GetCursorRow(), grid->GetCursorColumn());
          if ( !(m_editable && m_editInPlace ) ) grid->HighlightCell(& dc);
          dc.DestroyClippingRegion();
          dc.EndDrawing();

          //grid->OnCellChange(grid->GetCursorRow(), grid->GetCursorColumn());
          wxGridEvent g_evt(GetId(), wxEVT_GRID_CELL_CHANGE, grid,
                            grid->GetCursorRow(), grid->GetCursorColumn());
          GetEventHandler()->ProcessEvent(g_evt);

          //    grid->DrawCellText();
        }
    }
}

void wxGenericGrid::OnTextEnter(wxCommandEvent& WXUNUSED(ev) )
{
  // move the cursor down the current row (if possible)
  // when the enter key has been pressed
  //
  if ( m_editable )
    {
      if ( GetCursorRow() < GetRows()-1 )
        {
          wxClientDC dc( this );
          dc.BeginDrawing();
          OnSelectCellImplementation(& dc,
                                     GetCursorRow()+1,
                                     GetCursorColumn() );
          dc.EndDrawing();
        }
    }
}

void wxGenericGrid::OnTextInPlace(wxCommandEvent& ev )
{
  if ( m_editable )
    {
      wxGenericGrid *grid = this;
      wxGridCell *cell = grid->GetCell(grid->GetCursorRow(), grid->GetCursorColumn());
      if (cell && grid->CurrentCellVisible())
        {
          m_inOnTextInPlace = TRUE;
          grid->GetTextItem()->SetValue( m_inPlaceTextItem->GetValue() );
          OnText( ev );
          m_inOnTextInPlace = FALSE;
        }
    }
}

void wxGenericGrid::OnTextInPlaceEnter(wxCommandEvent& WXUNUSED(ev) )
{
  // move the cursor down the current row (if possible)
  // when the enter key has been pressed
  //
  if ( m_editable )
    {
      if ( GetCursorRow() < GetRows()-1 )
        {
          wxClientDC dc( this );
          dc.BeginDrawing();
          OnSelectCellImplementation(& dc,
                                     GetCursorRow()+1,
                                     GetCursorColumn() );
          dc.EndDrawing();
        }
    }
}

void wxGenericGrid::OnGridScroll(wxScrollEvent& ev)
{
  static bool inScroll = FALSE;

  if ( inScroll )
        return;

  if ( m_editInPlace ) m_inPlaceTextItem->Show(FALSE);

  inScroll = TRUE;
  wxGenericGrid *win = this;

  bool change = FALSE;

  if (ev.GetEventObject() == win->GetHorizScrollBar())
  {
    change = (ev.GetPosition() != m_scrollPosX);
    win->SetScrollPosX(ev.GetPosition());
  }
  else
  {
    change = (ev.GetPosition() != m_scrollPosY);
    win->SetScrollPosY(ev.GetPosition());
  }

  win->UpdateDimensions();

  win->SetCurrentRect(win->GetCursorRow(), win->GetCursorColumn());

  // Because rows and columns can be arbitrary sizes,
  // the scrollbars will need to be adjusted to reflect the
  // current view.
  AdjustScrollbars();

  if (change) win->Refresh(FALSE);

  if ( m_editInPlace && m_currentRectVisible )
    {
      m_inPlaceTextItem->SetSize( m_currentRect.x-2, m_currentRect.y-2,
                                  m_currentRect.width+4, m_currentRect.height+4 );
      m_inPlaceTextItem->Show( TRUE );
      m_inPlaceTextItem->SetFocus();
    }

  inScroll = FALSE;

}


//----------------------------------------------------------------------
// Default wxGridEvent handlers
//      (just redirect to the pre-existing virtual methods)

void wxGenericGrid::_OnSelectCell(wxGridEvent& ev)
{
    OnSelectCell(ev.m_row, ev.m_col);
}

void wxGenericGrid::_OnCreateCell(wxGridEvent& ev)
{
    ev.m_cell = OnCreateCell();
}

void wxGenericGrid::_OnChangeLabels(wxGridEvent& WXUNUSED(ev))
{
    OnChangeLabels();
}

void wxGenericGrid::_OnChangeSelectionLabel(wxGridEvent& WXUNUSED(ev))
{
    OnChangeSelectionLabel();
}

void wxGenericGrid::_OnCellChange(wxGridEvent& ev)
{
    OnCellChange(ev.m_row, ev.m_col);
}

void wxGenericGrid::_OnCellLeftClick(wxGridEvent& ev)
{
    OnCellLeftClick(ev.m_row, ev.m_col, ev.m_x, ev.m_y, ev.m_control, ev.m_shift);
}

void wxGenericGrid::_OnCellRightClick(wxGridEvent& ev)
{
    OnCellRightClick(ev.m_row, ev.m_col, ev.m_x, ev.m_y, ev.m_control, ev.m_shift);
}

void wxGenericGrid::_OnLabelLeftClick(wxGridEvent& ev)
{
    OnLabelLeftClick(ev.m_row, ev.m_col, ev.m_x, ev.m_y, ev.m_control, ev.m_shift);
}

void wxGenericGrid::_OnLabelRightClick(wxGridEvent& ev)
{
    OnLabelRightClick(ev.m_row, ev.m_col, ev.m_x, ev.m_y, ev.m_control, ev.m_shift);
}

void *wxGenericGrid::SetCellData(void *data, int row, int col)
{
    void *rc = NULL;

    wxGridCell *cell = GetCell(row, col);
    if ( cell )
        rc = cell->SetCellData(data);

    return rc;
}

void *wxGenericGrid::GetCellData(int row, int col)
{
    void *rc = NULL;

    wxGridCell *cell = GetCell(row, col);
    if ( cell )
        rc = cell->GetCellData();

    return rc;
}

