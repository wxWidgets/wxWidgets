/////////////////////////////////////////////////////////////////////////////
// Name:        tbarmsw.cpp
// Purpose:     wxToolBarMSW
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "tbarmsw.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if wxUSE_BUTTONBAR && wxUSE_TOOLBAR

#if !defined(__GNUWIN32__) && !defined(__SALFORDC__)
#include "malloc.h"
#endif

#if !defined(__MWERKS__) && !defined(__SALFORDC__)
#include <memory.h>
#endif

#include <stdlib.h>

#include "wx/tbarmsw.h"
#include "wx/event.h"
#include "wx/app.h"
#include "wx/bitmap.h"
#include "wx/msw/private.h"
#include "wx/msw/dib.h"

#define DEFAULTBITMAPX   16
#define DEFAULTBITMAPY   15
#define DEFAULTBUTTONX   24
#define DEFAULTBUTTONY   22
#define DEFAULTBARHEIGHT 27

/////// Non-Windows 95 implementation

#if !wxUSE_IMAGE_LOADING_IN_MSW
#error If wxUSE_IMAGE_LOADING_IN_MSW is set to 0, then wxUSE_BUTTONBAR must be set to 0 too.
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxToolBarMSW, wxToolBarBase)

BEGIN_EVENT_TABLE(wxToolBarMSW, wxToolBarBase)
	EVT_SIZE(wxToolBarMSW::OnSize)
	EVT_PAINT(wxToolBarMSW::OnPaint)
	EVT_MOUSE_EVENTS(wxToolBarMSW::OnMouseEvent)
END_EVENT_TABLE()
#endif

wxToolBarMSW::wxToolBarMSW(void)
{
  m_hbrDither = 0;
  m_rgbFace = 0;
  m_rgbShadow = 0;
  m_rgbHilight = 0;
  m_rgbFrame = 0;
  m_hdcMono = 0;
  m_hbmMono = 0;
  m_hbmDefault = 0;
  m_defaultWidth = DEFAULTBITMAPX;
  m_defaultHeight = DEFAULTBITMAPY;
}

bool wxToolBarMSW::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
	if ( ! wxWindow::Create(parent, id, pos, size, style, name) )
		return FALSE;

  if ( style & wxTB_HORIZONTAL )
    { m_lastX = 3; m_lastY = 7; }
  else
    { m_lastX = 7; m_lastY = 3; }
  m_maxWidth = m_maxHeight = 0;
  m_pressedTool = m_currentTool = -1;
  m_xMargin = 0;
  m_yMargin = 0;
  m_toolPacking = 1;
  m_toolSeparation = 5;

  // Set it to grey
  SetBackgroundColour(wxColour(192, 192, 192));

  m_hbrDither = 0;
  m_rgbFace = 0;
  m_rgbShadow = 0;
  m_rgbHilight = 0;
  m_rgbFrame = 0;
  m_hdcMono = 0;
  m_hbmMono = 0;
  m_hbmDefault = 0;
  m_defaultWidth = DEFAULTBITMAPX;
  m_defaultHeight = DEFAULTBITMAPY;

  InitGlobalObjects();

  return TRUE;
}

wxToolBarMSW::~wxToolBarMSW(void)
{
  FreeGlobalObjects();
}

void wxToolBarMSW::SetToolBitmapSize(const wxSize& size)
{
  m_defaultWidth = size.x; m_defaultHeight = size.y;
  FreeGlobalObjects();
  InitGlobalObjects();
}

// The button size is bigger than the bitmap size
wxSize wxToolBarMSW::GetToolSize(void) const
{
  return wxSize(m_defaultWidth + 8, m_defaultHeight + 7);
}

void wxToolBarMSW::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);

  static int wxOnPaintCount = 0;

  // Prevent reentry of OnPaint which would cause
  // wxMemoryDC errors.
  if (wxOnPaintCount > 0)
    return;
  wxOnPaintCount ++;

  wxNode *node = m_tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool->m_toolStyle != wxTOOL_STYLE_SEPARATOR)
    {
      int state = wxTBSTATE_ENABLED;
      if (!tool->m_enabled)
        state = 0;
      if (tool->m_isToggle && tool->m_toggleState)
        state |= wxTBSTATE_CHECKED;
      DrawTool(dc, tool, state);
    }
    node = node->Next();
  }
  wxOnPaintCount --;
}

void wxToolBarMSW::OnSize(wxSizeEvent& event)
{
  wxToolBarBase::OnSize(event);
}

// If a Button is disabled, then NO function (besides leaving
// or entering) should be carried out. Therefore the additions
// of 'enabled' testing (Stefan Hammes).
void wxToolBarMSW::OnMouseEvent(wxMouseEvent& event)
{
  static wxToolBarTool *eventCurrentTool = NULL;
  wxClientDC dc(this);

  if (event.Leaving())
  {
    m_currentTool = -1;
    if (eventCurrentTool && eventCurrentTool->m_enabled)
    {
      ::ReleaseCapture();
      int state = wxTBSTATE_ENABLED;
      if (eventCurrentTool->m_toggleState)
        state |= wxTBSTATE_CHECKED;
      DrawTool(dc, eventCurrentTool, state);
      eventCurrentTool = NULL;
    }
    OnMouseEnter(-1);
    return;
  }

  long x, y;
  event.Position(&x, &y);
  wxToolBarTool *tool = FindToolForPosition(x, y);

  if (!tool)
  {
    if (eventCurrentTool && eventCurrentTool->m_enabled)
    {
      ::ReleaseCapture();
      
      int state = wxTBSTATE_ENABLED;
      if (eventCurrentTool->m_toggleState)
        state |= wxTBSTATE_CHECKED;
      DrawTool(dc, eventCurrentTool, state);
      eventCurrentTool = NULL;
    }
    if (m_currentTool > -1)
    {
      m_currentTool = -1;
      OnMouseEnter(-1);
    }
    return;
  }
  
  if (!event.Dragging() && !event.IsButton())
  {
    if (tool->m_index != m_currentTool)
    {
      OnMouseEnter(tool->m_index);
      m_currentTool = tool->m_index;
      return;
    }
  }
  if (event.Dragging() && tool->m_enabled)
  {
    if (eventCurrentTool)
    {
      // Might have dragged outside tool
      if (eventCurrentTool != tool)
      {
        int state = wxTBSTATE_ENABLED;
        if (tool->m_toggleState)
          state |= wxTBSTATE_CHECKED;
        DrawTool(dc, tool, state);
        eventCurrentTool = NULL;
        return;
      }
    }
    else
    {
      if (tool && event.LeftIsDown() && tool->m_enabled)
      {
        eventCurrentTool = tool;
        ::SetCapture((HWND) GetHWND());
        int state = wxTBSTATE_ENABLED|wxTBSTATE_PRESSED;
        if (tool->m_toggleState)
          state |= wxTBSTATE_CHECKED;
        DrawTool(dc, tool, state);
      }
    }
  }
  if (event.LeftDown() && tool->m_enabled)
  {
    eventCurrentTool = tool;
    ::SetCapture((HWND) GetHWND());
    int state = wxTBSTATE_ENABLED|wxTBSTATE_PRESSED;
    if (tool->m_toggleState)
      state |= wxTBSTATE_CHECKED;
    DrawTool(dc, tool, state);
  }
  else if (event.LeftUp() && tool->m_enabled)
  {
    if (eventCurrentTool)
      ::ReleaseCapture();
    if (eventCurrentTool == tool)
    {
      if (tool->m_isToggle)
      {
        tool->m_toggleState = !tool->m_toggleState;
        if (!OnLeftClick(tool->m_index, tool->m_toggleState))
        {
          tool->m_toggleState = !tool->m_toggleState;
        }
        int state = wxTBSTATE_ENABLED;
        if (tool->m_toggleState)
          state |= wxTBSTATE_CHECKED;
        DrawTool(dc, tool, state);
      }
      else
      {
        int state = wxTBSTATE_ENABLED;
        if (tool->m_toggleState)
          state |= wxTBSTATE_CHECKED;
        DrawTool(dc, tool, state);
        OnLeftClick(tool->m_index, tool->m_toggleState);
      }
    }
    eventCurrentTool = NULL;
  }
  else if (event.RightDown() && tool->m_enabled)
  {
    OnRightClick(tool->m_index, x, y);
  }
}

// This function enables/disables a toolbar tool and redraws it.
// If that would not be done, the enabling/disabling wouldn't be
// visible on the screen.
void wxToolBarMSW::EnableTool(int toolIndex, bool enable)
{
  wxNode *node = m_tools.Find((long)toolIndex);
  if (node)
  {
	wxClientDC dc(this);

    // at first do the enabling/disabling in the base class
    wxToolBarBase::EnableTool(toolIndex,enable);
    // then calculate the state of the tool and draw it
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    int state = 0;
    if(tool->m_toggleState) state |= wxTBSTATE_CHECKED;
    if(tool->m_enabled) state |= wxTBSTATE_ENABLED;
    // how can i access the PRESSED state???
    DrawTool(dc, tool,state);
  }
}

void wxToolBarMSW::DrawTool(wxDC& dc, wxToolBarTool *tool, int state)
{
  DrawButton(dc.GetHDC(), (int)tool->m_x, (int)tool->m_y, (int)tool->GetWidth(), (int)tool->GetHeight(), tool, state);
}

void wxToolBarMSW::DrawTool(wxDC& dc, wxMemoryDC& , wxToolBarTool *tool)
{
  int state = wxTBSTATE_ENABLED;
  if (!tool->m_enabled)
    state = 0;
  if (tool->m_toggleState)
    state |= wxTBSTATE_CHECKED;
  DrawTool(dc, tool, state);
}

// If pushedBitmap is NULL, a reversed version of bitmap is
// created and used as the pushed/toggled image.
// If toggle is TRUE, the button toggles between the two states.
wxToolBarTool *wxToolBarMSW::AddTool(int index, const wxBitmap& bitmap, const wxBitmap& pushedBitmap,
             bool toggle, long xPos, long yPos, wxObject *clientData, const wxString& helpString1, const wxString& helpString2)
{
  // Using bitmap2 can cause problems (don't know why!)

  // TODO: use the mapping code from wxToolBar95 to get it right in this class
#if !defined(__WIN32__) && !defined(__WIN386__)
  wxBitmap bitmap2;
  if (toggle)
  {
    bitmap2.SetHBITMAP( (WXHBITMAP) CreateMappedBitmap((WXHINSTANCE)wxGetInstance(), (WXHBITMAP) ((wxBitmap& )bitmap).GetHBITMAP()));
  }

  wxToolBarTool *tool = new wxToolBarTool(index, bitmap, bitmap2, toggle, xPos, yPos, helpString1, helpString2);
#else
  wxToolBarTool *tool = new wxToolBarTool(index, bitmap, wxNullBitmap, toggle, xPos, yPos, helpString1, helpString2);
#endif

  tool->m_clientData = clientData;

  if (xPos > -1)
    tool->m_x = xPos;
  else
    tool->m_x = m_xMargin;

  if (yPos > -1)
    tool->m_y = yPos;
  else
    tool->m_y = m_yMargin;

  tool->m_deleteSecondBitmap = TRUE;
  tool->SetSize(GetToolSize().x, GetToolSize().y);
  
  // Calculate reasonable max size in case Layout() not called
  if ((tool->m_x + bitmap.GetWidth() + m_xMargin) > m_maxWidth)
    m_maxWidth = (tool->m_x + tool->GetWidth() + m_xMargin);

  if ((tool->m_y + bitmap.GetHeight() + m_yMargin) > m_maxHeight)
    m_maxHeight = (tool->m_y + tool->GetHeight() + m_yMargin);

  m_tools.Append((long)index, tool);
  return tool;
}

void wxToolBarMSW::LayoutTools()
{
  m_currentRowsOrColumns = 0;
  m_lastX = m_xMargin;
  m_lastY = m_yMargin;
  int maxToolWidth = 0;
  int maxToolHeight = 0;
  m_maxWidth = 0;
  m_maxHeight = 0;

  // Find the maximum tool width and height
  wxNode *node = m_tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool->GetWidth() > maxToolWidth)
      maxToolWidth = (int)tool->GetWidth();
    if (tool->GetHeight() > maxToolHeight)
      maxToolHeight = (int)tool->GetHeight();
    node = node->Next();
  }

  int separatorSize = m_toolSeparation;

  node = m_tools.First();
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool->m_toolStyle == wxTOOL_STYLE_SEPARATOR)
    {
      if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
      {
        if (m_currentRowsOrColumns >= m_maxCols)
          m_lastY += separatorSize;
        else
          m_lastX += separatorSize;
      }
      else
      {
        if (m_currentRowsOrColumns >= m_maxRows)
          m_lastX += separatorSize;
        else
          m_lastY += separatorSize;
      }
    }
    else if (tool->m_toolStyle == wxTOOL_STYLE_BUTTON)
    {
      if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
      {
        if (m_currentRowsOrColumns >= m_maxCols)
        {
          m_currentRowsOrColumns = 0;
          m_lastX = m_xMargin;
          m_lastY += maxToolHeight + m_toolPacking;
        }
        tool->m_x = (long) (m_lastX + (maxToolWidth - tool->GetWidth())/2.0);
        tool->m_y = (long) (m_lastY + (maxToolHeight - tool->GetHeight())/2.0);
  
        m_lastX += maxToolWidth + m_toolPacking;
      }
      else
      {
        if (m_currentRowsOrColumns >= m_maxRows)
        {
          m_currentRowsOrColumns = 0;
          m_lastX += (maxToolWidth + m_toolPacking);
          m_lastY = m_yMargin;
        }
        tool->m_x = (long) (m_lastX + (maxToolWidth - tool->GetWidth())/2.0);
        tool->m_y = (long) (m_lastY + (maxToolHeight - tool->GetHeight())/2.0);
  
        m_lastY += maxToolHeight + m_toolPacking;
      }
      m_currentRowsOrColumns ++;
    }
    
    if (m_lastX > m_maxWidth)
      m_maxWidth = m_lastX;
    if (m_lastY > m_maxHeight)
      m_maxHeight = m_lastY;

    node = node->Next();
  }
  if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
  {
    m_maxWidth += maxToolWidth;
    m_maxHeight += maxToolHeight;
  }
  else
  {
    m_maxWidth += maxToolWidth;
    m_maxHeight += maxToolHeight;
  }

  m_maxWidth += m_xMargin;
  m_maxHeight += m_yMargin;

  SetSize(m_maxWidth, m_maxHeight);
}


bool wxToolBarMSW::InitGlobalObjects(void)
{
  GetSysColors();      
  if (!CreateDitherBrush())
    return FALSE;

  m_hdcMono = (WXHDC) CreateCompatibleDC(NULL);
  if (!m_hdcMono)
      return FALSE;

  m_hbmMono = (WXHBITMAP) CreateBitmap((int)GetToolSize().x, (int)GetToolSize().y, 1, 1, NULL);
  if (!m_hbmMono)
      return FALSE;

  m_hbmDefault = (WXHBITMAP) SelectObject((HDC) m_hdcMono, (HBITMAP) m_hbmMono);
  return TRUE;
}

void wxToolBarMSW::FreeGlobalObjects(void)
{
    FreeDitherBrush();

    if (m_hdcMono) {
	if (m_hbmDefault)
	{
	    SelectObject((HDC) m_hdcMono, (HBITMAP) m_hbmDefault);
	    m_hbmDefault = 0;
	}
	DeleteDC((HDC) m_hdcMono);		// toast the DCs
    }
    m_hdcMono = 0;

    if (m_hbmMono)
		DeleteObject((HBITMAP) m_hbmMono);
    m_hbmMono = 0;
}


void wxToolBarMSW::PatB(WXHDC hdc,int x,int y,int dx,int dy, long rgb)
{
    RECT    rc;

    rc.left   = x;
    rc.top    = y;
    rc.right  = x + dx;
    rc.bottom = y + dy;

    SetBkColor((HDC) hdc,rgb);
    ExtTextOut((HDC) hdc,0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
}


// create a mono bitmap mask:
//   1's where color == COLOR_BTNFACE || COLOR_HILIGHT
//   0's everywhere else

void wxToolBarMSW::CreateMask(WXHDC hdc, int xoffset, int yoffset, int dx, int dy)
{
    HDC globalDC = ::GetDC(NULL);
    HDC hdcGlyphs = CreateCompatibleDC((HDC) globalDC);
    ReleaseDC(NULL, (HDC) globalDC);

    // krj - create a new bitmap and copy the image from hdc.
    //HBITMAP bitmapOld = SelectObject(hdcGlyphs, hBitmap);
    HBITMAP hBitmap = CreateCompatibleBitmap((HDC) hdc, dx, dy);
    HBITMAP bitmapOld = (HBITMAP) SelectObject(hdcGlyphs, hBitmap);
    BitBlt(hdcGlyphs, 0,0, dx, dy, (HDC) hdc, 0, 0, SRCCOPY);

    // initalize whole area with 1's
    PatBlt((HDC) m_hdcMono, 0, 0, dx, dy, WHITENESS);

    // create mask based on color bitmap
    // convert this to 1's
    SetBkColor(hdcGlyphs, m_rgbFace);
    BitBlt((HDC) m_hdcMono, xoffset, yoffset, (int)GetToolBitmapSize().x, (int)GetToolBitmapSize().y,
        hdcGlyphs, 0, 0, SRCCOPY);
    // convert this to 1's
    SetBkColor(hdcGlyphs, m_rgbHilight);
    // OR in the new 1's
    BitBlt((HDC) m_hdcMono, xoffset, yoffset, (int)GetToolBitmapSize().x, (int)GetToolBitmapSize().y,
        hdcGlyphs, 0, 0, SRCPAINT);

    SelectObject(hdcGlyphs, bitmapOld);
    DeleteObject(hBitmap);
    DeleteDC(hdcGlyphs);
}

void wxToolBarMSW::DrawBlankButton(WXHDC hdc, int x, int y, int dx, int dy, int state)
{
    // face color
    PatB(hdc, x, y, dx, dy, m_rgbFace);

    if (state & wxTBSTATE_PRESSED) {
	PatB(hdc, x + 1, y, dx - 2, 1, m_rgbFrame);
	PatB(hdc, x + 1, y + dy - 1, dx - 2, 1, m_rgbFrame);
	PatB(hdc, x, y + 1, 1, dy - 2, m_rgbFrame);
	PatB(hdc, x + dx - 1, y +1, 1, dy - 2, m_rgbFrame);
	PatB(hdc, x + 1, y + 1, 1, dy-2, m_rgbShadow);
	PatB(hdc, x + 1, y + 1, dx-2, 1, m_rgbShadow);
    }
    else {
	PatB(hdc, x + 1, y, dx - 2, 1, m_rgbFrame);
	PatB(hdc, x + 1, y + dy - 1, dx - 2, 1, m_rgbFrame);
	PatB(hdc, x, y + 1, 1, dy - 2, m_rgbFrame);
	PatB(hdc, x + dx - 1, y + 1, 1, dy - 2, m_rgbFrame);
	dx -= 2;
	dy -= 2;
	PatB(hdc, x + 1, y + 1, 1, dy - 1, m_rgbHilight);
	PatB(hdc, x + 1, y + 1, dx - 1, 1, m_rgbHilight);
	PatB(hdc, x + dx, y + 1, 1, dy, m_rgbShadow);
	PatB(hdc, x + 1, y + dy, dx, 1,   m_rgbShadow);
	PatB(hdc, x + dx - 1, y + 2, 1, dy - 2, m_rgbShadow);
	PatB(hdc, x + 2, y + dy - 1, dx - 2, 1,   m_rgbShadow);
    }
}

void wxToolBarMSW::DrawButton(WXHDC hdc, int x, int y, int dx, int dy, wxToolBarTool *tool, int state)
{
    int yOffset;
    HBRUSH hbrOld, hbr;
    BOOL bMaskCreated = FALSE;
    int xButton = 0;		// assume button is down
    int dxFace, dyFace;
    int xCenterOffset;

    dxFace = dx;
    dyFace = dy;

//    HBITMAP hBitmap = (HBITMAP) tool->m_bitmap1.GetHBITMAP();
    HDC globalDC = ::GetDC(NULL);
    HDC hdcGlyphs = CreateCompatibleDC(globalDC);
    ReleaseDC(NULL, globalDC);

    // get the proper button look - up or down.
    if (!(state & (wxTBSTATE_PRESSED | wxTBSTATE_CHECKED))) {
	xButton = dx;	// use 'up' version of button
	dxFace -= 2;	
	dyFace -= 2;	// extents to ignore button highlight
    }

    DrawBlankButton(hdc, x, y, dx, dy, state);


    // move coordinates inside border and away from upper left highlight.
    // the extents change accordingly.
    x += 2;
    y += 2;
    dxFace -= 3;		
    dyFace -= 3;

    // Using bitmap2 can cause problems (don't know why!)
#if !defined(__WIN32__) && !defined(__WIN386__)
    HBITMAP bitmapOld;
    if (tool->m_bitmap2.Ok())
      bitmapOld = (HBITMAP) SelectObject(hdcGlyphs, (HBITMAP) tool->m_bitmap2.GetHBITMAP());
    else
      bitmapOld = (HBITMAP) SelectObject(hdcGlyphs, (HBITMAP) tool->m_bitmap1.GetHBITMAP());
#else
    HBITMAP bitmapOld = (HBITMAP) SelectObject(hdcGlyphs, (HBITMAP) tool->m_bitmap1.GetHBITMAP());
#endif    

    // calculate offset of face from (x,y).  y is always from the top,
    // so the offset is easy.  x needs to be centered in face.
    yOffset = 1;
    xCenterOffset = (dxFace - (int)GetToolBitmapSize().x)/2;
    if (state & (wxTBSTATE_PRESSED | wxTBSTATE_CHECKED))
    {
	// pressed state moves down and to the right
	// (x moves automatically as face size grows)
        yOffset++;
    }

    // now put on the face
    if (state & wxTBSTATE_ENABLED) {
        // regular version
        BitBlt((HDC) hdc, x+xCenterOffset, y + yOffset, (int)GetToolBitmapSize().x, (int)GetToolBitmapSize().y,
            hdcGlyphs, 0, 0, SRCCOPY);
    } else {
        // disabled version (or indeterminate)
	bMaskCreated = TRUE;
        CreateMask((WXHDC) hdcGlyphs, xCenterOffset, yOffset, dxFace, dyFace);
//        CreateMask(hBitmap, xCenterOffset, yOffset, dxFace, dyFace);

	SetTextColor((HDC) hdc, 0L);	 // 0's in mono -> 0 (for ROP)
	SetBkColor((HDC) hdc, 0x00FFFFFF); // 1's in mono -> 1

	// draw glyph's white understrike
	if (!(state & wxTBSTATE_INDETERMINATE)) {
	    hbr = CreateSolidBrush(m_rgbHilight);
	    if (hbr) {
	        hbrOld = (HBRUSH) SelectObject((HDC) hdc, hbr);
	        if (hbrOld) {
	            // draw hilight color where we have 0's in the mask
                    BitBlt((HDC) hdc, x + 1, y + 1, dxFace, dyFace, (HDC) m_hdcMono, 0, 0, 0x00B8074A);
	            SelectObject((HDC) hdc, hbrOld);
	        }
	        DeleteObject(hbr);
	    }
	}

	// gray out glyph
	hbr = CreateSolidBrush(m_rgbShadow);
	if (hbr) {
	    hbrOld = (HBRUSH) SelectObject((HDC) hdc, hbr);
	    if (hbrOld) {
	        // draw the shadow color where we have 0's in the mask
                BitBlt((HDC) hdc, x, y, dxFace, dyFace, (HDC) m_hdcMono, 0, 0, 0x00B8074A);
	        SelectObject((HDC) hdc, hbrOld);
	    }
	    DeleteObject(hbr);
	}

	if (state & wxTBSTATE_CHECKED) {
	    BitBlt((HDC) m_hdcMono, 1, 1, dxFace - 1, dyFace - 1, (HDC) m_hdcMono, 0, 0, SRCAND);
	}
    }

    if (state & (wxTBSTATE_CHECKED | wxTBSTATE_INDETERMINATE)) {

        hbrOld = (HBRUSH) SelectObject((HDC) hdc, (HBRUSH) m_hbrDither);
	if (hbrOld) {

	    if (!bMaskCreated)
                CreateMask((WXHDC) hdcGlyphs, xCenterOffset, yOffset, dxFace, dyFace);
//                CreateMask(hBitmap, xCenterOffset, yOffset, dxFace, dyFace);

	    SetTextColor((HDC) hdc, 0L);		// 0 -> 0
	    SetBkColor((HDC) hdc, 0x00FFFFFF);	// 1 -> 1

	    // only draw the dither brush where the mask is 1's
            BitBlt((HDC) hdc, x, y, dxFace, dyFace, (HDC) m_hdcMono, 0, 0, 0x00E20746);

	    SelectObject((HDC) hdc, hbrOld);
	}
    }
    SelectObject(hdcGlyphs, bitmapOld);
    DeleteDC(hdcGlyphs);
}

void wxToolBarMSW::GetSysColors(void)
{
	static COLORREF rgbSaveFace    = 0xffffffffL,
	                rgbSaveShadow  = 0xffffffffL,
	                rgbSaveHilight = 0xffffffffL,
	                rgbSaveFrame   = 0xffffffffL;

    // For now, override these because the colour replacement isn't working,
    // and we get inconsistent colours. Assume all buttons are grey for the moment.

//	m_rgbFace    = GetSysColor(COLOR_BTNFACE);
        m_rgbFace = RGB(192,192,192);
//	m_rgbShadow  = GetSysColor(COLOR_BTNSHADOW);
        m_rgbShadow = RGB(128,128,128);
//	m_rgbHilight = GetSysColor(COLOR_BTNHIGHLIGHT);
	m_rgbHilight = RGB(255, 255, 255);

	m_rgbFrame   = GetSysColor(COLOR_WINDOWFRAME);

	if (rgbSaveFace!=m_rgbFace || rgbSaveShadow!=m_rgbShadow
		|| rgbSaveHilight!=m_rgbHilight || rgbSaveFrame!=m_rgbFrame)
	{
		rgbSaveFace    = m_rgbFace;
		rgbSaveShadow  = m_rgbShadow;
		rgbSaveHilight = m_rgbHilight;
        rgbSaveFrame   = m_rgbFrame;

		// Update the brush for pushed-in buttons
		CreateDitherBrush();
	}
}

WXHBITMAP wxToolBarMSW::CreateDitherBitmap()
{
    BITMAPINFO* pbmi;
    HBITMAP hbm;
    HDC hdc;
    int i;
    long patGray[8];
    DWORD rgb;

    pbmi = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER) + 16*sizeof(RGBQUAD));
    memset(pbmi, 0, (sizeof(BITMAPINFOHEADER) + 16*sizeof(RGBQUAD)));

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = 8;
    pbmi->bmiHeader.biHeight = 8;
    pbmi->bmiHeader.biPlanes = 1;
    pbmi->bmiHeader.biBitCount = 1;
    pbmi->bmiHeader.biCompression = BI_RGB;

//    rgb = GetSysColor(COLOR_BTNFACE);
    rgb = RGB(192,192,192);

    pbmi->bmiColors[0].rgbBlue  = GetBValue(rgb);
    pbmi->bmiColors[0].rgbGreen = GetGValue(rgb);
    pbmi->bmiColors[0].rgbRed   = GetRValue(rgb);
    pbmi->bmiColors[0].rgbReserved = 0;

//    rgb = GetSysColor(COLOR_BTNHIGHLIGHT);
    rgb = RGB(255, 255, 255);

    pbmi->bmiColors[1].rgbBlue  = GetBValue(rgb);
    pbmi->bmiColors[1].rgbGreen = GetGValue(rgb);
    pbmi->bmiColors[1].rgbRed   = GetRValue(rgb);
    pbmi->bmiColors[1].rgbReserved = 0;

    /* initialize the brushes */

    for (i = 0; i < 8; i++)
       if (i & 1)
           patGray[i] = 0xAAAA5555L;   //  0x11114444L; // lighter gray
       else
           patGray[i] = 0x5555AAAAL;   //  0x11114444L; // lighter gray

    hdc = ::GetDC(NULL);

    hbm = CreateDIBitmap(hdc, &pbmi->bmiHeader, CBM_INIT, patGray, pbmi, DIB_RGB_COLORS);

    ReleaseDC(NULL, hdc);
    free(pbmi);

    return (WXHBITMAP)hbm;
}

bool wxToolBarMSW::CreateDitherBrush(void)
{
	HBITMAP hbmGray;
	HBRUSH hbrSave;
	if (m_hbrDither)
	  return TRUE;
	hbmGray = (HBITMAP) CreateDitherBitmap();

	if (hbmGray)
	{
		hbrSave = (HBRUSH) m_hbrDither;
		m_hbrDither = (WXHBRUSH) CreatePatternBrush(hbmGray);
		DeleteObject(hbmGray);
		if (m_hbrDither)
		{
			if (hbrSave)
			{
				DeleteObject(hbrSave);
			}
			return TRUE;
		}
		else
		{
			m_hbrDither = (WXHBRUSH) hbrSave;
		}
	}

	return FALSE;
}

bool wxToolBarMSW::FreeDitherBrush(void)
{
    if (m_hbrDither)
      DeleteObject((HBRUSH) m_hbrDither);
    m_hbrDither = 0;
    return TRUE;
}

typedef struct tagCOLORMAP2
{
    COLORREF bgrfrom;
    COLORREF bgrto;
    COLORREF sysColor;
} COLORMAP2;

// these are the default colors used to map the dib colors
// to the current system colors

#define BGR_BUTTONTEXT      (RGB(000,000,000))  // black
#define BGR_BUTTONSHADOW    (RGB(128,128,128))  // dark grey
#define BGR_BUTTONFACE      (RGB(192,192,192))  // bright grey
#define BGR_BUTTONHILIGHT   (RGB(255,255,255))  // white
#define BGR_BACKGROUNDSEL   (RGB(255,000,000))  // blue
#define BGR_BACKGROUND      (RGB(255,000,255))  // magenta
#define FlipColor(rgb)      (RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb)))

WXHBITMAP wxToolBarMSW::CreateMappedBitmap(WXHINSTANCE WXUNUSED(hInstance), void *info)
{
  LPBITMAPINFOHEADER lpBitmapInfo = (LPBITMAPINFOHEADER)info;
  HDC			hdc, hdcMem = NULL;

  DWORD FAR		*p;
  LPSTR 		lpBits;
  HBITMAP		hbm = NULL, hbmOld;
  int numcolors, i;
  int wid, hgt;
  static COLORMAP2 ColorMap[] = {
    {BGR_BUTTONTEXT,    BGR_BUTTONTEXT,    COLOR_BTNTEXT},     // black
    {BGR_BUTTONSHADOW,  BGR_BUTTONSHADOW,  COLOR_BTNSHADOW},   // dark grey
    {BGR_BUTTONFACE,    BGR_BUTTONFACE,    COLOR_BTNFACE},     // bright grey
    {BGR_BUTTONHILIGHT, BGR_BUTTONHILIGHT, COLOR_BTNHIGHLIGHT},// white
    {BGR_BACKGROUNDSEL, BGR_BACKGROUNDSEL, COLOR_HIGHLIGHT},   // blue
    {BGR_BACKGROUND,    BGR_BACKGROUND,    COLOR_WINDOW}       // magenta
  };

  #define NUM_MAPS (sizeof(ColorMap)/sizeof(COLORMAP2))

  if (!lpBitmapInfo)
  	return 0;

  //
  // So what are the new colors anyway ?
  //
  for (i=0; i < (int) NUM_MAPS; i++) {
     ColorMap[i].bgrto = (long unsigned int) FlipColor(GetSysColor((int)ColorMap[i].sysColor));
  }

  p = (DWORD FAR *)(((LPSTR)lpBitmapInfo) + lpBitmapInfo->biSize);

  /* Replace button-face and button-shadow colors with the current values
   */
  numcolors = 16;

  while (numcolors-- > 0) {
      for (i = 0; i < (int) NUM_MAPS; i++) {
          if (*p == ColorMap[i].bgrfrom) {
          *p = ColorMap[i].bgrto;
	      break;
	  }
      }
      p++;
  }

  /* First skip over the header structure */
  lpBits = (LPSTR)(lpBitmapInfo + 1);

  /* Skip the color table entries, if any */
  lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

  /* Create a color bitmap compatible with the display device */
  i = wid = (int)lpBitmapInfo->biWidth;
  hgt = (int)lpBitmapInfo->biHeight;
  hdc = ::GetDC(NULL);

  hdcMem = CreateCompatibleDC(hdc);
  if (hdcMem) {
//    hbm = CreateDiscardableBitmap(hdc, i, hgt);
    hbm = CreateCompatibleBitmap(hdc, i, hgt);
    if (hbm) {
        hbmOld = (HBITMAP) SelectObject(hdcMem, hbm);

        // set the main image
        StretchDIBits(hdcMem, 0, 0, wid, hgt, 0, 0, wid, hgt, lpBits,
                   (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

        SelectObject(hdcMem, hbmOld);
    }

    DeleteObject(hdcMem);
  }

  ReleaseDC(NULL, hdc);

  return (WXHBITMAP) hbm;
}

WXHBITMAP wxToolBarMSW::CreateMappedBitmap(WXHINSTANCE hInstance, WXHBITMAP hBitmap)
{
  HANDLE hDIB = BitmapToDIB((HBITMAP) hBitmap, 0);
  if (hDIB)
  {
#ifdef __WINDOWS_386__
    LPBITMAPINFOHEADER lpbmInfoHdr = (LPBITMAPINFOHEADER)MK_FP32(GlobalLock(hDIB));
#else
    LPBITMAPINFOHEADER lpbmInfoHdr = (LPBITMAPINFOHEADER)GlobalLock(hDIB);
#endif
    HBITMAP newBitmap = (HBITMAP) CreateMappedBitmap((WXHINSTANCE) wxGetInstance(), lpbmInfoHdr);
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    return (WXHBITMAP) newBitmap;
  }
  return 0;
}

#endif
