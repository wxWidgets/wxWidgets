/////////////////////////////////////////////////////////////////////////////
// Name:        serext.cpp
// Purpose:     Serialization: Other classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     July 1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "serext.h"
#endif

#include "serext.h"
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/grid.h>
#include <wx/objstrm.h>
#include <wx/datstrm.h>

IMPLEMENT_SERIAL_CLASS(wxSplitterWindow, wxWindow)
IMPLEMENT_SERIAL_CLASS(wxGridCell, wxObject)
IMPLEMENT_SERIAL_CLASS(wxGrid, wxPanel)

void WXSERIAL(wxSplitterWindow)::StoreObject(wxObjectOutputStream& s)
{
  wxSplitterWindow *splitter = (wxSplitterWindow *)Object();
  WXSERIAL(wxWindow)::StoreObject(s);

  if (s.FirstStage()) {
    s.AddChild( splitter->GetWindow1() );
    s.AddChild( splitter->GetWindow2() );
    return;
  }

  wxDataOutputStream data_s(s);
  data_s.Write8( splitter->GetSplitMode() );
  data_s.Write32( splitter->GetSashSize() );
  data_s.Write8( splitter->GetBorderSize() );
  data_s.Write32( splitter->GetSashPosition() );
  data_s.Write32( splitter->GetMinimumPaneSize() );
}

void WXSERIAL(wxSplitterWindow)::LoadObject(wxObjectInputStream& s)
{
  wxSplitterWindow *splitter = (wxSplitterWindow *)Object();
  WXSERIAL(wxWindow)::LoadObject(s);

  wxDataInputStream data_s(s);
  int split_mode, sash_size, border_size, sash_position, min_pane_size;

  split_mode    = data_s.Read8();
  sash_size     = data_s.Read32();
  border_size   = data_s.Read8();
  sash_position = data_s.Read32();
  min_pane_size = data_s.Read32();

  splitter->Create(m_parent, m_id, wxPoint(m_x, m_y), wxSize(m_w, m_h), m_style,
                   m_name);
  
  if (s.GetChild(1)) {
    if (data_s.Read8() == wxSPLIT_VERTICAL)
      splitter->SplitVertically((wxWindow *)s.GetChild(0),
                                (wxWindow *)s.GetChild(1), sash_position);
    else
      splitter->SplitHorizontally((wxWindow *)s.GetChild(0),
                                  (wxWindow *)s.GetChild(1), sash_position);
  }

  splitter->SetSashSize(sash_size);
  splitter->SetBorderSize(border_size);
  splitter->SetMinimumPaneSize(min_pane_size);
}

void WXSERIAL(wxGridCell)::StoreObject(wxObjectOutputStream& s)
{
  wxGridCell *cell = (wxGridCell *)Object();
  wxDataOutputStream data_s(s);

  if (s.FirstStage()) {
    s.AddChild( cell->GetFont() );
    s.AddChild( cell->GetBackgroundBrush() );
    s.AddChild( cell->GetCellBitmap() );
    s.AddChild( &(cell->GetTextColour()) );
    s.AddChild( &(cell->GetBackgroundColour()) );
    return;
  }

  data_s.WriteString( cell->GetTextValue() );
  data_s.Write16( cell->GetAlignment() );
}

void WXSERIAL(wxGridCell)::LoadObject(wxObjectInputStream& s)
{
  wxGridCell *cell = (wxGridCell *)Object();
  wxDataInputStream data_s(s);

  cell->SetTextValue( data_s.ReadString() );
  cell->SetAlignment( data_s.Read16() );
  cell->SetFont( (wxFont *)s.GetChild() );
  cell->SetBackgroundBrush( (wxBrush *)s.GetChild() );
  cell->SetCellBitmap( (wxBitmap *)s.GetChild() );
  cell->SetTextColour( *((wxColour *)s.GetChild()) );
  cell->SetBackgroundColour( *((wxColour *)s.GetChild()) );
}

void WXSERIAL(wxGrid)::StoreObject(wxObjectOutputStream& s)
{
  wxDataOutputStream data_s(s);
  wxGrid *grid = (wxGrid *)Object();
  int n_rows = grid->GetRows(), n_cols = grid->GetCols();
  int r, c;

  if (s.FirstStage()) {
    for (r=0;r<n_rows;r++)
      for (c=0;c<n_cols;c++)
	s.AddChild( grid->GetCell(r, c) );

    s.AddChild( grid->GetDividerPen() );
    WXSERIAL(wxPanel)::StoreObject(s);
    return;
  }

  data_s.Write16( n_rows );
  data_s.Write16( n_cols );
  data_s.Write16( grid->GetCursorRow() );
  data_s.Write16( grid->GetCursorColumn() );

  WXSERIAL(wxPanel)::StoreObject(s);
}

void WXSERIAL(wxGrid)::LoadObject(wxObjectInputStream& s)
{
  WXSERIAL(wxPanel)::LoadObject(s);
}
