/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/bitmap.h
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_BITMAP_H_
#define _WX_QT_BITMAP_H_

class QImage;
class QPixmap;
class QBitmap;

class WXDLLIMPEXP_CORE wxBitmap : public wxBitmapBase
{
public:
    wxBitmap();
    wxBitmap(QPixmap pix);
    wxBitmap(const char bits[], int width, int height, int depth = 1);
    wxBitmap(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH);
    wxBitmap(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH);
    wxBitmap(int width, int height, const wxDC& dc);
    wxBitmap(const char* const* bits);
    wxBitmap(const wxString &filename, wxBitmapType type = wxBITMAP_TYPE_XPM);
    wxBitmap(const wxImage& image, int depth = wxBITMAP_SCREEN_DEPTH, double scale = 1.0);
    wxBitmap(const wxImage& image, const wxDC& dc);

    // Convert from wxIcon / wxCursor
    wxBitmap(const wxIcon& icon) { CopyFromIcon(icon); }
    explicit wxBitmap(const wxCursor& cursor);

    static void InitStandardHandlers();

    virtual bool Create(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH) wxOVERRIDE;
    virtual bool Create(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH) wxOVERRIDE;
    virtual bool Create(int width, int height, const wxDC& dc);

    virtual int GetHeight() const wxOVERRIDE;
    virtual int GetWidth() const wxOVERRIDE;
    virtual int GetDepth() const wxOVERRIDE;

#if wxUSE_IMAGE
    virtual wxImage ConvertToImage() const wxOVERRIDE;
#endif // wxUSE_IMAGE

    virtual wxMask *GetMask() const wxOVERRIDE;
    virtual void SetMask(wxMask *mask) wxOVERRIDE;

    virtual wxBitmap GetSubBitmap(const wxRect& rect) const wxOVERRIDE;

    virtual bool SaveFile(const wxString &name, wxBitmapType type,
                          const wxPalette *palette = NULL) const wxOVERRIDE;
    virtual bool LoadFile(const wxString &name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE) wxOVERRIDE;

#if wxUSE_PALETTE
    virtual wxPalette *GetPalette() const wxOVERRIDE;
    virtual void SetPalette(const wxPalette& palette) wxOVERRIDE;
#endif // wxUSE_PALETTE

    // implementation:
#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED(virtual void SetHeight(int height) wxOVERRIDE);
    wxDEPRECATED(virtual void SetWidth(int width) wxOVERRIDE);
    wxDEPRECATED(virtual void SetDepth(int depth) wxOVERRIDE);
#endif

    void *GetRawData(wxPixelDataBase& data, int bpp);
    void UngetRawData(wxPixelDataBase& data);

    // these functions are internal and shouldn't be used, they risk to
    // disappear in the future
    bool HasAlpha() const wxOVERRIDE;

    QPixmap *GetHandle() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const wxOVERRIDE;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const wxOVERRIDE;

private:
    void InitFromImage(const wxImage& image, int depth, double WXUNUSED(scale));

    wxDECLARE_DYNAMIC_CLASS(wxBitmap);
};

class WXDLLIMPEXP_CORE wxMask : public wxMaskBase
{
public:
    wxMask();

    // Copy constructor
    wxMask(const wxMask &mask);
    wxMask& operator=(const wxMask &mask);

    // Construct a mask from a bitmap and a colour indicating the transparent
    // area
    wxMask(const wxBitmap& bitmap, const wxColour& colour);

    // Construct a mask from a bitmap and a palette index indicating the
    // transparent area
    wxMask(const wxBitmap& bitmap, int paletteIndex);

    // Construct a mask from a mono bitmap (copies the bitmap).
    wxMask(const wxBitmap& bitmap);
    virtual ~wxMask();

    wxBitmap GetBitmap() const;

    // Implementation
    QBitmap *GetHandle() const;

protected:
    // this function is called from Create() to free the existing mask data
    void FreeData() wxOVERRIDE;
    // by the public wrappers
    bool InitFromColour(const wxBitmap& bitmap, const wxColour& colour) wxOVERRIDE;
    bool InitFromMonoBitmap(const wxBitmap& bitmap) wxOVERRIDE;

protected:
    wxDECLARE_DYNAMIC_CLASS(wxMask);

private:
    QBitmap *m_qtBitmap;
};

#endif // _WX_QT_BITMAP_H_
