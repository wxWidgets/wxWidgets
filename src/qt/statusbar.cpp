/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/statusbar.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statusbr.h"

wxStatusBar::wxStatusBar()
{
}

wxStatusBar::wxStatusBar(wxWindow *parent, wxWindowID winid,
            long style,
            const wxString& name)
{
}


void wxStatusBar::Init()
{
}

bool wxStatusBar::Create(wxWindow *parent, wxWindowID winid,
            long style,
            const wxString& name)
{
    return false;
}

bool wxStatusBar::GetFieldRect(int i, wxRect& rect) const
{
    return false;
}

void wxStatusBar::SetMinHeight(int height)
{
}

int wxStatusBar::GetBorderX() const
{
    return 0;
}

int wxStatusBar::GetBorderY() const
{
    return 0;
}

void wxStatusBar::DoUpdateStatusText(int number)
{
}

