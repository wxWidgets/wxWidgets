/////////////////////////////////////////////////////////////////////////////
// Name:        generic/tbarsmpl.cpp
// Purpose:     wxToolBarSimple
// Author:      Julian Smart
// Modified by: VZ on 14.12.99 during wxToolBarSimple reorganization
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
    #pragma implementation "tbarsmpl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TOOLBAR_SIMPLE

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/window.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
#endif

#include "wx/tbarsmpl.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxToolBarToolSimple : public wxToolBarToolBase
{
public:
    wxToolBarToolSimple(wxToolBarSimple *tbar,
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

    wxToolBarToolSimple(wxToolBarSimple *tbar, wxControl *control)
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

IMPLEMENT_DYNAMIC_CLASS(wxToolBarSimple, wxControl)

BEGIN_EVENT_TABLE(wxToolBarSimple, wxToolBarBase)
	EVT_SIZE(wxToolBarSimple::OnSize)
    EVT_SCROLL(wxToolBarSimple::OnScroll)
	EVT_PAINT(wxToolBarSimple::OnPaint)
	EVT_KILL_FOCUS(wxToolBarSimple::OnKillFocus)
	EVT_MOUSE_EVENTS(wxToolBarSimple::OnMouseEvent)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// tool bar tools creation
// ----------------------------------------------------------------------------

wxToolBarToolBase *wxToolBarSimple::CreateTool(int id,
                                               const wxBitmap& bitmap1,
                                               const wxBitmap& bitmap2,
                                               bool toggle,
                                               wxObject *clientData,
                                               const wxString& shortHelpString,
                                               const wxString& longHelpString)
{
    return new wxToolBarToolSimple(this, id, bitmap1, bitmap2, toggle,
                                   clientData, shortHelpString, longHelpString);
}

wxToolBarToolBase *wxToolBarSimple::CreateTool(wxControl *control)
{
    return new wxToolBarToolSimple(this, control);
}

// ----------------------------------------------------------------------------
// wxToolBarSimple creation
// ----------------------------------------------------------------------------

void wxToolBarSimple::Init()
{
    m_currentRowsOrColumns = 0;

    m_lastX =
    m_lastY = 0;

    m_maxWidth =
    m_maxHeight = 0;

    m_pressedTool =
    m_currentTool = -1;

    m_xPos =
    m_yPos = -1;

    m_toolPacking = 1;
    m_toolSeparation = 5;

    m_defaultWidth = 16;
    m_defaultHeight = 15;
}

wxToolBarToolBase *wxToolBarSimple::AddTool(int id,
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

bool wxToolBarSimple::DoInsertTool(size_t pos, wxToolBarToolBase *toolBase)
{
    wxToolBarToolSimple *tool = (wxToolBarToolSimple *)toolBase;

    wxCHECK_MSG( !tool->IsControl(), FALSE,
                 _T("generic wxToolBarSimple doesn't support controls") );

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

bool wxToolBarSimple::DoDeleteTool(size_t WXUNUSED(pos),
                                   wxToolBarToolBase *tool)
{
    // VZ: didn't test whether it works, but why not...
    tool->Detach();

    Refresh();

    return TRUE;
}

bool wxToolBarSimple::Create(wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
{
    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return FALSE;

    // Set it to grey (or other 3D face colour)
    wxSystemSettings settings;
    SetBackgroundColour(settings.GetSystemColour(wxSYS_COLOUR_3DFACE));

    if ( GetWindowStyleFlag() & wxTB_VERTICAL )
    {
        m_lastX = 7;
        m_lastY = 3;

        m_maxRows = 32000;      // a lot
        m_maxCols = 1;
    }
    else
    {
        m_lastX = 3;
        m_lastY = 7;

        m_maxRows = 1;
        m_maxCols = 32000;      // a lot
    }

    SetCursor(*wxSTANDARD_CURSOR);

    return TRUE;
}

wxToolBarSimple::~wxToolBarSimple()
{
}

bool wxToolBarSimple::Realize()
{
    m_currentRowsOrColumns = 0;
    m_lastX = m_xMargin;
    m_lastY = m_yMargin;
    m_maxWidth = 0;
    m_maxHeight = 0;

    int maxToolWidth = 0;
    int maxToolHeight = 0;

    // Find the maximum tool width and height
    wxToolBarToolsList::Node *node = m_tools.GetFirst();
    while ( node )
    {
        wxToolBarToolSimple *tool = (wxToolBarToolSimple *)node->GetData();
        if ( tool->GetWidth() > maxToolWidth )
            maxToolWidth = tool->GetWidth();
        if (tool->GetHeight() > maxToolHeight)
            maxToolHeight = tool->GetHeight();

        node = node->GetNext();
    }

    int separatorSize = m_toolSeparation;

    node = m_tools.GetFirst();
    while ( node )
    {
        wxToolBarToolSimple *tool = (wxToolBarToolSimple *)node->GetData();
        if ( tool->IsSeparator() )
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
        else if ( tool->IsButton() )
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
        else
        {
            // TODO: support the controls
        }

        if (m_lastX > m_maxWidth)
            m_maxWidth = m_lastX;
        if (m_lastY > m_maxHeight)
            m_maxHeight = m_lastY;

        node = node->GetNext();
    }

    if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
        m_maxWidth += maxToolWidth;
    else
        m_maxHeight += maxToolHeight;

    m_maxWidth += m_xMargin;
    m_maxHeight += m_yMargin;

    return TRUE;
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void wxToolBarSimple::OnPaint (wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);

    static int count = 0;
    // Prevent reentry of OnPaint which would cause wxMemoryDC errors.
    if ( count > 0 )
        return;
    count++;

    for ( wxToolBarToolsList::Node *node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxToolBarToolBase *tool = node->GetData();
        if ( tool->IsButton() )
            DrawTool(dc, tool);
    }

    count--;
}

void wxToolBarSimple::OnSize (wxSizeEvent& WXUNUSED(event))
{
#if wxUSE_CONSTRAINTS
    if (GetAutoLayout())
        Layout();
#endif

    AdjustScrollbars();
}

void wxToolBarSimple::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    OnMouseEnter(m_pressedTool = m_currentTool = -1);
}

void wxToolBarSimple::OnMouseEvent(wxMouseEvent & event)
{
    wxCoord x, y;
    event.GetPosition(&x, &y);
    wxToolBarToolSimple *tool = (wxToolBarToolSimple *)FindToolForPosition(x, y);

    if (event.LeftDown())
    {
        CaptureMouse();
    }
    if (event.LeftUp())
    {
        ReleaseMouse();
    }

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
        if ( tool->GetId() != m_currentTool )
        {
            // If the left button is kept down and moved over buttons,
            // press those buttons.
            if ( event.LeftIsDown() && tool->IsEnabled() )
            {
                SpringUpButton(m_currentTool);

                if ( tool->CanBeToggled() )
                {
                    tool->Toggle();
                }

                DrawTool(tool);
            }

            m_currentTool = tool->GetId();
            OnMouseEnter(m_currentTool);
        }
        return;
    }

    // Left button pressed.
    if ( event.LeftDown() && tool->IsEnabled() )
    {
        if ( tool->CanBeToggled() )
        {
            tool->Toggle();
        }

        DrawTool(tool);
    }
    else if (event.RightDown())
    {
        OnRightClick(tool->GetId(), x, y);
    }

    // Left Button Released.  Only this action confirms selection.
    // If the button is enabled and it is not a toggle tool and it is
    // in the pressed state, then raise the button and call OnLeftClick.
    //
    if ( event.LeftUp() && tool->IsEnabled() )
    {
        // Pass the OnLeftClick event to tool
        if ( !OnLeftClick(tool->GetId(), tool->IsToggled()) &&
                          tool->CanBeToggled() )
        {
            // If it was a toggle, and OnLeftClick says No Toggle allowed,
            // then change it back
            tool->Toggle();
        }

        DrawTool(tool);
    }
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxToolBarSimple::DrawTool(wxToolBarToolBase *tool)
{
    wxClientDC dc(this);
    DrawTool(dc, tool);
}

void wxToolBarSimple::DrawTool(wxDC& dc, wxToolBarToolBase *toolBase)
{
    wxToolBarToolSimple *tool = (wxToolBarToolSimple *)toolBase;

    wxMemoryDC memDC;
    PrepareDC(dc);

    wxPen dark_grey_pen(wxColour( 85,85,85 ), 1, wxSOLID);
    wxPen white_pen("WHITE", 1, wxSOLID);
    wxPen black_pen("BLACK", 1, wxSOLID);

    wxBitmap bitmap = tool->GetBitmap();

    if ( bitmap.Ok() )
    {
#ifndef __WXGTK__
        if (bitmap.GetPalette())
            memDC.SetPalette(*bitmap.GetPalette());
#endif

        int ax = (int)tool->m_x,
        ay = (int)tool->m_y,
        bx = (int)(tool->m_x+tool->GetWidth()),
        by = (int)(tool->m_y+tool->GetHeight());

        memDC.SelectObject(bitmap);
        if (m_windowStyle & wxTB_3DBUTTONS)
        {
            dc.SetClippingRegion(ax, ay, (bx-ax+1), (by-ay+1));
            dc.Blit((ax+1), (ay+1), (bx-ax-2), (by-ay-2), &memDC, 0, 0);
            wxPen * old_pen = & dc.GetPen();
            dc.SetPen( white_pen );
            dc.DrawLine(ax,(by-1),ax,ay);
            dc.DrawLine(ax,ay,(bx-1),ay);
            dc.SetPen( dark_grey_pen );
            dc.DrawLine((bx-1),(ay+1),(bx-1),(by-1));
            dc.DrawLine((bx-1),(by-1),(ax+1),(by-1));
            dc.SetPen( black_pen );
            dc.DrawLine(bx,ay,bx,by);
            dc.DrawLine(bx,by,ax,by);
            dc.SetPen( *old_pen );
            dc.DestroyClippingRegion();
            // Select bitmap out of the DC
        }
        else
        {
            dc.Blit(tool->m_x, tool->m_y,
                    bitmap.GetWidth(), bitmap.GetHeight(),
                    &memDC, 0, 0);
        }
        memDC.SelectObject(wxNullBitmap);
#ifndef __WXGTK__
        memDC.SetPalette(wxNullPalette);
#endif
    }
    // No second bitmap, so draw a thick line around bitmap, or invert if mono
    else if ( tool->IsToggled() )
    {
        bool drawBorder = FALSE;
#ifdef __X__ // X doesn't invert properly on colour
        drawBorder = wxColourDisplay();
#else       // Inversion works fine under Windows
        drawBorder = FALSE;
#endif

        if (!drawBorder)
        {
            memDC.SelectObject(tool->GetBitmap1());
            dc.Blit(tool->m_x, tool->m_y, tool->GetWidth(), tool->GetHeight(),
                    &memDC, 0, 0, wxSRC_INVERT);
            memDC.SelectObject(wxNullBitmap);
        }
        else
        {
            bitmap = tool->GetBitmap1();

            if (m_windowStyle & wxTB_3DBUTTONS)
            {
                int ax = (int)tool->m_x,
                ay = (int)tool->m_y,
                bx = (int)(tool->m_x+tool->GetWidth()),
                by = (int)(tool->m_y+tool->GetHeight());

                memDC.SelectObject(bitmap);
                dc.SetClippingRegion(ax, ay, (bx-ax+1), (by-ay+1));
                dc.Blit((ax+2), (ay+2), (bx-ax-2), (by-ay-2), &memDC, 0, 0);
                wxPen * old_pen = & dc.GetPen();
                dc.SetPen( black_pen );
                dc.DrawLine(ax,(by-1),ax,ay);
                dc.DrawLine(ax,ay,(bx-1),ay);
                dc.SetPen( dark_grey_pen );
                dc.DrawLine((ax+1),(by-2),(ax+1),(ay+1));
                dc.DrawLine((ax+1),(ay+1),(bx-2),(ay+1));
                dc.SetPen( white_pen );
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
                long w = bitmap.GetWidth();
                long h = bitmap.GetHeight();
                wxPen thick_black_pen("BLACK", 3, wxSOLID);

                memDC.SelectObject(bitmap);
                dc.SetClippingRegion(tool->m_x, tool->m_y, w, h);
                dc.Blit(tool->m_x, tool->m_y, w, h,
                        &memDC, 0, 0);
                dc.SetPen(thick_black_pen);
                dc.SetBrush(*wxTRANSPARENT_BRUSH);
                dc.DrawRectangle(x, y, w-1, h-1);
                dc.DestroyClippingRegion();
                memDC.SelectObject(wxNullBitmap);
            }
        }
    }
}

// ----------------------------------------------------------------------------
// toolbar geometry
// ----------------------------------------------------------------------------

void wxToolBarSimple::SetRows(int nRows)
{
    wxCHECK_RET( nRows != 0, _T("max number of rows must be > 0") );

    m_maxCols = (GetToolsCount() + nRows - 1) / nRows;

    AdjustScrollbars();
    Refresh();
}

wxToolBarToolBase *wxToolBarSimple::FindToolForPosition(wxCoord x,
                                                        wxCoord y) const
{
    wxToolBarToolsList::Node *node = m_tools.GetFirst();
    while (node)
    {
        wxToolBarToolSimple *tool = (wxToolBarToolSimple *)node->GetData();
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

// ----------------------------------------------------------------------------
// tool state change handlers
// ----------------------------------------------------------------------------

void wxToolBarSimple::DoEnableTool(wxToolBarToolBase *tool,
                                   bool WXUNUSED(enable))
{
    DrawTool(tool);
}

void wxToolBarSimple::DoToggleTool(wxToolBarToolBase *tool,
                                   bool WXUNUSED(toggle))
{
    DrawTool(tool);
}

void wxToolBarSimple::DoSetToggle(wxToolBarToolBase * WXUNUSED(tool),
                                  bool WXUNUSED(toggle))
{
    // nothing to do
}

// Okay, so we've left the tool we're in ... we must check if the tool we're
// leaving was a 'sprung push button' and if so, spring it back to the up
// state.
void wxToolBarSimple::SpringUpButton(int id)
{
    wxToolBarToolBase *tool = FindById(id);

    if ( tool && tool->CanBeToggled() )
    {
        tool->Toggle();

        DrawTool(tool);
    }
}

// ----------------------------------------------------------------------------
// scrolling implementation
// ----------------------------------------------------------------------------

/*
 * pixelsPerUnitX/pixelsPerUnitY: number of pixels per unit (e.g. pixels per text line)
 * noUnitsX/noUnitsY:        : no. units per scrollbar
 */
void wxToolBarSimple::SetScrollbars (int pixelsPerUnitX, int pixelsPerUnitY,
                                   int noUnitsX, int noUnitsY,
                                   int xPos, int yPos)
{
    m_xScrollPixelsPerLine = pixelsPerUnitX;
    m_yScrollPixelsPerLine = pixelsPerUnitY;
    m_xScrollLines = noUnitsX;
    m_yScrollLines = noUnitsY;

    int w, h;
    GetSize(&w, &h);

    // Recalculate scroll bar range and position
    if (m_xScrollLines > 0)
    {
        m_xScrollPosition = xPos;
        SetScrollPos (wxHORIZONTAL, m_xScrollPosition, TRUE);
    }
    else
    {
        SetScrollbar(wxHORIZONTAL, 0, 0, 0, FALSE);
        m_xScrollPosition = 0;
    }

    if (m_yScrollLines > 0)
    {
        m_yScrollPosition = yPos;
        SetScrollPos (wxVERTICAL, m_yScrollPosition, TRUE);
    }
    else
    {
        SetScrollbar(wxVERTICAL, 0, 0, 0, FALSE);
        m_yScrollPosition = 0;
    }
    AdjustScrollbars();
    Refresh();

#if 0 //def __WXMSW__
    ::UpdateWindow ((HWND) GetHWND());
#endif
}

void wxToolBarSimple::OnScroll(wxScrollEvent& event)
{
    int orient = event.GetOrientation();

    int nScrollInc = CalcScrollInc(event);
    if (nScrollInc == 0)
        return;

    if (orient == wxHORIZONTAL)
    {
        int newPos = m_xScrollPosition + nScrollInc;
        SetScrollPos(wxHORIZONTAL, newPos, TRUE );
    }
    else
    {
        int newPos = m_yScrollPosition + nScrollInc;
        SetScrollPos(wxVERTICAL, newPos, TRUE );
    }

    if (orient == wxHORIZONTAL)
    {
        if (m_xScrollingEnabled)
            ScrollWindow(-m_xScrollPixelsPerLine * nScrollInc, 0, NULL);
        else
            Refresh();
    }
    else
    {
        if (m_yScrollingEnabled)
            ScrollWindow(0, -m_yScrollPixelsPerLine * nScrollInc, NULL);
        else
            Refresh();
    }

    if (orient == wxHORIZONTAL)
    {
        m_xScrollPosition += nScrollInc;
    }
    else
    {
        m_yScrollPosition += nScrollInc;
    }

}

int wxToolBarSimple::CalcScrollInc(wxScrollEvent& event)
{
    int pos = event.GetPosition();
    int orient = event.GetOrientation();

    int nScrollInc = 0;
    switch (event.GetEventType())
    {
        case wxEVT_SCROLL_TOP:
            {
                if (orient == wxHORIZONTAL)
                    nScrollInc = - m_xScrollPosition;
                else
                    nScrollInc = - m_yScrollPosition;
                break;
            }
        case wxEVT_SCROLL_BOTTOM:
            {
                if (orient == wxHORIZONTAL)
                    nScrollInc = m_xScrollLines - m_xScrollPosition;
                else
                    nScrollInc = m_yScrollLines - m_yScrollPosition;
                break;
            }
        case wxEVT_SCROLL_LINEUP:
            {
                nScrollInc = -1;
                break;
            }
        case wxEVT_SCROLL_LINEDOWN:
            {
                nScrollInc = 1;
                break;
            }
        case wxEVT_SCROLL_PAGEUP:
            {
                if (orient == wxHORIZONTAL)
                    nScrollInc = -GetScrollPageSize(wxHORIZONTAL);
                else
                    nScrollInc = -GetScrollPageSize(wxVERTICAL);
                break;
            }
        case wxEVT_SCROLL_PAGEDOWN:
            {
                if (orient == wxHORIZONTAL)
                    nScrollInc = GetScrollPageSize(wxHORIZONTAL);
                else
                    nScrollInc = GetScrollPageSize(wxVERTICAL);
                break;
            }
        case wxEVT_SCROLL_THUMBTRACK:
            {
                if (orient == wxHORIZONTAL)
                    nScrollInc = pos - m_xScrollPosition;
                else
                    nScrollInc = pos - m_yScrollPosition;
                break;
            }
        default:
            {
                break;
            }
    }
    if (orient == wxHORIZONTAL)
    {
        int w, h;
        GetClientSize(&w, &h);

        int nMaxWidth = m_xScrollLines*m_xScrollPixelsPerLine;
        int noPositions = (int) ( ((nMaxWidth - w)/(float)m_xScrollPixelsPerLine) + 0.5 );
        if (noPositions < 0)
            noPositions = 0;

        if ( (m_xScrollPosition + nScrollInc) < 0 )
            nScrollInc = -m_xScrollPosition; // As -ve as we can go
        else if ( (m_xScrollPosition + nScrollInc) > noPositions )
            nScrollInc = noPositions - m_xScrollPosition; // As +ve as we can go

        return nScrollInc;
    }
    else
    {
        int w, h;
        GetClientSize(&w, &h);

        int nMaxHeight = m_yScrollLines*m_yScrollPixelsPerLine;
        int noPositions = (int) ( ((nMaxHeight - h)/(float)m_yScrollPixelsPerLine) + 0.5 );
        if (noPositions < 0)
            noPositions = 0;

        if ( (m_yScrollPosition + nScrollInc) < 0 )
            nScrollInc = -m_yScrollPosition; // As -ve as we can go
        else if ( (m_yScrollPosition + nScrollInc) > noPositions )
            nScrollInc = noPositions - m_yScrollPosition; // As +ve as we can go

        return nScrollInc;
    }
}

// Adjust the scrollbars - new version.
void wxToolBarSimple::AdjustScrollbars()
{
    int w, h;
    GetClientSize(&w, &h);

    // Recalculate scroll bar range and position
    if (m_xScrollLines > 0)
    {
        int nMaxWidth = m_xScrollLines*m_xScrollPixelsPerLine;
        int newRange = (int) ( ((nMaxWidth)/(float)m_xScrollPixelsPerLine) + 0.5 );
        if (newRange < 0)
            newRange = 0;

        m_xScrollPosition = wxMin(newRange, m_xScrollPosition);

        // Calculate page size i.e. number of scroll units you get on the
        // current client window
        int noPagePositions = (int) ( (w/(float)m_xScrollPixelsPerLine) + 0.5 );
        if (noPagePositions < 1)
            noPagePositions = 1;

        SetScrollbar(wxHORIZONTAL, m_xScrollPosition, noPagePositions, newRange);
        SetScrollPageSize(wxHORIZONTAL, noPagePositions);
    }
    if (m_yScrollLines > 0)
    {
        int nMaxHeight = m_yScrollLines*m_yScrollPixelsPerLine;
        int newRange = (int) ( ((nMaxHeight)/(float)m_yScrollPixelsPerLine) + 0.5 );
        if (newRange < 0)
            newRange = 0;

        m_yScrollPosition = wxMin(newRange, m_yScrollPosition);

        // Calculate page size i.e. number of scroll units you get on the
        // current client window
        int noPagePositions = (int) ( (h/(float)m_yScrollPixelsPerLine) + 0.5 );
        if (noPagePositions < 1)
            noPagePositions = 1;

        SetScrollbar(wxVERTICAL, m_yScrollPosition, noPagePositions, newRange);
        SetScrollPageSize(wxVERTICAL, noPagePositions);
    }
}

// Prepare the DC by translating it according to the current scroll position
void wxToolBarSimple::PrepareDC(wxDC& dc)
{
    dc.SetDeviceOrigin(- m_xScrollPosition * m_xScrollPixelsPerLine, - m_yScrollPosition * m_yScrollPixelsPerLine);
}

void wxToolBarSimple::GetScrollPixelsPerUnit (int *x_unit, int *y_unit) const
{
      *x_unit = m_xScrollPixelsPerLine;
      *y_unit = m_yScrollPixelsPerLine;
}

int wxToolBarSimple::GetScrollPageSize(int orient) const
{
    if ( orient == wxHORIZONTAL )
        return m_xScrollLinesPerPage;
    else
        return m_yScrollLinesPerPage;
}

void wxToolBarSimple::SetScrollPageSize(int orient, int pageSize)
{
    if ( orient == wxHORIZONTAL )
        m_xScrollLinesPerPage = pageSize;
    else
        m_yScrollLinesPerPage = pageSize;
}

/*
 * Scroll to given position (scroll position, not pixel position)
 */
void wxToolBarSimple::Scroll (int x_pos, int y_pos)
{
    int old_x, old_y;
    ViewStart (&old_x, &old_y);
    if (((x_pos == -1) || (x_pos == old_x)) && ((y_pos == -1) || (y_pos == old_y)))
        return;

    if (x_pos > -1)
    {
        m_xScrollPosition = x_pos;
        SetScrollPos (wxHORIZONTAL, x_pos, TRUE);
    }
    if (y_pos > -1)
    {
        m_yScrollPosition = y_pos;
        SetScrollPos (wxVERTICAL, y_pos, TRUE);
    }
    Refresh();

#if 0 //def __WXMSW__
    UpdateWindow ((HWND) GetHWND());
#endif
}

void wxToolBarSimple::EnableScrolling (bool x_scroll, bool y_scroll)
{
    m_xScrollingEnabled = x_scroll;
    m_yScrollingEnabled = y_scroll;
}

void wxToolBarSimple::GetVirtualSize (int *x, int *y) const
{
    *x = m_xScrollPixelsPerLine * m_xScrollLines;
    *y = m_yScrollPixelsPerLine * m_yScrollLines;
}

// Where the current view starts from
void wxToolBarSimple::ViewStart (int *x, int *y) const
{
    *x = m_xScrollPosition;
    *y = m_yScrollPosition;
}

#endif // wxUSE_TOOLBAR_SIMPLE
