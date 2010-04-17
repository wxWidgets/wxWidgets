/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/scrolbar.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/scrolbar.h"

IMPLEMENT_DYNAMIC_CLASS( wxScrollBar, wxScrollBarBase )

wxScrollBar::wxScrollBar()
{
}

wxScrollBar::wxScrollBar( wxWindow *parent, wxWindowID id,
       const wxPoint& pos,
       const wxSize& size,
       long style,
       const wxValidator& validator,
       const wxString& name)
{
}

bool wxScrollBar::Create( wxWindow *parent, wxWindowID id,
       const wxPoint& pos,
       const wxSize& size,
       long style,
       const wxValidator& validator,
       const wxString& name)
{
    return false;
}

int wxScrollBar::GetThumbPosition() const
{
    return 0;
}

int wxScrollBar::GetThumbSize() const
{
    return 0;
}

int wxScrollBar::GetPageSize() const
{
    return 0;
}

int wxScrollBar::GetRange() const
{
    return 0;
}

void wxScrollBar::SetThumbPosition(int viewStart)
{
}

void wxScrollBar::SetScrollbar(int position, int thumbSize,
                          int range, int pageSize,
                          bool refresh)
{
}

