/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_vscroll_g.cpp
// Purpose:     wxMoVScrolledWindow class
// Author:      Julian Smart
// Modified by:
// Created:     2009-06-07
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/frame.h"
#include "wx/popupwin.h"

#include "wx/mobile/generic/vscroll.h"

IMPLEMENT_CLASS(wxMoVScrolledWindow, wxVScrolledWindow)

BEGIN_EVENT_TABLE(wxMoVScrolledWindow, wxVScrolledWindow)
END_EVENT_TABLE()

wxMoVScrolledWindow::wxMoVScrolledWindow(): m_touchScrollHelper(this)
{
    Init();
}

void wxMoVScrolledWindow::Init()
{
    m_showHorizontalScrollIndicator = true;
    m_showVerticalScrollIndicator = true;
}

bool wxMoVScrolledWindow::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    bool ok = wxVScrolledWindow::Create(parent, id, pos, size, style/*|wxHSCROLL|wxVSCROLL*/, name);
    
    wxWindow::SetScrollbar(wxHORIZONTAL, 0, 0, 0, false);
    wxWindow::SetScrollbar(wxVERTICAL, 0, 0, 0, false);

    return ok;
}

wxMoVScrolledWindow::~wxMoVScrolledWindow()
{
}

int wxMoVScrolledWindow::GetAverageLineHeight() const
{
    if (GetLineCount() == 0)
        return 20;
    else
        return OnGetLineHeight(0);
}

int wxMoVScrolledWindow::GetEstimatedTotalHeight() const
{
    return EstimateTotalHeight();
}

IMPLEMENT_CLASS(wxMoTouchVScrollHelper, wxMoTouchScrollHelperBase)

bool wxMoTouchVScrollHelper::GetScrollBarDetails(int orientation,
        int* range, int* thumbPosition, int* thumbSize)
{
    wxMoVScrolledWindow* scrolledWindow = wxDynamicCast(m_scrolledWindow, wxMoVScrolledWindow);
    if (scrolledWindow)
    {        
        if (orientation == wxHORIZONTAL)
        {
            return false;
        }
        else if (orientation == wxVERTICAL)
        {
            if (!scrolledWindow->GetShowVerticalScrollIndicator())
                return false;

            int height = wxMax(m_scrolledWindow->GetSize().y, scrolledWindow->GetEstimatedTotalHeight());
            if (height == 0)
                height = 10;

            * range = scrolledWindow->GetLineCount();
            * thumbPosition = scrolledWindow->GetVisibleBegin();
            * thumbSize = m_scrolledWindow->GetSize().y/height;

            return true;
        }
    }
    return false;
}

// Does the scrolling for the specific scrolled window type.
bool wxMoTouchVScrollHelper::DoScrolling(const wxPoint& WXUNUSED(lastPos), const wxPoint& newPos)
{
    wxMoVScrolledWindow* scrolledWindow = wxDynamicCast(m_scrolledWindow, wxMoVScrolledWindow);
    if (scrolledWindow)
    {
        if (scrolledWindow)
        {
            int averageHeight = scrolledWindow->GetAverageLineHeight();
            int lineOffset = (m_startPos.y - newPos.y) / averageHeight;
            int newLine = wxMax(0, lineOffset - m_firstVisibleLine);

            scrolledWindow->ScrollToLine(newLine);
            scrolledWindow->Update();
        }
    }
    
    return true;
}

// Override to do processing on start pos
void wxMoTouchVScrollHelper::OnStartScrolling(const wxPoint& WXUNUSED(startPos))
{
    wxMoVScrolledWindow* scrolledWindow = wxDynamicCast(m_scrolledWindow, wxMoVScrolledWindow);
    if (scrolledWindow)
    {
        m_firstVisibleLine = scrolledWindow->GetFirstVisibleLine();
    }
}
