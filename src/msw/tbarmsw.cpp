/////////////////////////////////////////////////////////////////////////////
// Name:        tbarmsw.cpp
// Purpose:     wxToolBar
// Author:      Julian Smart
// Modified by: 13.12.99 by VZ during toolbar classes reorganization
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

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

#if wxUSE_TOOLBAR && defined(__WIN16__)

#if !defined(__WIN32__) && !wxUSE_IMAGE_LOADING_IN_MSW
    #error wxToolBar needs wxUSE_IMAGE_LOADING_IN_MSW under Win16
#endif

#if !defined(__GNUWIN32__) && !defined(__SALFORDC__)
    #include "malloc.h"
#endif

#if !defined(__MWERKS__) && !defined(__SALFORDC__)
    #include <memory.h>
#endif

#include <stdlib.h>

#include "wx/msw/tbarmsw.h"
#include "wx/event.h"
#include "wx/app.h"
#include "wx/bitmap.h"
#include "wx/msw/private.h"
#include "wx/msw/dib.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define DEFAULTBITMAPX   16
#define DEFAULTBITMAPY   15
#define DEFAULTBUTTONX   24
#define DEFAULTBUTTONY   22
#define DEFAULTBARHEIGHT 27

//
// States (not all of them currently used)
//
#define wxTBSTATE_CHECKED         0x01    // radio button is checked
#define wxTBSTATE_PRESSED         0x02    // button is being depressed (any style)
#define wxTBSTATE_ENABLED         0x04    // button is enabled
#define wxTBSTATE_HIDDEN          0x08    // button is hidden
#define wxTBSTATE_INDETERMINATE   0x10    // button is indeterminate

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar,
                  int id,
                  const wxBitmap& bitmap1,
                  const wxBitmap& bitmap2,
                  bool toggle,
                  wxObject *clientData,
                  const wxString& shortHelpString,
                  const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, bitmap1, bitmap2, toggle,
                            clientData, shortHelpString, longHelpString)
    {
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control)
        : wxToolBarToolBase(tbar, control)
    {
    }

    void SetSize(const wxSize& size)
    {
        m_width = size.x;
        m_height = size.y;
    }

    long GetWidth() const { return m_width; }
    long GetHeight() const { return m_height; }

    wxCoord               m_x;
    wxCoord               m_y;
    wxCoord               m_width;
    wxCoord               m_height;
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl)

BEGIN_EVENT_TABLE(wxToolBar, wxToolBarBase)
	EVT_PAINT(wxToolBar::OnPaint)
	EVT_MOUSE_EVENTS(wxToolBar::OnMouseEvent)
END_EVENT_TABLE()
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

wxToolBarToolBase *wxToolBar::CreateTool(int id,
                                         const wxBitmap& bitmap1,
                                         const wxBitmap& bitmap2,
                                         bool toggle,
                                         wxObject *clientData,
                                         const wxString& shortHelpString,
                                         const wxString& longHelpString)
{
    return new wxToolBarTool(this, id, bitmap1, bitmap2, toggle,
                             clientData, shortHelpString, longHelpString);
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control)
{
    return new wxToolBarTool(this, control);
}

// ----------------------------------------------------------------------------
// wxToolBar
// ----------------------------------------------------------------------------

void wxToolBar::Init()
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

    m_xPos =
    m_yPos = -1;

    m_maxWidth = m_maxHeight = 0;
    m_pressedTool = m_currentTool = -1;
    m_toolPacking = 1;
    m_toolSeparation = 5;
}

bool wxToolBar::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return FALSE;

    if ( style & wxTB_HORIZONTAL )
    {
        m_lastX = 3;
        m_lastY = 7;
    }
    else
    {
        m_lastX = 7;
        m_lastY = 3;
    }

    // Set it to grey
    SetBackgroundColour(wxColour(192, 192, 192));

    InitGlobalObjects();

    return TRUE;
}

wxToolBar::~wxToolBar()
{
    FreeGlobalObjects();
}

void wxToolBar::SetToolBitmapSize(const wxSize& size)
{
    m_defaultWidth = size.x;
    m_defaultHeight = size.y;

    FreeGlobalObjects();
    InitGlobalObjects();
}

// The button size is bigger than the bitmap size
wxSize wxToolBar::GetToolSize() const
{
    return wxSize(m_defaultWidth + 8, m_defaultHeight + 7);
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    wxToolBarToolsList::Node *node = m_tools.GetFirst();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
        if ((x >= tool->m_x) && (y >= tool->m_y) &&
            (x <= (tool->m_x + tool->GetWidth())) &&
            (y <= (tool->m_y + tool->GetHeight())))
        {
            return tool;
        }

        node = node->GetNext();
    }

    return (wxToolBarToolBase *)NULL;
}

wxToolBarToolBase *wxToolBar::AddTool(int id,
                                      const wxBitmap& bitmap,
                                      const wxBitmap& pushedBitmap,
                                      bool toggle,
                                      wxCoord xPos,
                                      wxCoord yPos,
                                      wxObject *clientData,
                                      const wxString& helpString1,
                                      const wxString& helpString2)
{
    // rememeber the position for DoInsertTool()
    m_xPos = xPos;
    m_yPos = yPos;

    return wxToolBarBase::AddTool(id, bitmap, pushedBitmap, toggle,
                                  xPos, yPos, clientData,
                                  helpString1, helpString2);
}

void wxToolBar::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    static int wxOnPaintCount = 0;

    // Prevent reentry of OnPaint which would cause
    // wxMemoryDC errors.
    if (wxOnPaintCount > 0)
        return;
    wxOnPaintCount++;

    wxToolBarToolsList::Node *node = m_tools.GetFirst();
    while (node)
    {
        wxToolBarToolBase *tool = node->GetData();
        if ( tool->GetStyle()!= wxTOOL_STYLE_BUTTON )
        {
            int state = tool->IsEnabled() ? wxTBSTATE_ENABLED : 0;
            if ( tool->IsToggled() )
                state |= wxTBSTATE_CHECKED;

            DrawTool(dc, tool, state);
        }

        node = node->GetNext();
    }

    wxOnPaintCount--;
}

// If a Button is disabled, then NO function (besides leaving
// or entering) should be carried out. Therefore the additions
// of 'enabled' testing (Stefan Hammes).
void wxToolBar::OnMouseEvent(wxMouseEvent& event)
{
    static wxToolBarToolBase *eventCurrentTool = NULL;
    wxClientDC dc(this);

    if (event.Leaving())
    {
        m_currentTool = -1;
        if (eventCurrentTool && eventCurrentTool->IsEnabled())
        {
            ::ReleaseCapture();
            int state = wxTBSTATE_ENABLED;
            if (eventCurrentTool->IsToggled())
                state |= wxTBSTATE_CHECKED;
            DrawTool(dc, eventCurrentTool, state);
            eventCurrentTool = NULL;
        }
        OnMouseEnter(-1);
        return;
    }

    wxCoord x, y;
    event.GetPosition(&x, &y);
    wxToolBarToolBase *tool = FindToolForPosition(x, y);

    if (!tool)
    {
        if (eventCurrentTool && eventCurrentTool->IsEnabled())
        {
            ::ReleaseCapture();

            int state = wxTBSTATE_ENABLED;
            if (eventCurrentTool->IsToggled())
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
        if (tool->GetId() != m_currentTool)
        {
            OnMouseEnter(m_currentTool = tool->GetId());
            return;
        }
    }
    if (event.Dragging() && tool->IsEnabled())
    {
        if (eventCurrentTool)
        {
            // Might have dragged outside tool
            if (eventCurrentTool != tool)
            {
                int state = wxTBSTATE_ENABLED;
                if (tool->IsToggled())
                    state |= wxTBSTATE_CHECKED;
                DrawTool(dc, tool, state);
                eventCurrentTool = NULL;
                return;
            }
        }
        else
        {
            if (tool && event.LeftIsDown() && tool->IsEnabled())
            {
                eventCurrentTool = tool;
                ::SetCapture((HWND) GetHWND());
                int state = wxTBSTATE_ENABLED|wxTBSTATE_PRESSED;
                if (tool->IsToggled())
                    state |= wxTBSTATE_CHECKED;
                DrawTool(dc, tool, state);
            }
        }
    }
    if (event.LeftDown() && tool->IsEnabled())
    {
        eventCurrentTool = tool;
        ::SetCapture((HWND) GetHWND());
        int state = wxTBSTATE_ENABLED|wxTBSTATE_PRESSED;
        if (tool->IsToggled())
            state |= wxTBSTATE_CHECKED;
        DrawTool(dc, tool, state);
    }
    else if (event.LeftUp() && tool->IsEnabled())
    {
        if (eventCurrentTool)
            ::ReleaseCapture();
        if (eventCurrentTool == tool)
        {
            if (tool->CanBeToggled())
            {
                tool->Toggle();
                if (!OnLeftClick(tool->GetId(), tool->IsToggled()))
                {
                    tool->Toggle();
                }
                int state = wxTBSTATE_ENABLED;
                if (tool->IsToggled())
                    state |= wxTBSTATE_CHECKED;
                DrawTool(dc, tool, state);
            }
            else
            {
                int state = wxTBSTATE_ENABLED;
                if (tool->IsToggled())
                    state |= wxTBSTATE_CHECKED;
                DrawTool(dc, tool, state);
                OnLeftClick(tool->GetId(), tool->IsToggled());
            }
        }
        eventCurrentTool = NULL;
    }
    else if (event.RightDown() && tool->IsEnabled())
    {
        OnRightClick(tool->GetId(), x, y);
    }
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *tool, bool WXUNUSED(enable))
{
    DoRedrawTool(tool);
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *tool, bool WXUNUSED(toggle))
{
    DoRedrawTool(tool);
}

void wxToolBar::DoSetToggle(wxToolBarToolBase * WXUNUSED(tool),
                            bool WXUNUSED(toggle))
{
    // nothing to do
}

void wxToolBar::DoRedrawTool(wxToolBarToolBase *tool)
{
    wxClientDC dc(this);

    DrawTool(dc, tool);
}

void wxToolBar::DrawTool(wxDC& dc, wxToolBarToolBase *toolBase, int state)
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    DrawButton(dc.GetHDC(),
               tool->m_x, tool->m_y,
               tool->GetWidth(), tool->GetHeight(),
               tool, state);
}

void wxToolBar::DrawTool(wxDC& dc, wxToolBarToolBase *tool)
{
    int state = 0;
    if (tool->IsEnabled())
        state |= wxTBSTATE_ENABLED;
    if (tool->IsToggled())
        state |= wxTBSTATE_CHECKED;
    // how can i access the PRESSED state???

    DrawTool(dc, tool, state);
}

bool wxToolBar::DoDeleteTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase *tool)
{
    // VZ: didn't test whether it works, but why not...
    tool->Detach();

    Refresh();

    return TRUE;
}

bool wxToolBar::DoInsertTool(size_t pos, wxToolBarToolBase *toolBase)
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    wxCHECK_MSG( !tool->IsControl(), FALSE,
                 _T("generic wxToolBar doesn't support controls") );

    // TODO: use the mapping code from wxToolBar95 to get it right in this class
#if !defined(__WIN32__) && !defined(__WIN386__)
    wxBitmap bitmap2;
    if (tool->CanBeToggled())
    {
        HBITMAP hbmp = CreateMappedBitmap((WXHINSTANCE)wxGetInstance(),
                                          GetHbitmapOf(tool->GetBitmap1()));

        wxBitmap bmp;
        bmp.SetHBITMAP((WXHBITMAP)hbmp);
        tool->SetBitmap2(bmp);
    }
#endif

    tool->m_x = m_xPos;
    if ( tool->m_x == -1 )
        tool->m_x = m_xMargin;

    tool->m_y = m_yPos;
    if ( tool->m_y == -1 )
        tool->m_y = m_yMargin;

    tool->SetSize(GetToolSize());

    if ( tool->IsButton() )
    {
        // Calculate reasonable max size in case Layout() not called
        if ((tool->m_x + tool->GetBitmap1().GetWidth() + m_xMargin) > m_maxWidth)
            m_maxWidth = (tool->m_x + tool->GetWidth() + m_xMargin);

        if ((tool->m_y + tool->GetBitmap1().GetHeight() + m_yMargin) > m_maxHeight)
            m_maxHeight = (tool->m_y + tool->GetHeight() + m_yMargin);
    }

    return TRUE;
}

bool wxToolBar::Realize()
{
    m_currentRowsOrColumns = 0;
    m_lastX = m_xMargin;
    m_lastY = m_yMargin;
    int maxToolWidth = 0;
    int maxToolHeight = 0;
    m_maxWidth = 0;
    m_maxHeight = 0;

    // Find the maximum tool width and height
    wxToolBarToolsList::Node *node = m_tools.GetFirst();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
        if (tool->GetWidth() > maxToolWidth)
            maxToolWidth = tool->GetWidth();
        if (tool->GetHeight() > maxToolHeight)
            maxToolHeight = tool->GetHeight();
        node = node->GetNext();
    }

    int separatorSize = m_toolSeparation;

    node = m_tools.GetFirst();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
        if (tool->GetStyle() == wxTOOL_STYLE_SEPARATOR)
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
        else if (tool->GetStyle() == wxTOOL_STYLE_BUTTON)
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

        node = node->GetNext();
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

    return TRUE;
}

bool wxToolBar::InitGlobalObjects()
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

void wxToolBar::FreeGlobalObjects()
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

// ----------------------------------------------------------------------------
// drawing routines
// ----------------------------------------------------------------------------

void wxToolBar::PatB(WXHDC hdc,int x,int y,int dx,int dy, long rgb)
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

void wxToolBar::CreateMask(WXHDC hdc, int xoffset, int yoffset, int dx, int dy)
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

void wxToolBar::DrawBlankButton(WXHDC hdc, int x, int y, int dx, int dy, int state)
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

void wxToolBar::DrawButton(WXHDC hdc, int x, int y, int dx, int dy,
                           wxToolBarToolBase *toolBase, int state)
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

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
    if (tool->GetBitmap2().Ok())
      bitmapOld = GetHbitmapOf(tool->GetBitmap2());
    else
      bitmapOld = GetHbitmapOf(tool->GetBitmap1());
#else
    HBITMAP bitmapOld = GetHbitmapOf(tool->GetBitmap1());
#endif    

    bitmapOld = (HBITMAP)SelectObject(hdcGlyphs, bitmapOld);

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

// ----------------------------------------------------------------------------
// drawing helpers
// ----------------------------------------------------------------------------

void wxToolBar::GetSysColors()
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

WXHBITMAP wxToolBar::CreateDitherBitmap()
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

bool wxToolBar::CreateDitherBrush()
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

bool wxToolBar::FreeDitherBrush(void)
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

WXHBITMAP wxToolBar::CreateMappedBitmap(WXHINSTANCE WXUNUSED(hInstance), void *info)
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

WXHBITMAP wxToolBar::CreateMappedBitmap(WXHINSTANCE hInstance, WXHBITMAP hBitmap)
{
  HANDLE hDIB = wxBitmapToDIB((HBITMAP) hBitmap, 0);
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
