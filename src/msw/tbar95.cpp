/////////////////////////////////////////////////////////////////////////////
// Name:        tbar95.cpp
// Purpose:     wxToolBar95
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "tbar95.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#if wxUSE_BUTTONBAR && wxUSE_TOOLBAR && defined(__WIN95__)

#ifndef __GNUWIN32__
#include "malloc.h"
#endif

#include <windows.h>

#ifndef __GNUWIN32__
#include <commctrl.h>
#endif

#ifdef __GNUWIN32__
#include "wx/msw/gnuwin32/extra.h"
#endif

#include "wx/msw/dib.h"
#include "wx/tbar95.h"
#include "wx/app.h"
#include "wx/msw/private.h"

// Styles
#ifndef TBSTYLE_FLAT
#define TBSTYLE_LIST            0x1000
#define TBSTYLE_FLAT            0x0800
#define TBSTYLE_TRANSPARENT     0x8000
#endif
 // use TBSTYLE_TRANSPARENT if you use TBSTYLE_FLAT

// Messages
#ifndef TB_GETSTYLE
#define TB_GETSTYLE             (WM_USER + 57)
#define TB_SETSTYLE             (WM_USER + 56)
#endif

/* Hint from a newsgroup for custom flatbar drawing:
Set the TBSTYLE_CUSTOMERASE style, then handle the
NM_CUSTOMDRAW message and do your custom drawing.
*/

#define DEFAULTBITMAPX   16
#define DEFAULTBITMAPY   15
#define DEFAULTBUTTONX   24
#define DEFAULTBUTTONY   24
#define DEFAULTBARHEIGHT 27

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxToolBar95, wxToolBarBase)

BEGIN_EVENT_TABLE(wxToolBar95, wxToolBarBase)
	EVT_SIZE(wxToolBar95::OnSize)
	EVT_PAINT(wxToolBar95::OnPaint)
	EVT_KILL_FOCUS(wxToolBar95::OnKillFocus)
	EVT_MOUSE_EVENTS(wxToolBar95::OnMouseEvent)
    EVT_SYS_COLOUR_CHANGED(wxToolBar95::OnSysColourChanged)
END_EVENT_TABLE()
#endif

void wxMapBitmap(HBITMAP hBitmap, int width, int height);

wxToolBar95::wxToolBar95(void)
{
  m_maxWidth = -1;
  m_maxHeight = -1;
  m_hBitmap = 0;
  m_defaultWidth = DEFAULTBITMAPX;
  m_defaultHeight = DEFAULTBITMAPY;
}

bool wxToolBar95::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
  m_backgroundColour = wxColour(GetRValue(GetSysColor(COLOR_BTNFACE)),
  	GetGValue(GetSysColor(COLOR_BTNFACE)), GetBValue(GetSysColor(COLOR_BTNFACE)));
  m_foregroundColour = *wxBLACK ;

  if (style & wxTB_VERTICAL)
    wxMessageBox("Sorry, wxToolBar95 under Windows 95 only supports horizontal orientation.", "wxToolBar95 usage", wxOK);
  m_maxWidth = -1;
  m_maxHeight = -1;
  
  m_hBitmap = 0;

  m_defaultWidth = DEFAULTBITMAPX;
  m_defaultHeight = DEFAULTBITMAPY;
  SetName(name);

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  m_windowStyle = style;

  SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

  SetParent(parent);

  DWORD msflags = 0;
  if (style & wxBORDER)
    msflags |= WS_BORDER;
  msflags |= WS_CHILD | WS_VISIBLE;
  
  if (width <= 0)
    width = 100;
  if (height <= 0)
    height = 30;
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;

  m_windowId = (id < 0 ? NewControlId() : id);
  DWORD msStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | TBSTYLE_TOOLTIPS;

  if (style & wxTB_FLAT)
  {
    if (wxTheApp->GetComCtl32Version() > 400)
        msStyle |= TBSTYLE_FLAT;
  }

  // Create the toolbar control.
  HWND hWndToolbar = CreateWindowEx(0L,   // No extended styles.
    TOOLBARCLASSNAME,                 // Class name for the toolbar.
    "",                               // No default text.
    msStyle,    // Styles and defaults.
    x, y, width, height,                    // Standard toolbar size and position.
    (HWND) parent->GetHWND(),                       // Parent window of the toolbar.
    (HMENU)m_windowId,                // Toolbar ID.
    wxGetInstance(),                            // Current instance.
    NULL );                           // No class data.

  // Toolbar-specific initialisation
  ::SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), (LPARAM)0);

  m_hWnd = (WXHWND) hWndToolbar;
  if (parent) parent->AddChild(this);
  
  SubclassWin((WXHWND) hWndToolbar);

  return TRUE;
}

wxToolBar95::~wxToolBar95(void)
{
  UnsubclassWin();

  if (m_hBitmap)
  {
    ::DeleteObject((HBITMAP) m_hBitmap);
    m_hBitmap = 0;
  }
}

bool wxToolBar95::CreateTools(void)
{
  if (m_tools.Number() == 0)
    return FALSE;

  HBITMAP oldToolBarBitmap = (HBITMAP) m_hBitmap;
    
  int totalBitmapWidth = (int)(m_defaultWidth * m_tools.Number());
  int totalBitmapHeight = (int)m_defaultHeight;

  // Create a bitmap for all the tool bitmaps
  HDC dc = ::GetDC(NULL);
  m_hBitmap = (WXHBITMAP) ::CreateCompatibleBitmap(dc, totalBitmapWidth, totalBitmapHeight);
  ::ReleaseDC(NULL, dc);
  
  // Now blit all the tools onto this bitmap
  HDC memoryDC = ::CreateCompatibleDC(NULL);
  HBITMAP oldBitmap = (HBITMAP) ::SelectObject(memoryDC, (HBITMAP) m_hBitmap);

  HDC memoryDC2 = ::CreateCompatibleDC(NULL);
  int x = 0;
  wxNode *node = m_tools.First();
  int noButtons = 0;
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if ((tool->m_toolStyle != wxTOOL_STYLE_SEPARATOR) && tool->m_bitmap1.Ok() && tool->m_bitmap1.GetHBITMAP())
    {
//      wxPalette *palette = tool->m_bitmap1->GetPalette();

      HBITMAP oldBitmap2 = (HBITMAP) ::SelectObject(memoryDC2, (HBITMAP) tool->m_bitmap1.GetHBITMAP());
      /* int bltResult = */
	  BitBlt(memoryDC, x, 0, (int) m_defaultWidth, (int) m_defaultHeight, memoryDC2,
                        0, 0, SRCCOPY);
      ::SelectObject(memoryDC2, oldBitmap2);
      x += (int)m_defaultWidth;
      noButtons ++;
    }
    node = node->Next();
  }
  ::SelectObject(memoryDC, oldBitmap);
  ::DeleteDC(memoryDC);
  ::DeleteDC(memoryDC2);

  // Map to system colours
  wxMapBitmap((HBITMAP) m_hBitmap, totalBitmapWidth, totalBitmapHeight);

  if ( oldToolBarBitmap )
  {
    TBREPLACEBITMAP replaceBitmap;
    replaceBitmap.hInstOld = NULL;
    replaceBitmap.hInstNew = NULL;
    replaceBitmap.nIDOld = (UINT) oldToolBarBitmap;
    replaceBitmap.nIDNew = (UINT) (HBITMAP) m_hBitmap;
    replaceBitmap.nButtons = noButtons;
    if (::SendMessage((HWND) GetHWND(), TB_REPLACEBITMAP, (WPARAM) 0, (LPARAM) &replaceBitmap) == -1)
      wxMessageBox("Could not add bitmap to toolbar");

    ::DeleteObject((HBITMAP) oldToolBarBitmap);

    // Now delete all the buttons
    int i = 0;
    while ( TRUE )
    {
        // TODO: What about separators???? They don't have an id!
        if ( ! ::SendMessage( (HWND) GetHWND(), TB_DELETEBUTTON, i, 0 ) )
            break;
    }
  }
  else
  {
    TBADDBITMAP addBitmap;
    addBitmap.hInst = 0;
    addBitmap.nID = (UINT)m_hBitmap;
    if (::SendMessage((HWND) GetHWND(), TB_ADDBITMAP, (WPARAM) noButtons, (LPARAM) &addBitmap) == -1)
      wxMessageBox("Could not add bitmap to toolbar");
  }

  // Now add the buttons.
  TBBUTTON buttons[50];

  node = m_tools.First();
  int i = 0;
  int bitmapId = 0;
  while (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool->m_toolStyle == wxTOOL_STYLE_SEPARATOR)
    {
      buttons[i].iBitmap = 0;
      buttons[i].idCommand = 0;

      buttons[i].fsState = TBSTATE_ENABLED;
      buttons[i].fsStyle = TBSTYLE_SEP;
      buttons[i].dwData = 0L;
      buttons[i].iString = 0;
    }
    else
    {
      buttons[i].iBitmap = bitmapId;
      buttons[i].idCommand = tool->m_index;

      buttons[i].fsState = 0;
      if (tool->m_enabled)
        buttons[i].fsState |= TBSTATE_ENABLED;
      if (tool->m_toggleState)
        buttons[i].fsState |= TBSTATE_CHECKED;
      buttons[i].fsStyle = tool->m_isToggle ? TBSTYLE_CHECK : TBSTYLE_BUTTON;
      buttons[i].dwData = 0L;
      buttons[i].iString = 0;

      bitmapId ++;
    }
        
    i ++;
    node = node->Next();
  }

  int ans = (int)::SendMessage((HWND) GetHWND(), TB_ADDBUTTONS, (WPARAM)i, (LPARAM)& buttons);
  ans = (int)::SendMessage((HWND) GetHWND(), TB_AUTOSIZE, (WPARAM)0, (LPARAM) 0);

  RECT rect;
  ::SendMessage((HWND) GetHWND(), TB_SETROWS, MAKEWPARAM(m_maxRows, TRUE), (LPARAM) & rect);
  m_maxWidth = (rect.right - rect.left + 2);
  m_maxHeight = (rect.bottom - rect.top + 2);

  return TRUE;
}

bool wxToolBar95::MSWCommand(WXUINT cmd, WXWORD id)
{
  wxNode *node = m_tools.Find((long)id);
  if (!node)
    return FALSE;
  wxToolBarTool *tool = (wxToolBarTool *)node->Data();
  if (tool->m_isToggle)
    tool->m_toggleState = (1 == (1 & (int)::SendMessage((HWND) GetHWND(), TB_GETSTATE, (WPARAM) id, (LPARAM) 0)));

  BOOL ret = OnLeftClick((int)id, tool->m_toggleState);
  if (ret == FALSE && tool->m_isToggle)
  {
    tool->m_toggleState = !tool->m_toggleState;
    ::SendMessage((HWND) GetHWND(), TB_CHECKBUTTON, (WPARAM)id, (LPARAM)MAKELONG(tool->m_toggleState, 0));
  }
  return TRUE;
}

bool wxToolBar95::MSWNotify(WXWPARAM WXUNUSED(wParam), WXLPARAM lParam)
{
	// First check if this applies to us
    NMHDR *hdr = (NMHDR *)lParam;
    if (hdr->code != TTN_NEEDTEXT)
		return FALSE;

 	HWND toolTipWnd = (HWND) ::SendMessage((HWND) GetHWND(), TB_GETTOOLTIPS, 0, 0);
	if ( toolTipWnd != hdr->hwndFrom )
		return FALSE;

  LPTOOLTIPTEXT ttText = (LPTOOLTIPTEXT) lParam;
  int id = (int)ttText->hdr.idFrom;
  wxNode *node = m_tools.Find((long)id);
  if (!node)
    return FALSE;
  
  wxToolBarTool *tool = (wxToolBarTool *)node->Data();

  switch (ttText->hdr.code)
  {
    case TTN_NEEDTEXT:
    {
      if (tool->m_shortHelpString != "")
        ttText->lpszText = (char *) (const char *)tool->m_shortHelpString;

      // For backward compatibility...
      OnMouseEnter(tool->m_index);
      break;
    }
    default:
      return FALSE;
      break;
  }
  
  return TRUE;
}

void wxToolBar95::SetToolBitmapSize(const wxSize& size)
{
  m_defaultWidth = size.x; m_defaultHeight = size.y;
  ::SendMessage((HWND) GetHWND(), TB_SETBITMAPSIZE, 0, (LPARAM) MAKELONG ((int)size.x, (int)size.y));
}

void wxToolBar95::SetRows(int nRows)
{
  RECT rect;
  ::SendMessage((HWND) GetHWND(), TB_SETROWS, MAKEWPARAM(nRows, TRUE), (LPARAM) & rect);
  m_maxWidth = (rect.right - rect.left + 2);
  m_maxHeight = (rect.bottom - rect.top + 2);
}

wxSize wxToolBar95::GetMaxSize(void) const
{
  if ((m_maxWidth == -1) || (m_maxHeight == -1))
  {
    RECT rect;
    ::SendMessage((HWND) GetHWND(), TB_SETROWS, MAKEWPARAM(m_maxRows, TRUE), (LPARAM) & rect);
    ((wxToolBar95 *)this)->m_maxWidth = (rect.right - rect.left + 2); // ???
    ((wxToolBar95 *)this)->m_maxHeight = (rect.bottom - rect.top + 2); // ???
  }
  return wxSize(m_maxWidth, m_maxHeight);
}

void wxToolBar95::GetSize(int *w, int *h) const
{
  wxWindow::GetSize(w, h);
  // For some reason, the returned height is several pixels bigger than that
  // displayed!
  *h -= 2;
}

// The button size is bigger than the bitmap size
wxSize wxToolBar95::GetToolSize(void) const
{
  return wxSize(m_defaultWidth + 8, m_defaultHeight + 7);
}

void wxToolBar95::EnableTool(int toolIndex, bool enable)
{
  wxNode *node = m_tools.Find((long)toolIndex);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    tool->m_enabled = enable;
    ::SendMessage((HWND) GetHWND(), TB_ENABLEBUTTON, (WPARAM)toolIndex, (LPARAM)MAKELONG(enable, 0));
  }
}

void wxToolBar95::ToggleTool(int toolIndex, bool toggle)
{
  wxNode *node = m_tools.Find((long)toolIndex);
  if (node)
  {
    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool->m_isToggle)
    {
      tool->m_toggleState = toggle;
      ::SendMessage((HWND) GetHWND(), TB_CHECKBUTTON, (WPARAM)toolIndex, (LPARAM)MAKELONG(toggle, 0));
    }
  }
}

bool wxToolBar95::GetToolState(int toolIndex) const
{
    return (::SendMessage((HWND) GetHWND(), TB_ISBUTTONCHECKED, (WPARAM)toolIndex, (LPARAM)0) != 0);
}

void wxToolBar95::ClearTools(void)
{
  // TODO: Don't know how to reset the toolbar bitmap, as yet.
  // But adding tools and calling CreateTools should probably
  // recreate a buttonbar OK.
  wxToolBarBase::ClearTools();
}

// If pushedBitmap is NULL, a reversed version of bitmap is
// created and used as the pushed/toggled image.
// If toggle is TRUE, the button toggles between the two states.
wxToolBarTool *wxToolBar95::AddTool(int index, const wxBitmap& bitmap, const wxBitmap& pushedBitmap,
             bool toggle, long xPos, long yPos, wxObject *clientData, const wxString& helpString1, const wxString& helpString2)
{
  wxToolBarTool *tool = new wxToolBarTool(index, bitmap, wxNullBitmap, toggle, xPos, yPos, helpString1, helpString2);
  tool->m_clientData = clientData;

  if (xPos > -1)
    tool->m_x = xPos;
  else
    tool->m_x = m_xMargin;

  if (yPos > -1)
    tool->m_y = yPos;
  else
    tool->m_y = m_yMargin;

  tool->SetSize(GetDefaultButtonWidth(), GetDefaultButtonHeight());

  m_tools.Append((long)index, tool);
  return tool;
}

// Responds to colour changes, and passes event on to children.
void wxToolBar95::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    m_backgroundColour = wxColour(GetRValue(GetSysColor(COLOR_BTNFACE)),
  	    GetGValue(GetSysColor(COLOR_BTNFACE)), GetBValue(GetSysColor(COLOR_BTNFACE)));

    // Remap the buttons
    CreateTools();

    Default();

    Refresh();

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

// These are the default colors used to map the bitmap colors
// to the current system colors

#define BGR_BUTTONTEXT      (RGB(000,000,000))  // black
#define BGR_BUTTONSHADOW    (RGB(128,128,128))  // dark grey
#define BGR_BUTTONFACE      (RGB(192,192,192))  // bright grey
#define BGR_BUTTONHILIGHT   (RGB(255,255,255))  // white
#define BGR_BACKGROUNDSEL   (RGB(255,000,000))  // blue
#define BGR_BACKGROUND      (RGB(255,000,255))  // magenta

void wxMapBitmap(HBITMAP hBitmap, int width, int height)
{
  COLORMAP ColorMap[] = {
    {BGR_BUTTONTEXT,    COLOR_BTNTEXT},     // black
    {BGR_BUTTONSHADOW,  COLOR_BTNSHADOW},   // dark grey
    {BGR_BUTTONFACE,    COLOR_BTNFACE},     // bright grey
    {BGR_BUTTONHILIGHT, COLOR_BTNHIGHLIGHT},// white
    {BGR_BACKGROUNDSEL, COLOR_HIGHLIGHT},   // blue
    {BGR_BACKGROUND,    COLOR_WINDOW}       // magenta
  };

  int NUM_MAPS = (sizeof(ColorMap)/sizeof(COLORMAP));
  int n;
  for ( n = 0; n < NUM_MAPS; n++)
  {
    ColorMap[n].to = ::GetSysColor(ColorMap[n].to);
  }

  HBITMAP hbmOld;
  HDC hdcMem = CreateCompatibleDC(NULL);

  if (hdcMem)
  {
    hbmOld = (HBITMAP) SelectObject(hdcMem, hBitmap);

    int i, j, k;
    for ( i = 0; i < width; i++)
    {
        for ( j = 0; j < height; j++)
        {
            COLORREF pixel = ::GetPixel(hdcMem, i, j);
/*
            BYTE red = GetRValue(pixel);
            BYTE green = GetGValue(pixel);
            BYTE blue = GetBValue(pixel);
*/

            for ( k = 0; k < NUM_MAPS; k ++)
            {
                if ( ColorMap[k].from == pixel )
                {
                    /* COLORREF actualPixel = */ ::SetPixel(hdcMem, i, j, ColorMap[k].to);
                    break;
                }
            }
        }
    }


    SelectObject(hdcMem, hbmOld);
    DeleteObject(hdcMem);
  }

}

// Some experiments...
#if 0
  // What we want to do is create another bitmap which has a depth of 4,
  // and set the bits. So probably we want to convert this HBITMAP into a
  // DIB, then call SetDIBits.
  // AAAGH. The stupid thing is that if newBitmap has a depth of 4 (less than that of
  // the screen), then SetDIBits fails.
  HBITMAP newBitmap = ::CreateBitmap(totalBitmapWidth, totalBitmapHeight, 1, 4, NULL);
  HANDLE newDIB = ::BitmapToDIB((HBITMAP) m_hBitmap, NULL);
  LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER) GlobalLock(newDIB);

  dc = ::GetDC(NULL);
//  LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER) newDIB;

  int result = ::SetDIBits(dc, newBitmap, 0, lpbmi->biHeight, FindDIBBits((LPSTR)lpbmi), (LPBITMAPINFO)lpbmi,
    DIB_PAL_COLORS);
  DWORD err = GetLastError();

  ::ReleaseDC(NULL, dc);

  // Delete the DIB
  GlobalUnlock (newDIB);
  GlobalFree (newDIB);

//  WXHBITMAP hBitmap2 = wxCreateMappedBitmap((WXHINSTANCE) wxGetInstance(), (WXHBITMAP) m_hBitmap);
  // Substitute our new bitmap for the old one
  ::DeleteObject((HBITMAP) m_hBitmap);
  m_hBitmap = (WXHBITMAP) newBitmap;
#endif


#endif
