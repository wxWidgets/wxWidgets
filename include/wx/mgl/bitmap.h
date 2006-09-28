/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.h
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_BITMAP_H__
#define __WX_BITMAP_H__

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxMemoryDC;

class MGLDevCtx;
struct bitmap_t;

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxBitmapHandler : public wxBitmapHandlerBase
{
public:
    wxBitmapHandler() : wxBitmapHandlerBase() {}
private:
    DECLARE_DYNAMIC_CLASS(wxBitmapHandler)
};

class WXDLLEXPORT wxBitmap: public wxBitmapBase
{
public:
    wxBitmap() {}
    wxBitmap(int width, int height, int depth = -1);
    wxBitmap(const char bits[], int width, int height, int depth = 1);
    wxBitmap(const char **bits) { CreateFromXpm(bits); }
    wxBitmap(char **bits) { CreateFromXpm((const char **)bits); }
    wxBitmap(const wxString &filename, wxBitmapType type = wxBITMAP_TYPE_RESOURCE);
    wxBitmap(const wxImage& image, int depth = -1);
    virtual ~wxBitmap() {}
    bool operator == (const wxBitmap& bmp) const;
    bool operator != (const wxBitmap& bmp) const;
    bool Ok() const;

    bool Create(int width, int height, int depth = -1);

    virtual int GetHeight() const;
    virtual int GetWidth() const;
    virtual int GetDepth() const;

    virtual wxImage ConvertToImage() const;

    virtual wxMask *GetMask() const;
    virtual void SetMask(wxMask *mask);

    virtual wxBitmap GetSubBitmap(const wxRect& rect) const;

    virtual bool SaveFile(const wxString &name, wxBitmapType type, const wxPalette *palette = (wxPalette *) NULL) const;
    virtual bool LoadFile(const wxString &name, wxBitmapType type = wxBITMAP_TYPE_RESOURCE);

    virtual wxPalette *GetPalette() const;
    virtual void SetPalette(const wxPalette& palette);

    // copies the contents and mask of the given (colour) icon to the bitmap
    virtual bool CopyFromIcon(const wxIcon& icon);

    static void InitStandardHandlers();

    // implementation:
    virtual void SetHeight(int height);
    virtual void SetWidth(int width);
    virtual void SetDepth(int depth);

    virtual wxColour QuantizeColour(const wxColour& colour) const;

    // get underlying native representation:
    bitmap_t *GetMGLbitmap_t() const;

protected:
    bool CreateFromXpm(const char **bits);

    // creates temporary DC for access to bitmap's data:
    MGLDevCtx *CreateTmpDC() const;
    // sets fg & bg colours for 1bit bitmaps:
    void SetMonoPalette(const wxColour& fg, const wxColour& bg);

private:
    DECLARE_DYNAMIC_CLASS(wxBitmap)

    friend class wxDC;
    friend class wxMemoryDC;
};

#endif // __WX_BITMAP_H__
