/////////////////////////////////////////////////////////////////////////////
// Name:        splittree.cpp
// Purpose:     Classes to achieve a remotely-scrolled tree in a splitter
//              window that can be scrolled by a scrolled window higher in the
//              hierarchy
// Author:      Julian Smart
// Modified by:
// Created:     8/7/2000
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#ifdef __WXMSW__
#include <windows.h>
#include "wx/msw/winundef.h"
#endif

#include "wx/gizmos/splittree.h"
#include "wx/renderer.h"
#include <math.h>

/*
 * wxRemotelyScrolledTreeCtrl
 */

#if USE_GENERIC_TREECTRL
IMPLEMENT_CLASS(wxRemotelyScrolledTreeCtrl, wxGenericTreeCtrl)
#else
IMPLEMENT_CLASS(wxRemotelyScrolledTreeCtrl, wxTreeCtrl)
#endif

#if USE_GENERIC_TREECTRL
BEGIN_EVENT_TABLE(wxRemotelyScrolledTreeCtrl, wxGenericTreeCtrl)
#else
BEGIN_EVENT_TABLE(wxRemotelyScrolledTreeCtrl, wxTreeCtrl)
#endif
    EVT_SIZE(wxRemotelyScrolledTreeCtrl::OnSize)
    EVT_PAINT(wxRemotelyScrolledTreeCtrl::OnPaint)
    EVT_TREE_ITEM_EXPANDED(wxID_ANY, wxRemotelyScrolledTreeCtrl::OnExpand)
    EVT_TREE_ITEM_COLLAPSED(wxID_ANY, wxRemotelyScrolledTreeCtrl::OnExpand)
    EVT_SCROLLWIN(wxRemotelyScrolledTreeCtrl::OnScroll)
END_EVENT_TABLE()

wxRemotelyScrolledTreeCtrl::wxRemotelyScrolledTreeCtrl(
    wxWindow* parent, wxWindowID id, const wxPoint& pt,
    const wxSize& sz, long style)
    : wxTreeCtrl(parent, id, pt, sz, style & ~wxTR_ROW_LINES)
{
    m_companionWindow = NULL;

    // We draw the row lines ourself so they match what's done
    // by the companion window.  That is why the flag is turned
    // off above, so wxGenericTreeCtrl doesn't draw them in a
    // different colour.
    m_drawRowLines = (style & wxTR_ROW_LINES) != 0;
}

wxRemotelyScrolledTreeCtrl::~wxRemotelyScrolledTreeCtrl()
{
}

void wxRemotelyScrolledTreeCtrl::HideVScrollbar()
{
#if defined(__WXMSW__)
#if USE_GENERIC_TREECTRL
    if (!IsKindOf(CLASSINFO(wxGenericTreeCtrl)))
#endif
    {
        ::ShowScrollBar((HWND) GetHWND(), SB_VERT, false);
    }
#if USE_GENERIC_TREECTRL
    else
    {
        // Implicit in overriding SetScrollbars
    }
#endif
#endif
}

void wxRemotelyScrolledTreeCtrl::DoCalcScrolledPosition(int x, int y, int *xx, int *yy) const
{
#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
    if (IsKindOf(CLASSINFO(wxGenericTreeCtrl)))
    {
        wxGenericTreeCtrl* win = (wxGenericTreeCtrl*) this;
        * yy = 0;
        int yyy;
        win->wxGenericTreeCtrl::DoCalcScrolledPosition(x, y, xx, & yyy);

        wxScrolledWindow* scrolledWindow = GetScrolledWindow();
        if (scrolledWindow)
        {
            int xxx;
            scrolledWindow->DoCalcScrolledPosition(x, y, & xxx, yy);
        }
    }
#endif
}

void wxRemotelyScrolledTreeCtrl::SetScrollbar(int orient,
                               int pos,
                               int thumbVisible,
                               int range,
                               bool update)
{
#ifndef __WXMSW__
    if (orient == wxVERTICAL)
        range = 0;
#endif
    wxWindow::SetScrollbar(orient, pos, thumbVisible, range, update);
}

// Number of pixels per user unit (0 or -1 for no scrollbar)
// Length of virtual canvas in user units
// Length of page in user units
void wxRemotelyScrolledTreeCtrl::SetScrollbars(
                                                #if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
                                                  int pixelsPerUnitX, int pixelsPerUnitY,
                             int noUnitsX, int noUnitsY,
                             int xPos, int yPos,
                                                  bool noRefresh
                                                #else
                                                  int WXUNUSED(pixelsPerUnitX), int WXUNUSED(pixelsPerUnitY),
                                                  int WXUNUSED(noUnitsX), int WXUNUSED(noUnitsY),
                                                  int WXUNUSED(xPos), int WXUNUSED(yPos),
                                                  bool WXUNUSED(noRefresh)
                                                #endif
                                              )
{
#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
    if (IsKindOf(CLASSINFO(wxGenericTreeCtrl)))
    {
        wxGenericTreeCtrl* win = (wxGenericTreeCtrl*) this;
        win->wxGenericTreeCtrl::SetScrollbars(pixelsPerUnitX, pixelsPerUnitY, noUnitsX, noUnitsY, xPos, yPos, /* noRefresh */ true);

        wxScrolledWindow* scrolledWindow = GetScrolledWindow();
        if (scrolledWindow)
        {
            scrolledWindow->SetScrollbars(0, pixelsPerUnitY, 0, noUnitsY, 0, yPos, noRefresh);
        }
    }
#endif
}

// In case we're using the generic tree control.
int wxRemotelyScrolledTreeCtrl::GetScrollPos(
                                             #if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
                                                 int orient
                                             #else
                                                 int WXUNUSED(orient)
                                             #endif
                                             ) const
{

#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
    // this condition fixes extsitence of warning but
    wxScrolledWindow* scrolledWindow =
    // but GetScrolledWindow is still executed in case internally does something
#endif
                                       GetScrolledWindow();

#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
    if (IsKindOf(CLASSINFO(wxGenericTreeCtrl)))
    {
        wxGenericTreeCtrl* win = (wxGenericTreeCtrl*) this;

        if (orient == wxHORIZONTAL)
            return win->wxGenericTreeCtrl::GetScrollPos(orient);
        else
        {
            return scrolledWindow->GetScrollPos(orient);
        }
    }
#endif
    return 0;
}


// In case we're using the generic tree control.
// Get the view start
void wxRemotelyScrolledTreeCtrl::GetViewStart(int *x, int *y) const
{
    wxScrolledWindow* scrolledWindow = GetScrolledWindow();

#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
    if (IsKindOf(CLASSINFO(wxGenericTreeCtrl)))
    {

        wxGenericTreeCtrl* win = (wxGenericTreeCtrl*) this;
        int x1, y1, x2, y2;
        win->wxGenericTreeCtrl::GetViewStart(& x1, & y1);
        * x = x1; * y = y1;
        if (!scrolledWindow)
            return;

        scrolledWindow->GetViewStart(& x2, & y2);
        * y = y2;
    }
    else
#endif
    {
        // x is wrong since the horizontal scrollbar is controlled by the
        // tree control, but we probably don't need it.
        scrolledWindow->GetViewStart(x, y);
    }
}

// In case we're using the generic tree control.
void wxRemotelyScrolledTreeCtrl::PrepareDC(
                                            #if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
                                                wxDC& dc
                                            #else
                                                wxDC& WXUNUSED(dc)
                                            #endif
                                           )
{
#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
    if (IsKindOf(CLASSINFO(wxGenericTreeCtrl)))
    {
        wxScrolledWindow* scrolledWindow = GetScrolledWindow();

        wxGenericTreeCtrl* win = (wxGenericTreeCtrl*) this;

        int startX, startY;
        GetViewStart(& startX, & startY);

        int xppu1, yppu1, xppu2, yppu2;
        win->wxGenericTreeCtrl::GetScrollPixelsPerUnit(& xppu1, & yppu1);
        scrolledWindow->GetScrollPixelsPerUnit(& xppu2, & yppu2);

        dc.SetDeviceOrigin( -startX * xppu1, -startY * yppu2 );
        // dc.SetUserScale( win->GetScaleX(), win->GetScaleY() );
    }
#endif
}

// Scroll to the given line (in scroll units where each unit is
// the height of an item)
void wxRemotelyScrolledTreeCtrl::ScrollToLine(int WXUNUSED(posHoriz), int posVert)
{
#ifdef __WXMSW__
#if USE_GENERIC_TREECTRL
    if (!IsKindOf(CLASSINFO(wxGenericTreeCtrl)))
#endif // USE_GENERIC_TREECTRL
    {
        // JLD - 2008-07-31 - call SetScrollInfo() - Vista ignores nPos passed in WM_VSCROLL wParam
        SCROLLINFO si = {sizeof(SCROLLINFO), SIF_POS, 0, 0, 0, posVert, 0};
        SetScrollInfo((HWND) GetHWND(), SB_VERT, &si, FALSE);

        UINT sbCode = SB_THUMBPOSITION;
        HWND vertScrollBar = 0;
        MSWDefWindowProc((WXUINT) WM_VSCROLL, MAKELONG(sbCode, posVert), (WXLPARAM) vertScrollBar);
    }
#if USE_GENERIC_TREECTRL
    else
#endif // USE_GENERIC_TREECTRL
#endif // __WXMSW__
#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
    {
        wxGenericTreeCtrl* win = (wxGenericTreeCtrl*) this;
        win->Refresh();
        /* Doesn't work yet because scrolling is ignored by Scroll
        int xppu, yppu;
        wxScrolledWindow* scrolledWindow = GetScrolledWindow();
        if (scrolledWindow)
        {
            scrolledWindow->GetScrollPixelsPerUnit(& xppu, & yppu);
            win->Scroll(-1, posVert*yppu);
        }
        */
    }
#endif // USE_GENERIC_TREECTRL || !defined(__WXMSW__)
    wxUnusedVar(posVert);
}

void wxRemotelyScrolledTreeCtrl::OnSize(wxSizeEvent& event)
{
    HideVScrollbar();
    AdjustRemoteScrollbars();
    event.Skip();
}

void wxRemotelyScrolledTreeCtrl::OnExpand(wxTreeEvent& event)
{
    AdjustRemoteScrollbars();
    event.Skip();

    // If we don't have this, we get some bits of lines still remaining
    if (event.GetEventType() == wxEVT_COMMAND_TREE_ITEM_COLLAPSED)
        Refresh();

    // Pass on the event
    if (m_companionWindow)
        m_companionWindow->GetEventHandler()->ProcessEvent(event);
}

void wxRemotelyScrolledTreeCtrl::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    wxTreeCtrl::OnPaint(event);

    if (! m_drawRowLines)
        return;

    // Reset the device origin since it may have been set
    dc.SetDeviceOrigin(0, 0);

    wxPen pen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT), 1, wxSOLID);
    dc.SetPen(pen);
    dc.SetBrush(* wxTRANSPARENT_BRUSH);

    wxSize clientSize = GetClientSize();
    wxRect itemRect;
    wxTreeItemId h, lastH;
    for (h=GetFirstVisibleItem();
         h.IsOk();
         h=GetNextVisible(h))
    {
        if (GetBoundingRect(h, itemRect))
        {
            int cy = itemRect.GetTop();
            dc.DrawLine(0, cy, clientSize.x, cy);
            lastH = h;
        }
        if (! IsVisible(h))
            break;
    }
    if (lastH.IsOk() && GetBoundingRect(lastH, itemRect))
    {
        int cy = itemRect.GetBottom();
        dc.DrawLine(0, cy, clientSize.x, cy);
    }
}


// Adjust the containing wxScrolledWindow's scrollbars appropriately
void wxRemotelyScrolledTreeCtrl::AdjustRemoteScrollbars()
{
#if USE_GENERIC_TREECTRL || !defined(__WXMSW__)
    if (IsKindOf(CLASSINFO(wxGenericTreeCtrl)))
    {
        // This is for the generic tree control.
        // It calls SetScrollbars which has been overridden
        // to adjust the parent scrolled window vertical
        // scrollbar.
        ((wxGenericTreeCtrl*) this)->AdjustMyScrollbars();
        return;
    }
    else
#endif
    {
        // This is for the wxMSW tree control
        wxScrolledWindow* scrolledWindow = GetScrolledWindow();
        if (scrolledWindow)
        {
            wxRect itemRect;
            if (GetBoundingRect(GetRootItem(), itemRect))
            {
                // Actually, the real height seems to be 1 less than reported
                // (e.g. 16 instead of 16)
                int itemHeight = itemRect.GetHeight() - 1;

                int w, h;
                GetClientSize(&w, &h);

                wxRect rect(0, 0, 0, 0);
                CalcTreeSize(rect);

                double f = ((double) (rect.GetHeight()) / (double) itemHeight)  ;
                int treeViewHeight = (int) ceil(f);

                int scrollPixelsPerLine = itemHeight;
                int scrollPos = - (itemRect.y / itemHeight);

                scrolledWindow->SetScrollbars(0, scrollPixelsPerLine, 0, treeViewHeight, 0, scrollPos);

                // Ensure that when a scrollbar becomes hidden or visible,
                // the contained window sizes are right.
                // Problem: this is called too early (?)
                wxSizeEvent event(scrolledWindow->GetSize(), scrolledWindow->GetId());
                scrolledWindow->GetEventHandler()->ProcessEvent(event);
            }
        }
    }
}


// Calculate the area that contains both rectangles
static wxRect CombineRectangles(const wxRect& rect1, const wxRect& rect2)
{
    wxRect rect;

    int right1 = rect1.GetRight();
    int bottom1 = rect1.GetBottom();
    int right2 = rect2.GetRight();
    int bottom2 = rect2.GetBottom();

    wxPoint topLeft = wxPoint(wxMin(rect1.x, rect2.x), wxMin(rect1.y, rect2.y));
    wxPoint bottomRight = wxPoint(wxMax(right1, right2), wxMax(bottom1, bottom2));

    rect.x = topLeft.x; rect.y = topLeft.y;
    rect.SetRight(bottomRight.x);
    rect.SetBottom(bottomRight.y);

    return rect;
}


// Calculate the tree overall size so we can set the scrollbar
// correctly
void wxRemotelyScrolledTreeCtrl::CalcTreeSize(wxRect& rect)
{
    CalcTreeSize(GetRootItem(), rect);
}

void wxRemotelyScrolledTreeCtrl::CalcTreeSize(const wxTreeItemId& id, wxRect& rect)
{
    // More efficient implementation would be to find the last item (but how?)
    // Q: is the bounding rect relative to the top of the virtual tree workspace
    // or the top of the window? How would we convert?
    wxRect itemSize;
    if (GetBoundingRect(id, itemSize))
    {
        rect = CombineRectangles(rect, itemSize);
    }

    wxTreeItemIdValue cookie;
    wxTreeItemId childId = GetFirstChild(id, cookie);
    while (childId)
    {
        CalcTreeSize(childId, rect);
        childId = GetNextChild(childId, cookie);
    }
}

// Find the scrolled window that contains this control
wxScrolledWindow* wxRemotelyScrolledTreeCtrl::GetScrolledWindow() const
{
    wxWindow* parent = wxWindow::GetParent();
    while (parent)
    {
        if (parent->IsKindOf(CLASSINFO(wxScrolledWindow)))
            return (wxScrolledWindow*) parent;
        parent = parent->GetParent();
    }
    return NULL;
}

void wxRemotelyScrolledTreeCtrl::OnScroll(wxScrollWinEvent& event)
{
    int orient = event.GetOrientation();
    if (orient == wxHORIZONTAL)
    {
        event.Skip();
        return;
    }
    wxScrolledWindow* scrollWin = GetScrolledWindow();
    if (!scrollWin)
        return;

    int x, y;
    scrollWin->GetViewStart(& x, & y);

    ScrollToLine(-1, y);

#ifndef __WXMSW__
    m_yScrollPosition = GetScrollPos(wxVERTICAL);
#endif
}

/*
 * wxTreeCompanionWindow
 *
 * A window displaying values associated with tree control items.
 */

IMPLEMENT_CLASS(wxTreeCompanionWindow, wxWindow)

BEGIN_EVENT_TABLE(wxTreeCompanionWindow, wxWindow)
    EVT_PAINT(wxTreeCompanionWindow::OnPaint)
    EVT_SCROLLWIN(wxTreeCompanionWindow::OnScroll)
    EVT_TREE_ITEM_EXPANDED(-1, wxTreeCompanionWindow::OnExpand)
    EVT_TREE_ITEM_COLLAPSED(-1, wxTreeCompanionWindow::OnExpand)
END_EVENT_TABLE()

wxTreeCompanionWindow::wxTreeCompanionWindow(wxWindow* parent, wxWindowID id,
      const wxPoint& pos,
      const wxSize& sz,
      long style):
    wxWindow(parent, id, pos, sz, style)
{
    m_treeCtrl = NULL;
}

void wxTreeCompanionWindow::DrawItem(wxDC& dc, wxTreeItemId id, const wxRect& rect)
{
    // TEST CODE
#if 1
    if (m_treeCtrl)
    {
        wxString text = m_treeCtrl->GetItemText(id);
        dc.SetTextForeground(* wxBLACK);
        dc.SetBackgroundMode(wxTRANSPARENT);

        int textW, textH;
        dc.GetTextExtent(text, & textW, & textH);

        int x = 5;
        int y = rect.GetY() + wxMax(0, (rect.GetHeight() - textH) / 2);

        dc.DrawText(text, x, y);
    }
#endif
}

void wxTreeCompanionWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    if (!m_treeCtrl)
        return;

        wxPen pen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT), 1, wxSOLID);
    dc.SetPen(pen);
    dc.SetBrush(* wxTRANSPARENT_BRUSH);
    wxFont font(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    dc.SetFont(font);

    wxSize clientSize = GetClientSize();
    wxRect itemRect;
    wxTreeItemId h, lastH;
    for (h=m_treeCtrl->GetFirstVisibleItem();
         h.IsOk();
         h=m_treeCtrl->GetNextVisible(h))
    {
        if (m_treeCtrl->GetBoundingRect(h, itemRect))
        {
            int cy = itemRect.GetTop();
            wxRect drawItemRect(0, cy, clientSize.x, itemRect.GetHeight());

            lastH = h;

            // Draw the actual item
            DrawItem(dc, h, drawItemRect);
            dc.DrawLine(0, cy, clientSize.x, cy);
        }
        if (! m_treeCtrl->IsVisible(h))
            break;
    }
    if (lastH.IsOk() && m_treeCtrl->GetBoundingRect(lastH, itemRect))
    {
        int cy = itemRect.GetBottom();
        dc.DrawLine(0, cy, clientSize.x, cy);
    }
}

void wxTreeCompanionWindow::OnScroll(wxScrollWinEvent& event)
{
    int orient = event.GetOrientation();
    if (orient == wxHORIZONTAL)
    {
        event.Skip();
        return;
    }
    if (!m_treeCtrl)
        return;

    // TODO: scroll the window physically instead of just refreshing.
    Refresh(true);
}

void wxTreeCompanionWindow::OnExpand(wxTreeEvent& WXUNUSED(event))
{
    // TODO: something more optimized than simply refresh the whole
    // window when the tree is expanded/collapsed. Tricky.
    Refresh();
}

/*
 * wxThinSplitterWindow
 */

IMPLEMENT_CLASS(wxThinSplitterWindow, wxSplitterWindow)

BEGIN_EVENT_TABLE(wxThinSplitterWindow, wxSplitterWindow)
    EVT_SIZE(wxThinSplitterWindow::OnSize)
END_EVENT_TABLE()

wxThinSplitterWindow::wxThinSplitterWindow(wxWindow* parent, wxWindowID id,
      const wxPoint& pos,
      const wxSize& sz,
      long style):
      wxSplitterWindow(parent, id, pos, sz, style)
{
    wxColour faceColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    m_facePen = new wxPen(faceColour, 1, wxSOLID);
    m_faceBrush = new wxBrush(faceColour, wxSOLID);
}

wxThinSplitterWindow::~wxThinSplitterWindow()
{
    delete m_facePen;
    delete m_faceBrush;
}


void wxThinSplitterWindow::SizeWindows()
{
    // The client size may have changed inbetween
    // the sizing of the first window and the sizing of
    // the second. So repeat SizeWindows.
    wxSplitterWindow::SizeWindows();
    wxSplitterWindow::SizeWindows();
}

// Tests for x, y over sash
bool wxThinSplitterWindow::SashHitTest(int x, int y, int WXUNUSED(tolerance))
{
    return wxSplitterWindow::SashHitTest(x, y, 4);
}

void wxThinSplitterWindow::DrawSash(wxDC& dc)
{
    wxRendererNative::Get().DrawSplitterBorder
                           (
                               this,
                               dc,
                               GetClientRect()
                           );
    if ( m_sashPosition == 0 || !m_windowTwo)
        return;
    if (GetWindowStyle() & wxSP_NOSASH)
        return;

    int w, h;
    GetClientSize(&w, &h);

    if ( m_splitMode == wxSPLIT_VERTICAL )
    {
        dc.SetPen(* m_facePen);
        dc.SetBrush(* m_faceBrush);
        int h1 = h-1;
        int y1 = 0;
        if ( (GetWindowStyleFlag() & wxSP_BORDER) != wxSP_BORDER && (GetWindowStyleFlag() & wxSP_3DBORDER) != wxSP_3DBORDER )
            h1 += 1; // Not sure why this is necessary...
        if ( (GetWindowStyleFlag() & wxSP_3DBORDER) == wxSP_3DBORDER)
        {
            y1 = 2; h1 -= 3;
        }
        dc.DrawRectangle(m_sashPosition, y1, GetSashSize(), h1);
    }
    else
    {
        dc.SetPen(* m_facePen);
        dc.SetBrush(* m_faceBrush);
        int w1 = w-1;
        int x1 = 0;
        if ( (GetWindowStyleFlag() & wxSP_BORDER) != wxSP_BORDER && (GetWindowStyleFlag() & wxSP_3DBORDER) != wxSP_3DBORDER )
            w1 ++;
        if ( (GetWindowStyleFlag() & wxSP_3DBORDER) == wxSP_3DBORDER)
        {
            x1 = 2; w1 -= 3;
        }
        dc.DrawRectangle(x1, m_sashPosition, w1, GetSashSize());
    }

    dc.SetPen(wxNullPen);
    dc.SetBrush(wxNullBrush);
}

void wxThinSplitterWindow::OnSize(wxSizeEvent& event)
{
    wxSplitterWindow::OnSize(event);
}

/*
 * wxSplitterScrolledWindow
 */

IMPLEMENT_CLASS(wxSplitterScrolledWindow, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxSplitterScrolledWindow, wxScrolledWindow)
    EVT_SCROLLWIN(wxSplitterScrolledWindow::OnScroll)
    EVT_SIZE(wxSplitterScrolledWindow::OnSize)
END_EVENT_TABLE()

wxSplitterScrolledWindow::wxSplitterScrolledWindow(wxWindow* parent, wxWindowID id,
      const wxPoint& pos,
      const wxSize& sz,
      long style):
      wxScrolledWindow(parent, id, pos, sz, style)
{
}

void wxSplitterScrolledWindow::OnSize(wxSizeEvent& WXUNUSED(event))
{
    wxSize sz = GetClientSize();
    if (GetChildren().GetFirst())
    {
        ((wxWindow*) GetChildren().GetFirst()->GetData())->SetSize(0, 0, sz.x, sz.y);
    }
}

void wxSplitterScrolledWindow::OnScroll(wxScrollWinEvent& event)
{
    // Ensure that events being propagated back up the window hierarchy
    // don't cause an infinite loop
    static bool inOnScroll = false;
    if (inOnScroll)
    {
        event.Skip();
        return;
    }
    inOnScroll = true;

    int orient = event.GetOrientation();

    int nScrollInc = CalcScrollInc(event);
    if (nScrollInc == 0)
    {
        inOnScroll = false;
        return;
    }

    if (orient == wxHORIZONTAL)
    {
        inOnScroll = false;
        event.Skip();
        return;
#if 0
        int newPos = m_xScrollPosition + nScrollInc;
        SetScrollPos(wxHORIZONTAL, newPos, true );
#endif
    }
    else
    {
        int newPos = m_yScrollPosition + nScrollInc;
        SetScrollPos(wxVERTICAL, newPos, true );
    }

    if (orient == wxHORIZONTAL)
    {
        m_xScrollPosition += nScrollInc;
    }
    else
    {
        m_yScrollPosition += nScrollInc;
    }

    // Find targets in splitter window and send the event to them
    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
    while (node)
    {
        wxWindow* child = (wxWindow*) node->GetData();
        if (child->IsKindOf(CLASSINFO(wxSplitterWindow)))
        {
            wxSplitterWindow* splitter = (wxSplitterWindow*) child;
            if (splitter->GetWindow1())
                splitter->GetWindow1()->ProcessEvent(event);
            if (splitter->GetWindow2())
                splitter->GetWindow2()->ProcessEvent(event);
            break;
        }
        node = node->GetNext();
    }

    m_targetWindow->Update() ;

    inOnScroll = false;
}
