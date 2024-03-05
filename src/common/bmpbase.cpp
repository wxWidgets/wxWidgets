/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/bmpbase.cpp
// Purpose:     wxBitmapBase
// Author:      VaclavSlavik
// Created:     2001/04/11
// Copyright:   (c) 2001, Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/bitmap.h"

#ifndef WX_PRECOMP
    #include "wx/colour.h"
    #include "wx/icon.h"
    #include "wx/image.h"
#endif // WX_PRECOMP

#if wxUSE_IMAGE && wxUSE_LIBPNG && wxUSE_STREAMS
    #define wxHAS_PNG_LOAD

    #include "wx/mstream.h"
#endif

extern bool wxDumpBitmap(const wxBitmap& bitmap, const char* path)
{
    return bitmap.SaveFile(path, wxBITMAP_TYPE_BMP);
}

// ----------------------------------------------------------------------------
// wxVariant support
// ----------------------------------------------------------------------------

#if wxUSE_VARIANT
IMPLEMENT_VARIANT_OBJECT_EXPORTED_SHALLOWCMP(wxBitmap,WXDLLEXPORT)
IMPLEMENT_VARIANT_OBJECT_EXPORTED_SHALLOWCMP(wxIcon,WXDLLEXPORT)
#endif

#if wxUSE_EXTENDED_RTTI
//WX_IMPLEMENT_ANY_VALUE_TYPE(wxAnyValueTypeImpl<wxBitmap>)
//WX_IMPLEMENT_ANY_VALUE_TYPE(wxAnyValueTypeImpl<wxIcon>)
#endif

// ----------------------------------------------------------------------------
// wxBitmapHelpers
// ----------------------------------------------------------------------------

// wxOSX has a native version and doesn't use this one.

#ifndef __WXOSX__

/* static */
wxBitmap wxBitmapHelpers::NewFromPNGData(const void* data, size_t size)
{
    wxBitmap bitmap;

#ifdef wxHAS_PNG_LOAD
    wxMemoryInputStream is(data, size);
    wxImage image(is, wxBITMAP_TYPE_PNG);
    if ( image.IsOk() )
        bitmap = wxBitmap(image);
#endif // wxHAS_PNG_LOAD

    return bitmap;
}

#endif // !__WXOSX__

/* static */
void wxBitmapHelpers::Rescale(wxBitmap& bmp, const wxSize& sizeNeeded)
{
    wxCHECK_RET( sizeNeeded.IsFullySpecified(), wxS("New size must be given") );

#if wxUSE_IMAGE
    // Note that we use "nearest" rescale mode here to preserve sharp edges in
    // the icons for which this function is often used. It's also consistent
    // with what wxDC::DrawBitmap() does, i.e. the fallback method below.
    wxImage img = bmp.ConvertToImage();
    img.Rescale(sizeNeeded.x, sizeNeeded.y, wxIMAGE_QUALITY_NEAREST);
    bmp = wxBitmap(img);
#else // !wxUSE_IMAGE
    // Fallback method of scaling the bitmap
    wxBitmap newBmp(sizeNeeded, bmp.GetDepth());
#if defined(__WXMSW__) || defined(__WXOSX__)
    // wxBitmap::UseAlpha() is used only on wxMSW and wxOSX.
    newBmp.UseAlpha(bmp.HasAlpha());
#endif // __WXMSW__ || __WXOSX__
    {
        wxMemoryDC dc(newBmp);
        double scX = (double)sizeNeeded.GetWidth() / bmp.GetWidth();
        double scY = (double)sizeNeeded.GetHeight() / bmp.GetHeight();
        dc.SetUserScale(scX, scY);
        dc.DrawBitmap(bmp, 0, 0);
    }
    bmp = newBmp;
#endif // wxUSE_IMAGE/!wxUSE_IMAGE
}

// ----------------------------------------------------------------------------
// wxBitmapBase
// ----------------------------------------------------------------------------

#if wxUSE_BITMAP_BASE

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/palette.h"
    #include "wx/module.h"
#endif // WX_PRECOMP


wxIMPLEMENT_ABSTRACT_CLASS(wxBitmapBase, wxGDIObject);
wxIMPLEMENT_ABSTRACT_CLASS(wxBitmapHandler, wxObject);

wxList wxBitmapBase::sm_handlers;

void wxBitmapBase::AddHandler(wxBitmapHandler *handler)
{
    sm_handlers.Append(handler);
}

void wxBitmapBase::InsertHandler(wxBitmapHandler *handler)
{
    sm_handlers.Insert(handler);
}

bool wxBitmapBase::RemoveHandler(const wxString& name)
{
    wxBitmapHandler *handler = FindHandler(name);
    if ( handler )
    {
        sm_handlers.DeleteObject(handler);
        return true;
    }
    else
        return false;
}

wxBitmapHandler *wxBitmapBase::FindHandler(const wxString& name)
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->GetData();
        if ( handler->GetName() == name )
            return handler;
        node = node->GetNext();
    }
    return nullptr;
}

wxBitmapHandler *wxBitmapBase::FindHandler(const wxString& extension, wxBitmapType bitmapType)
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->GetData();
        if ( handler->GetExtension() == extension &&
                    (bitmapType == wxBITMAP_TYPE_ANY || handler->GetType() == bitmapType) )
            return handler;
        node = node->GetNext();
    }
    return nullptr;
}

wxBitmapHandler *wxBitmapBase::FindHandler(wxBitmapType bitmapType)
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->GetData();
        if (handler->GetType() == bitmapType)
            return handler;
        node = node->GetNext();
    }
    return nullptr;
}

void wxBitmapBase::CleanUpHandlers()
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->GetData();
        wxList::compatibility_iterator next = node->GetNext();
        delete handler;
        sm_handlers.Erase(node);
        node = next;
    }
}

class wxBitmapBaseModule: public wxModule
{
    wxDECLARE_DYNAMIC_CLASS(wxBitmapBaseModule);
public:
    wxBitmapBaseModule() {}
    bool OnInit() override { wxBitmap::InitStandardHandlers(); return true; }
    void OnExit() override { wxBitmap::CleanUpHandlers(); }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapBaseModule, wxModule);

bool wxBitmapBase::CopyFromIcon(const wxIcon& icon)
{
    *this = icon;
    return IsOk();
}

// ----------------------------------------------------------------------------
// Trivial implementations of scale-factor related functions
// ----------------------------------------------------------------------------

bool wxBitmapBase::DoCreate(const wxSize& sz, double scale, int depth)
{
    return Create(sz*scale, depth);
}

void wxBitmapBase::SetScaleFactor(double WXUNUSED(scale))
{
}

double wxBitmapBase::GetScaleFactor() const
{
    return 1.0;
}

wxSize wxBitmapBase::GetDIPSize() const
{
    return GetSize() / GetScaleFactor();
}

#ifdef wxHAS_DPI_INDEPENDENT_PIXELS

double wxBitmapBase::GetLogicalWidth() const
{
    return GetWidth() / GetScaleFactor();
}

double wxBitmapBase::GetLogicalHeight() const
{
    return GetHeight() / GetScaleFactor();
}

wxSize wxBitmapBase::GetLogicalSize() const
{
    return wxSize(wxRound(GetLogicalWidth()), wxRound(GetLogicalHeight()));
}

#else // !wxHAS_DPI_INDEPENDENT_PIXELS

double wxBitmapBase::GetLogicalWidth() const
{
    return GetWidth();
}

double wxBitmapBase::GetLogicalHeight() const
{
    return GetHeight();
}

wxSize wxBitmapBase::GetLogicalSize() const
{
    return GetSize();
}

#endif // wxHAS_DPI_INDEPENDENT_PIXELS/!wxHAS_DPI_INDEPENDENT_PIXELS

// ----------------------------------------------------------------------------
// Alpha support
// ----------------------------------------------------------------------------

bool wxBitmapBase::HasAlpha() const
{
    // We assume that only 32bpp bitmaps use alpha (which is always true) and
    // that all 32bpp bitmaps do use it (which is not necessarily always the
    // case, but the ports where it isn't need to override this function to
    // deal with it as we can't do it here).
    return GetDepth() == 32;
}

bool wxBitmapBase::UseAlpha(bool WXUNUSED(use))
{
    // This function is not implemented in the case class, we don't have any
    // generic way to make a bitmap use, or prevent it from using, alpha.
    return false;
}

#endif // wxUSE_BITMAP_BASE

// ----------------------------------------------------------------------------
// wxBitmap common
// ----------------------------------------------------------------------------

#if !(defined(__WXGTK__) || defined(__WXX11__) || defined(__WXQT__))

wxBitmap::wxBitmap(const char* const* bits)
{
    wxCHECK2_MSG(bits != nullptr, return, wxT("invalid bitmap data"));

#if wxUSE_IMAGE && wxUSE_XPM
    wxImage image(bits);
    wxCHECK2_MSG(image.IsOk(), return, wxT("invalid bitmap data"));

    *this = wxBitmap(image);
#else
    wxFAIL_MSG(wxT("creating bitmaps from XPMs not supported"));
#endif // wxUSE_IMAGE && wxUSE_XPM
}
#endif // !(defined(__WXGTK__) || defined(__WXX11__))

// ----------------------------------------------------------------------------
// wxMaskBase
// ----------------------------------------------------------------------------

bool wxMaskBase::Create(const wxBitmap& bitmap, const wxColour& colour)
{
    FreeData();

    return InitFromColour(bitmap, colour);
}

#if wxUSE_PALETTE

bool wxMaskBase::Create(const wxBitmap& bitmap, int paletteIndex)
{
    wxPalette *pal = bitmap.GetPalette();

    wxCHECK_MSG( pal, false,
                 wxT("Cannot create mask from palette index of a bitmap without palette") );

    unsigned char r,g,b;
    pal->GetRGB(paletteIndex, &r, &g, &b);

    return Create(bitmap, wxColour(r, g, b));
}

#endif // wxUSE_PALETTE

bool wxMaskBase::Create(const wxBitmap& bitmap)
{
    FreeData();

    return InitFromMonoBitmap(bitmap);
}
