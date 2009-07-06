///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/control.cpp
// Purpose:     Extension of wxControl with common ribbon methods
// Author:      Peter Cawley
// Modified by:
// Created:     2009-06-05
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/ribbon/control.h"

#if wxUSE_RIBBON

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

IMPLEMENT_CLASS(wxRibbonControl, wxControl)

wxRibbonControl::wxRibbonControl(wxWindow *parent, wxWindowID id,
                    const wxPoint& pos,
                    const wxSize& size, long style,
                    const wxValidator& validator,
                    const wxString& name)
    : wxControl(parent, id, pos, size, style, validator, name)
{
    m_art = NULL;

    wxRibbonControl *ribbon_parent = wxDynamicCast(parent, wxRibbonControl);
    if(ribbon_parent)
    {
        m_art = ribbon_parent->GetArtProvider();
    }
}

void wxRibbonControl::SetArtProvider(wxRibbonArtProvider* art)
{
    m_art = art;
}

wxSize wxRibbonControl::GetNextSmallerSize(wxOrientation direction) const
{
    // Dummy implementation for code which doesn't check for IsSizingContinuous() == true
    wxSize size(GetSize());
    wxSize minimum(GetMinSize());
    if((direction & wxHORIZONTAL) && size.x > minimum.x)
    {
        size.x--;
    }
    if((direction & wxVERTICAL) && size.y > minimum.y)
    {
        size.y--;
    }
    return size;
}

wxSize wxRibbonControl::GetNextLargerSize(wxOrientation direction) const
{
    // Dummy implementation for code which doesn't check for IsSizingContinuous() == true
    wxSize size(GetSize());
    if(direction & wxHORIZONTAL)
    {
        size.x++;
    }
    if(direction & wxVERTICAL)
    {
        size.y++;
    }
    return size;
}

bool wxRibbonControl::Realize()
{
    return true;
}

#endif // wxUSE_RIBBON
