/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_window_g.cpp
// Purpose:     wxMoWindow class
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

#include "wx/mobile/generic/window.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoWindow, wxWindow)

BEGIN_EVENT_TABLE(wxMoWindow, wxWindow)
END_EVENT_TABLE()

void wxMoWindow::Init()
{
    m_scrollPosX = 0;
    m_scrollPosY = 0;
    m_scrollThumbSizeX = 0;
    m_scrollThumbSizeY = 0;
    m_scrollRangeX = 0;
    m_scrollRangeY = 0;
}

bool wxMoWindow::Create(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return false;

    return true;
}

wxMoWindow::~wxMoWindow()
{
}

void wxMoWindow::SetScrollbar( int orient, int pos, int thumbVisible,
                               int range, bool refresh)
{
    if (orient == wxHORIZONTAL)
    {
        m_scrollPosX = pos;
        m_scrollThumbSizeX = thumbVisible;
        m_scrollRangeX = range;
    }
    else
    {
        m_scrollPosY = pos;
        m_scrollThumbSizeY = thumbVisible;
        m_scrollRangeY = range;
    }

    if (refresh)
        Refresh();
}

void wxMoWindow::SetScrollPos( int orient, int pos, bool refresh)
{
    if (orient == wxHORIZONTAL)
        m_scrollPosX = pos;
    else
        m_scrollPosY = pos;

    if (refresh)
        Refresh();
}

int wxMoWindow::GetScrollPos( int orient ) const
{
    return orient == wxHORIZONTAL ? m_scrollPosX : m_scrollPosY;
}

int wxMoWindow::GetScrollThumb( int orient ) const
{
    return orient == wxHORIZONTAL ? m_scrollThumbSizeX : m_scrollThumbSizeY;
}

int wxMoWindow::GetScrollRange( int orient ) const
{
    return orient == wxHORIZONTAL ? m_scrollRangeX : m_scrollRangeY;
}

bool wxMoWindow::ScrollLines(int WXUNUSED(lines))
{
    // TODO
    return false;
}

bool wxMoWindow::ScrollPages(int WXUNUSED(pages))
{
    // TODO
    return false;
}
