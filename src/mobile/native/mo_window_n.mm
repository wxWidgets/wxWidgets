/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_window_n.mm
// Purpose:     wxMoWindow class
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

#include "wx/mobile/native/window.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoWindow, wxWindow)

BEGIN_EVENT_TABLE(wxMoWindow, wxWindow)
END_EVENT_TABLE()

void wxMoWindow::Init()
{
    // FIXME stub
}

bool wxMoWindow::Create(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    // FIXME stub

	return true;
}

wxMoWindow::~wxMoWindow()
{
}

void wxMoWindow::SetScrollbar( int orient, int pos, int thumbVisible,
                               int range, bool refresh)
{
    // FIXME stub
}

void wxMoWindow::SetScrollPos( int orient, int pos, bool refresh)
{
    // FIXME stub
}

int wxMoWindow::GetScrollPos( int orient ) const
{
	// FIXME stub
	return 0;
}

int wxMoWindow::GetScrollThumb( int orient ) const
{
	// FIXME stub
	return 0;
}

int wxMoWindow::GetScrollRange( int orient ) const
{
	// FIXME stub
	return 0;
}

bool wxMoWindow::ScrollLines(int WXUNUSED(lines))
{
	// FIXME stub
    // TODO
    return false;
}

bool wxMoWindow::ScrollPages(int WXUNUSED(pages))
{
	// FIXME stub
    // TODO
    return false;
}
