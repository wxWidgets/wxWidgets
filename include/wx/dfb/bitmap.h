/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/bitmap.h
// Purpose:     wxBitmap class
// Author:      Vaclav Slavik
// Created:     2006-08-04
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_BITMAP_H_
#define _WX_DFB_BITMAP_H_

#include "wx/dfb/dfbptr.h"

wxDFB_DECLARE_INTERFACE(IDirectFBSurface);

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmapHandler: public wxBitmapHandlerBase
{
    DECLARE_ABSTRACT_CLASS(wxBitmapHandler)
};

class WXDLLIMPEXP_CORE wxBitmap: public wxBitmapBase
{
public:
    wxBitmap() {}
    wxBitmap(const wxIDirectFBSurfacePtr& surface) { Create(surface); }
    wxBitmap(int width, int height, int depth = -1);
    wxBitmap(const char bits[], int width, int height, int depth = 1);
    wxBitmap(const wxString &filename, wxBitmapType type = wxBITMAP_TYPE_RESOURCE);
    wxBitmap(const char* const* bits);
#if wxUSE_IMAGE
    wxBitmap(const wxImage& image, int depth = -1);
#endif

    bool Ok() const { return IsOk(); }
    bool IsOk() const;
    bool operator==(const wxBitmap& bmp) const;
    bool operator!=(const wxBitmap& bmp) const { return !(*this == bmp); }

    bool Create(const wxIDirectFBSurfacePtr& surface);
    bool Create(int width, int height, int depth = -1);

    virtual int GetHeight() const;
    virtual int GetWidth() const;
    virtual int GetDepth() const;

#if wxUSE_IMAGE
    virtual wxImage ConvertToImage() const;
#endif

    virtual wxMask *GetMask() const;
    virtual void SetMask(wxMask *mask);

    virtual wxBitmap GetSubBitmap(const wxRect& rect) const;

    virtual bool SaveFile(const wxString &name, wxBitmapType type, const wxPalette *palette = (wxPalette *) NULL) const;
    virtual bool LoadFile(const wxString &name, wxBitmapType type = wxBITMAP_TYPE_RESOURCE);

#if wxUSE_PALETTE
    virtual wxPalette *GetPalette() const;
    virtual void SetPalette(const wxPalette& palette);
#endif

    // copies the contents and mask of the given (colour) icon to the bitmap
    virtual bool CopyFromIcon(const wxIcon& icon);

    static void InitStandardHandlers();

    // implementation:
    virtual void SetHeight(int height);
    virtual void SetWidth(int width);
    virtual void SetDepth(int depth);

    // get underlying native representation:
    wxIDirectFBSurfacePtr GetDirectFBSurface() const;

protected:
    // ref counting code
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;

    DECLARE_DYNAMIC_CLASS(wxBitmap)
};

#endif // _WX_DFB_BITMAP_H_
