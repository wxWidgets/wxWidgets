/////////////////////////////////////////////////////////////////////////////
// Name:        tbarsmpl.cpp
// Purpose:     wxToolBarSimple
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "tbarsmpl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#if USE_TOOLBAR

#include "wx/tbarsmpl.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxToolBarSimple, wxToolBarBase)

BEGIN_EVENT_TABLE(wxToolBarSimple, wxToolBarBase)
	EVT_SIZE(wxToolBarSimple::OnSize)
	EVT_PAINT(wxToolBarSimple::OnPaint)
	EVT_KILL_FOCUS(wxToolBarSimple::OnKillFocus)
	EVT_MOUSE_EVENTS(wxToolBarSimple::OnMouseEvent)
END_EVENT_TABLE()
#endif

// TODO: eliminate these; use system colours
static wxPen * white_pen = NULL,
             * dark_grey_pen = NULL,
             * black_pen = NULL,
             * thick_black_pen;

wxToolBarSimple::wxToolBarSimple(void)
{
}

bool wxToolBarSimple::Create(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, const long style,
  const int direction, const int RowsOrColumns, const wxString& name )
{
	if ( ! wxWindow::Create(parent, id, pos, size, style, name) )
		return FALSE;

  // Set it to grey (or other 3D face colour)
  wxSystemSettings settings;
  SetBackgroundColour(settings.GetSystemColour(wxSYS_COLOUR_3DFACE));
  SetDefaultBackgroundColour(settings.GetSystemColour(wxSYS_COLOUR_3DFACE));

  if ( white_pen == 0 )
  {
    white_pen = new wxPen;
    white_pen->SetColour( "WHITE" );
  }
  if ( dark_grey_pen == 0 )
  {
    dark_grey_pen = new wxPen;
    dark_grey_pen->SetColour( 85,85,85 );
  }
  if ( black_pen == 0 )
  {
    black_pen = new wxPen;
    black_pen->SetColour( "BLACK" );
  }
  if ( thick_black_pen == 0 )
  {
    thick_black_pen = new wxPen("BLACK", 3, wxSOLID);
  }
  m_tilingDirection = direction;
  m_rowsOrColumns = RowsOrColumns;
  if ( m_tilingDirection == wxVERTICAL )
    { m_lastX = 3; m_lastY = 7; }
  else
    { m_lastX = 7; m_lastY = 3; }
  m_maxWidth = m_maxHeight = 0;
  m_pressedTool = m_currentTool = -1;
  m_xMargin = 0;
  m_yMargin = 0;
  m_toolPacking = 1;
  m_toolSeparation = 5;
  
  return TRUE;
}

wxToolBarSimple::~wxToolBarSimple ()
{
}

void wxToolBarSimple::OnPaint (wxPaintEvent& event)
{
  wxPaintDC dc(this);
  PrepareDC(dc);

  static int count = 0;
  // Prevent reentry of OnPaint which would cause wxMemoryDC errors.
  if ( count > 0 )
    return;
  count++;

  wxMemoryDC mem_dc;

  for ( wxNode *node = m_tools.First(); node; node = node->Next() )
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool->m_toolStyle == wxTOOL_STYLE_BUTTON)
      DrawTool(dc, mem_dc, tool);
  }

/* Old code which drew a line beneath the toolbar - not generally
 * wanted.
  int w, h;
  GetClientSize( &w, &h );
  wxPen * old_pen = dc.GetPen();
  dc.SetPen( *white_pen );
  dc.DrawLine(0,0,w,0);
  dc.SetPen( *black_pen );
  dc.DrawLine(0,(h-1),w,(h-1));
  dc.SetPen( *old_pen );
*/

  count--;
}

void wxToolBarSimple::OnSize ( wxSizeEvent& event )
{
  wxToolBarBase::OnSize(event);
}

void wxToolBarSimple::OnKillFocus (wxFocusEvent& event)
{
  OnMouseEnter(m_pressedTool = m_currentTool = -1);
}

void wxToolBarSimple::OnMouseEvent ( wxMouseEvent & event )
{
    long x, y;
    event.Position(&x, &y);
    wxToolBarTool *tool = FindToolForPosition(x, y);

    if (!tool)
      {
	  if (m_currentTool > -1)
	    {
		if (event.LeftIsDown())
		  SpringUpButton(m_currentTool);
		m_currentTool = -1;
		OnMouseEnter(-1);
	    }
	  return;
      }

    if (!event.IsButton())
      {
	  if (tool->m_index != m_currentTool)
	    {
		// If the left button is kept down and moved over buttons,
		// press those buttons.
		if (event.LeftIsDown() && tool->m_enabled) {
		    SpringUpButton(m_currentTool);
		    tool->m_toggleState = !tool->m_toggleState;
		    wxMemoryDC *dc2 = new wxMemoryDC;
			wxClientDC dc(this);
		    DrawTool(dc, *dc2, tool);
		    delete dc2;
		}
		OnMouseEnter(tool->m_index);
		m_currentTool = tool->m_index;
	    }
	  return;
      }

  // Left button pressed.
  if (event.LeftDown() && tool->m_enabled)
  {
      tool->m_toggleState = !tool->m_toggleState;
      wxMemoryDC *dc2 = new wxMemoryDC;
	  wxClientDC dc(this);
      DrawTool(dc, *dc2, tool);
      delete dc2;
  }
  else if (event.RightDown())
  {
    OnRightClick(tool->m_index, x, y);
  }
  // Left Button Released.  Only this action confirms selection.
  // If the button is enabled and it is not a toggle tool and it is
  // in the pressed state, then raise the button and call OnLeftClick.
  //
  if (event.LeftUp() && tool->m_enabled &&
      (tool->m_toggleState || tool->m_isToggle)){
      if (!tool->m_isToggle)
	tool->m_toggleState = FALSE;
      // Pass the OnLeftClick event to tool
      if (!OnLeftClick(tool->m_index, tool->m_toggleState) && tool->m_isToggle)
	// If it was a toggle, and OnLeftClick says No Toggle allowed,
	// then change it back
	tool->m_toggleState = !tool->m_toggleState;
    wxClientDC dc(this);
      wxMemoryDC *dc2 = new wxMemoryDC;
      DrawTool(dc, *dc2, tool);
      delete dc2;
  }
}

void wxToolBarSimple::DrawTool(wxDC& dc, wxMemoryDC& memDC, wxToolBarTool *tool)
{
  PrepareDC(dc);

  wxBitmap *bitmap = tool->m_toggleState ? (& tool->m_bitmap2) : (& tool->m_bitmap1);

  if (bitmap && bitmap->Ok())
  {
    if (bitmap->GetColourMap())
      memDC.SetPalette(*bitmap->GetColourMap());

    int ax = (int)tool->m_x,
        ay = (int)tool->m_y,
        bx = (int)(tool->m_x+tool->GetWidth()),
        by = (int)(tool->m_y+tool->GetHeight());

    memDC.SelectObject(*bitmap);
    if (m_windowStyle & wxTB_3DBUTTONS)
    {
      dc.SetClippingRegion(ax, ay, (bx-ax+1), (by-ay+1));
      dc.Blit((ax+1), (ay+1), (bx-ax-2), (by-ay-2), &memDC, 0, 0);
      wxPen * old_pen = dc.GetPen();
      dc.SetPen( *white_pen );
      dc.DrawLine(ax,(by-1),ax,ay);
      dc.DrawLine(ax,ay,(bx-1),ay);
      dc.SetPen( *dark_grey_pen );
      dc.DrawLine((bx-1),(ay+1),(bx-1),(by-1));
      dc.DrawLine((bx-1),(by-1),(ax+1),(by-1));
      dc.SetPen( *black_pen );
      dc.DrawLine(bx,ay,bx,by);
      dc.DrawLine(bx,by,ax,by);
      dc.SetPen( *old_pen );
      dc.DestroyClippingRegion();
      // Select bitmap out of the DC
    }
    else
    {
      dc.Blit(tool->m_x, tool->m_y,
            bitmap->GetWidth(), bitmap->GetHeight(),
            &memDC, 0, 0);
    }
    memDC.SelectObject(wxNullBitmap);
    memDC.SetPalette(wxNullPalette);
  }
  // No second bitmap, so draw a thick line around bitmap, or invert if mono
  else if (tool->m_toggleState)
  {
    bool drawBorder = FALSE;
    #ifdef __X__ // X doesn't invert properly on colour
    drawBorder = wxColourDisplay();
    #else       // Inversion works fine under Windows
    drawBorder = FALSE;
    #endif

    if (!drawBorder)
    {
      memDC.SelectObject(tool->m_bitmap1);
      dc.Blit(tool->m_x, tool->m_y, tool->GetWidth(), tool->GetHeight(),
               &memDC, 0, 0, wxSRC_INVERT);
      memDC.SelectObject(wxNullBitmap);
    }
    else
    {
      if (m_windowStyle & wxTB_3DBUTTONS)
      {
        int ax = (int)tool->m_x,
            ay = (int)tool->m_y,
            bx = (int)(tool->m_x+tool->GetWidth()),
            by = (int)(tool->m_y+tool->GetHeight());

        memDC.SelectObject(tool->m_bitmap1);
        dc.SetClippingRegion(ax, ay, (bx-ax+1), (by-ay+1));
        dc.Blit((ax+2), (ay+2), (bx-ax-2), (by-ay-2), &memDC, 0, 0);
        wxPen * old_pen = dc.GetPen();
        dc.SetPen( *black_pen );
        dc.DrawLine(ax,(by-1),ax,ay);
        dc.DrawLine(ax,ay,(bx-1),ay);
        dc.SetPen( *dark_grey_pen );
        dc.DrawLine((ax+1),(by-2),(ax+1),(ay+1));
        dc.DrawLine((ax+1),(ay+1),(bx-2),(ay+1));
        dc.SetPen( *white_pen );
        dc.DrawLine(bx,ay,bx,by);
        dc.DrawLine(bx,by,ax,by);
        dc.SetPen( *old_pen );
        dc.DestroyClippingRegion();
        memDC.SelectObject(wxNullBitmap);
      }
      else
      {
        long x = tool->m_x;
        long y = tool->m_y;
        long w = tool->m_bitmap1.GetWidth();
        long h = tool->m_bitmap1.GetHeight();

        memDC.SelectObject(tool->m_bitmap1);
        dc.SetClippingRegion(tool->m_x, tool->m_y, w, h);
        dc.Blit(tool->m_x, tool->m_y, w, h,
                 &memDC, 0, 0);
        dc.SetPen(*thick_black_pen);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(x, y, w-1, h-1);
        dc.DestroyClippingRegion();
        memDC.SelectObject(wxNullBitmap);
      }
    }
  }
}

void wxToolBarSimple::ToggleTool(const int index, const bool toggle)
{
  wxNode *node = m_tools.Find((long)index);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool && tool->m_isToggle)
    {
      bool oldState = tool->m_toggleState;
      tool->m_toggleState = toggle;

      if (oldState != toggle)
      {
        wxMemoryDC memDC;
		wxClientDC dc(this);
        DrawTool(dc, memDC, tool);
      }
    }
  }
}

// Okay, so we've left the tool we're in ... we must check if
// the tool we're leaving was a 'sprung push button' and if so,
// spring it back to the up state.
//
void wxToolBarSimple::SpringUpButton(const int index)
{
  wxNode *node=m_tools.Find((long)index);
  if (node) {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool && !tool->m_isToggle && tool->m_toggleState){
      tool->m_toggleState = FALSE;
      wxMemoryDC memDC;
	  wxClientDC dc(this);
      DrawTool(dc, memDC, tool);
    }
    else if (tool && tool->m_isToggle){
	tool->m_toggleState = !tool->m_toggleState;
    wxMemoryDC memDC;
	wxClientDC dc(this);
    DrawTool(dc, memDC, tool);
   }
  }
}

#endif
