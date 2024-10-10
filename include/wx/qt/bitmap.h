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
#if wxUSE_IMAGE
    wxBitmap(const wxImage& image, int depth = wxBITMAP_SCREEN_DEPTH, double scale = 1.0);
    wxBitmap(const wxImage& image, const wxDC& dc);
#endif // wxUSE_IMAGE

    // Convert from wxIcon / wxCursor
    wxBitmap(const wxIcon& icon) { CopyFromIcon(icon); }
    explicit wxBitmap(const wxCursor& cursor);

    static void InitStandardHandlers();

    bool Create(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH) final;
    bool Create(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH) final;
    bool Create(int width, int height, const wxDC& dc);

    virtual void SetScaleFactor(double scale) override;
    virtual double GetScaleFactor() const override;

    virtual int GetHeight() const override;
    virtual int GetWidth() const override;
    virtual int GetDepth() const override;

#if wxUSE_IMAGE
    virtual wxImage ConvertToImage() const override;
#endif // wxUSE_IMAGE

    virtual wxMask *GetMask() const override;
    virtual void SetMask(wxMask *mask) override;

    virtual wxBitmap GetSubBitmap(const wxRect& rect) const override;

    virtual bool SaveFile(const wxString &name, wxBitmapType type,
                          const wxPalette *palette = nullptr) const override;
    virtual bool LoadFile(const wxString &name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE) override;

#if wxUSE_PALETTE
    virtual wxPalette *GetPalette() const override;
    virtual void SetPalette(const wxPalette& palette) override;
#endif // wxUSE_PALETTE

    // implementation:
#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED(virtual void SetHeight(int height) override);
    wxDEPRECATED(virtual void SetWidth(int width) override);
    wxDEPRECATED(virtual void SetDepth(int depth) override);
#endif

    void *GetRawData(wxPixelDataBase& data, int bpp);
    void UngetRawData(wxPixelDataBase& data);

    // these functions are internal and shouldn't be used, they risk to
    // disappear in the future
    bool HasAlpha() const override;

    // Blend mask with alpha channel and remove the mask
    void QtBlendMaskWithAlpha();

    QPixmap *GetHandle() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const override;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

    virtual bool DoCreate(const wxSize& sz, double scale, int depth) override;

private:
#if wxUSE_IMAGE
    void InitFromImage(const wxImage& image, int depth, double WXUNUSED(scale));
#endif

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

    // Construct a mask from QBitmap, takes ownership.
    explicit wxMask(QBitmap* qtBitmap);

    wxBitmap GetBitmap() const;

    // Implementation
    QBitmap *GetHandle() const;

protected:
    // this function is called from Create() to free the existing mask data
    void FreeData() override;
    // by the public wrappers
    bool InitFromColour(const wxBitmap& bitmap, const wxColour& colour) override;
    bool InitFromMonoBitmap(const wxBitmap& bitmap) override;

protected:
    wxDECLARE_DYNAMIC_CLASS(wxMask);

private:
    QBitmap *m_qtBitmap;
};

#endif // _WX_QT_BITMAP_H_
