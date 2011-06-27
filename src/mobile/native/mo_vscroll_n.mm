/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_vscroll_n.mm
// Purpose:     wxMoVScrolledWindow class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/frame.h"
#include "wx/popupwin.h"

#include "wx/mobile/native/vscroll.h"

IMPLEMENT_CLASS(wxMoVScrolledWindow, wxVScrolledWindow)

BEGIN_EVENT_TABLE(wxMoVScrolledWindow, wxVScrolledWindow)
END_EVENT_TABLE()

wxMoVScrolledWindow::wxMoVScrolledWindow(): m_touchScrollHelper(this)
{
    // FIXME stub
}

void wxMoVScrolledWindow::Init()
{
    // FIXME stub
}

bool wxMoVScrolledWindow::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    // FIXME stub

    return true;
}

wxMoVScrolledWindow::~wxMoVScrolledWindow()
{
}

void wxMoVScrolledWindow::SetScrollbars(int pixelsPerUnitX,
                                        int pixelsPerUnitY,
                                        int noUnitsX,
                                        int noUnitsY,
                                        int xPos,
                                        int yPos,
                                        bool noRefresh)
{
    // Do nothing
}

int wxMoVScrolledWindow::GetAverageLineHeight() const
{
    // FIXME stub
    return 0;
}

int wxMoVScrolledWindow::GetEstimatedTotalHeight() const
{
    // FIXME stub
    return 0;
}

IMPLEMENT_CLASS(wxMoTouchVScrollHelper, wxMoTouchScrollHelperBase)

bool wxMoTouchVScrollHelper::GetScrollBarDetails(int orientation,
        int* range, int* thumbPosition, int* thumbSize)
{
    // FIXME stub

    return true;
}



// Does the scrolling for the specific scrolled window type.
bool wxMoTouchVScrollHelper::DoScrolling(const wxPoint& WXUNUSED(lastPos), const wxPoint& newPos)
{
    // FIXME stub
    
    return true;
}

// Override to do processing on start pos
void wxMoTouchVScrollHelper::OnStartScrolling(const wxPoint& WXUNUSED(startPos))
{
    // FIXME stub
}
