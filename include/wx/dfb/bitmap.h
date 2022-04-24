/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/bitmap.h
// Purpose:     wxBitmap class
// Author:      Vaclav Slavik
// Created:     2006-08-04
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_BITMAP_H_
#define _WX_DFB_BITMAP_H_

#include "wx/dfb/dfbptr.h"

class WXDLLIMPEXP_FWD_CORE wxPixelDataBase;

wxDFB_DECLARE_INTERFACE(IDirectFBSurface);

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmap : public wxBitmapBase
{
public:
    wxBitmap() {}
    wxBitmap(const wxIDirectFBSurfacePtr& surface) { Create(surface); }
    wxBitmap(int width, int height, int depth = -1) { Create(width, height, depth); }
    wxBitmap(const wxSize& sz, int depth = -1) { Create(sz, depth); }
    wxBitmap(int width, int height, const wxDC& dc) { Create(width, height, dc); }
    wxBitmap(const char bits[], int width, int height, int depth = 1);
    wxBitmap(const wxString &filename, wxBitmapType type = wxBITMAP_DEFAULT_TYPE);
    wxBitmap(const char* const* bits);
#if wxUSE_IMAGE
    wxBitmap(const wxImage& image, int depth = -1, double scale = 1.0)
        { InitFromImage(image, depth, scale); }
    wxBitmap(const wxImage& image, const wxDC& WXUNUSED(dc))
        { InitFromImage(image, -1, 1.0); }
#endif

    bool Create(const wxIDirectFBSurfacePtr& surface);
    bool Create(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH);
    bool Create(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH)
        { return Create(sz.GetWidth(), sz.GetHeight(), depth); }
    bool Create(int width, int height, const wxDC& WXUNUSED(dc))
        { return Create(width,height); }

    virtual int GetHeight() const;
    virtual int GetWidth() const;
    virtual int GetDepth() const;

#if wxUSE_IMAGE
    virtual wxImage ConvertToImage() const;
#endif

    virtual wxMask *GetMask() const;
    virtual void SetMask(wxMask *mask);

    virtual wxBitmap GetSubBitmap(const wxRect& rect) const;

    virtual bool SaveFile(const wxString &name, wxBitmapType type,
                          const wxPalette *palette = NULL) const;
    virtual bool LoadFile(const wxString &name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE);

#if wxUSE_PALETTE
    virtual wxPalette *GetPalette() const;
    virtual void SetPalette(const wxPalette& palette);
#endif

    static void InitStandardHandlers();

    // raw bitmap access support functions
    void *GetRawData(wxPixelDataBase& data, int bpp);
    void UngetRawData(wxPixelDataBase& data);

    bool HasAlpha() const;

    // implementation:
#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED(virtual void SetHeight(int height));
    wxDEPRECATED(virtual void SetWidth(int width));
    wxDEPRECATED(virtual void SetDepth(int depth));
#endif

    // get underlying native representation:
    wxIDirectFBSurfacePtr GetDirectFBSurface() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    void InitFromImage(const wxImage& image, int depth, double scale);

    bool CreateWithFormat(int width, int height, int dfbFormat);

    wxDECLARE_DYNAMIC_CLASS(wxBitmap);
};

#endif // _WX_DFB_BITMAP_H_
