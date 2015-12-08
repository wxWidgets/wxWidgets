/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/bitmap.h
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_BITMAP_H_
#define _WX_QT_BITMAP_H_

class WXDLLIMPEXP_FWD_CORE wxPixelDataBase;

class WXDLLIMPEXP_FWD_CORE wxImage;
class WXDLLIMPEXP_FWD_CORE wxCursor;
class QImage;

class QPixmap;
class QBitmap;

class WXDLLIMPEXP_CORE wxBitmap : public wxBitmapBase
{
public:
    wxBitmap();
    wxBitmap(QPixmap pix);
    wxBitmap(const wxBitmap& bmp);
    wxBitmap(const char bits[], int width, int height, int depth = 1);
    wxBitmap(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH);
    wxBitmap(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH);
    wxBitmap(const char* const* bits);
    wxBitmap(const wxString &filename, wxBitmapType type = wxBITMAP_TYPE_XPM);
    wxBitmap(const wxImage& image, int depth = wxBITMAP_SCREEN_DEPTH, double scale = 1.0);
    
    // Convert from wxIcon / wxCursor
    wxBitmap(const wxIcon& icon) { CopyFromIcon(icon); }
    wxEXPLICIT wxBitmap(const wxCursor& cursor);

    static void InitStandardHandlers();

    virtual bool Create(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH);
    virtual bool Create(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH);
    virtual bool Create(int width, int height, const wxDC& WXUNUSED(dc));

    virtual int GetHeight() const;
    virtual int GetWidth() const;
    virtual int GetDepth() const;

#if wxUSE_IMAGE
    virtual wxImage ConvertToImage() const;
#endif // wxUSE_IMAGE

    virtual wxMask *GetMask() const;
    virtual void SetMask(wxMask *mask);

    virtual wxBitmap GetSubBitmap(const wxRect& rect) const;

    virtual bool SaveFile(const wxString &name, wxBitmapType type,
                          const wxPalette *palette = NULL) const;
    virtual bool LoadFile(const wxString &name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE);

#if wxUSE_PALETTE
    virtual wxPalette *GetPalette() const;
    virtual void SetPalette(const wxPalette& palette);
#endif // wxUSE_PALETTE

    // copies the contents and mask of the given (colour) icon to the bitmap
    virtual bool CopyFromIcon(const wxIcon& icon);

    // implementation:
    virtual void SetHeight(int height);
    virtual void SetWidth(int width);
    virtual void SetDepth(int depth);

    void *GetRawData(wxPixelDataBase& data, int bpp);
    void UngetRawData(wxPixelDataBase& data);

    // these functions are internal and shouldn't be used, they risk to
    // disappear in the future
    bool HasAlpha() const;

    QPixmap *GetHandle() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    wxDECLARE_DYNAMIC_CLASS(wxBitmap);
};

class WXDLLIMPEXP_CORE wxMask : public wxObject
{
public:
    wxMask();

    // Copy constructor
    wxMask(const wxMask &mask);

    // Construct a mask from a bitmap and a colour indicating the transparent
    // area
    wxMask(const wxBitmap& bitmap, const wxColour& colour);

    // Construct a mask from a bitmap and a palette index indicating the
    // transparent area
    wxMask(const wxBitmap& bitmap, int paletteIndex);

    // Construct a mask from a mono bitmap (copies the bitmap).
    wxMask(const wxBitmap& bitmap);


    virtual ~wxMask();

    bool Create(const wxBitmap& bitmap, const wxColour& colour);
    bool Create(const wxBitmap& bitmap, int paletteIndex);
    bool Create(const wxBitmap& bitmap);

    wxBitmap GetBitmap() const;

    // Implementation
    QBitmap *GetHandle() const;

protected:
    wxDECLARE_DYNAMIC_CLASS(wxMask);

private:
    QBitmap *m_qtBitmap;
};

#endif // _WX_QT_BITMAP_H_
