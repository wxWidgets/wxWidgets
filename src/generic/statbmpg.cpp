///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/statbmpg.cpp
// Purpose:     wxGenericStaticBitmap
// Author:      Marcin Wojdyr, Stefan Csomor
// Created:     2008-06-16
// Copyright:   wxWidgets developers
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATBMP

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
#endif

#include "wx/generic/statbmpg.h"

#if wxUSE_GRAPHICS_CONTEXT
    #include "wx/graphics.h"
    #include "wx/scopedptr.h"
#else
    #include "wx/image.h"
    #include "wx/math.h"
#endif

bool wxGenericStaticBitmap::Create(wxWindow *parent, wxWindowID id,
                                   const wxBitmap& bitmap,
                                   const wxPoint& pos, const wxSize& size,
                                   long style, const wxString& name)
{
    if (! wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name))
        return false;
    m_scaleMode = Scale_None;
    SetBitmap(bitmap);
    Bind(wxEVT_PAINT, &wxGenericStaticBitmap::OnPaint, this);
    return true;
}

void wxGenericStaticBitmap::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    if ( !m_bitmap.IsOk() )
        return;

    wxPaintDC dc(this);
    const wxSize drawSize = GetClientSize();
    const wxSize bmpSize = m_bitmap.GetSize();
    wxDouble w = 0;
    wxDouble h = 0;
    switch ( m_scaleMode )
    {
        case Scale_None:
            dc.DrawBitmap(m_bitmap, 0, 0, true);
            return;
        case Scale_Fill:
            w = drawSize.x;
            h = drawSize.y;
            break;
        case Scale_AspectFill:
        case Scale_AspectFit:
        {
            wxDouble scaleFactor;
            wxDouble scaleX = (wxDouble)drawSize.x / (wxDouble)bmpSize.x;
            wxDouble scaleY = (wxDouble)drawSize.y / (wxDouble)bmpSize.y;
            if ( ( m_scaleMode == Scale_AspectFit && scaleY < scaleX ) ||
                 ( m_scaleMode == Scale_AspectFill && scaleY > scaleX ) )
                scaleFactor = scaleY;
            else
                scaleFactor = scaleX;

            w = bmpSize.x * scaleFactor;
            h = bmpSize.y * scaleFactor;

            break;
        }
    }

    wxASSERT_MSG(w, wxS("Unknown scale mode"));

    wxDouble x = (drawSize.x - w) / 2;
    wxDouble y = (drawSize.y - h) / 2;
#if wxUSE_GRAPHICS_CONTEXT
    wxScopedPtr<wxGraphicsContext> const
        gc(wxGraphicsRenderer::GetDefaultRenderer()->CreateContext(dc));
    gc->DrawBitmap(m_bitmap, x, y, w, h);
#else
    wxImage img = m_bitmap.ConvertToImage();
    img.Rescale(wxRound(w), wxRound(h), wxIMAGE_QUALITY_HIGH);
    dc.DrawBitmap(wxBitmap(img), wxRound(x), wxRound(y), true);
#endif
}

// under OSX_cocoa is a define, avoid duplicate info
#ifndef wxGenericStaticBitmap

wxIMPLEMENT_DYNAMIC_CLASS(wxGenericStaticBitmap, wxStaticBitmapBase);

#endif

#endif // wxUSE_STATBMP

