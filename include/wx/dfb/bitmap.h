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
// wxMask
//-----------------------------------------------------------------------------

#warning "FIXME: move wxMask to common code"
class WXDLLIMPEXP_CORE wxMask: public wxObject
{
public:
    wxMask();
    wxMask(const wxBitmap& bitmap);
    wxMask(const wxBitmap& bitmap, const wxColour& colour);
#if wxUSE_PALETTE
    wxMask(const wxBitmap& bitmap, int paletteIndex);
#endif
    wxMask(const wxMask& mask);
    ~wxMask();

    bool Create(const wxBitmap& bitmap);
    bool Create(const wxBitmap& bitmap, const wxColour& colour);
#if wxUSE_PALETTE
    bool Create(const wxBitmap& bitmap, int paletteIndex);
#endif

    // implementation
    const wxBitmap& GetBitmap() const;

private:
    wxBitmap *m_bitmap;

    DECLARE_DYNAMIC_CLASS(wxMask)
};

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmapHandler : public wxBitmapHandlerBase
{
public:
    wxBitmapHandler() : wxBitmapHandlerBase() {}
private:
    DECLARE_DYNAMIC_CLASS(wxBitmapHandler)
};

class WXDLLIMPEXP_CORE wxBitmap: public wxBitmapBase
{
public:
    wxBitmap() {}
    wxBitmap(int width, int height, int depth = -1);
    wxBitmap(const char bits[], int width, int height, int depth = 1);
    wxBitmap(const wxString &filename, wxBitmapType type = wxBITMAP_TYPE_RESOURCE);
    wxBitmap(const char **bits) { CreateFromXpm(bits); }
    wxBitmap(char **bits) { CreateFromXpm((const char **)bits); }
#if wxUSE_IMAGE
    wxBitmap(const wxImage& image, int depth = -1);
#endif

    bool Ok() const;
    bool operator==(const wxBitmap& bmp) const;
    bool operator!=(const wxBitmap& bmp) const { return !(*this == bmp); }

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
    bool CreateFromXpm(const char **bits);

    // ref counting code
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;

    DECLARE_DYNAMIC_CLASS(wxBitmap)
};

#endif // _WX_DFB_BITMAP_H_
