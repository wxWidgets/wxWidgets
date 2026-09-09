///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/control.cpp
// Purpose:     Extension of wxControl with common ribbon methods
// Author:      Peter Cawley
// Created:     2009-06-05
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_RIBBON

#include "wx/ribbon/control.h"
#include "wx/ribbon/bar.h"

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

wxIMPLEMENT_CLASS(wxRibbonControl, wxControl);

wxBitmap wxRibbonControl::MakeDisabledBitmap(const wxBitmap& original)
{
    wxImage img(original.ConvertToImage());
    return wxBitmap(img.ConvertToGreyscale(), -1, original.GetScaleFactor());
}

wxBitmapBundle wxRibbonControl::MakeDisabledBundle(const wxBitmapBundle& bundle)
{
    wxSize sizeDef = bundle.GetDefaultSize();
    wxVector<wxBitmap> bitmaps;
    bitmaps.reserve(2);
    wxBitmap bmp1 = bundle.GetBitmap(sizeDef);
    bitmaps.push_back(MakeDisabledBitmap(bmp1));
    wxSize size2x = sizeDef * 2;
    wxBitmap bmp2 = bundle.GetBitmap(size2x);
    if ( bmp2.GetSize() != bmp1.GetSize() )
        bitmaps.push_back(MakeDisabledBitmap(bmp2));
    return wxBitmapBundle::FromBitmaps(bitmaps);
}

bool wxRibbonControl::Create(wxWindow *parent, wxWindowID id,
                    const wxPoint& pos,
                    const wxSize& size, long style,
                    const wxValidator& validator,
                    const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    wxRibbonControl *ribbon_parent = wxDynamicCast(parent, wxRibbonControl);
    if(ribbon_parent)
    {
        m_art = ribbon_parent->GetArtProvider();
    }

    return true;
}

void wxRibbonControl::SetArtProvider(wxRibbonArtProvider* art)
{
    m_art = art;
}

wxSize wxRibbonControl::DoGetNextSmallerSize(wxOrientation direction,
                                           wxSize size) const
{
    // Dummy implementation for code which doesn't check for IsSizingContinuous() == true
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

wxSize wxRibbonControl::DoGetNextLargerSize(wxOrientation direction,
                                          wxSize size) const
{
    // Dummy implementation for code which doesn't check for IsSizingContinuous() == true
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

wxSize wxRibbonControl::GetNextSmallerSize(wxOrientation direction,
                                           wxSize relative_to) const
{
    return DoGetNextSmallerSize(direction, relative_to);
}

wxSize wxRibbonControl::GetNextLargerSize(wxOrientation direction,
                                          wxSize relative_to) const
{
    return DoGetNextLargerSize(direction, relative_to);
}

wxSize wxRibbonControl::GetNextSmallerSize(wxOrientation direction) const
{
    return DoGetNextSmallerSize(direction, GetSize());
}

wxSize wxRibbonControl::GetNextLargerSize(wxOrientation direction) const
{
    return DoGetNextLargerSize(direction, GetSize());
}

bool wxRibbonControl::Realize()
{
    return true;
}

wxRibbonBar* wxRibbonControl::GetAncestorRibbonBar()const
{
    for ( wxWindow* win = GetParent(); win; win = win->GetParent() )
    {
        wxRibbonBar* bar = wxDynamicCast(win, wxRibbonBar);
        if ( bar )
            return bar;
    }

    return nullptr;
}

void wxRibbonControl::DismissKeyTips()
{
    if ( wxRibbonBar* bar = GetAncestorRibbonBar() )
        bar->HideKeyTips();
}

#endif // wxUSE_RIBBON
