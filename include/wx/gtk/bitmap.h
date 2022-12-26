/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/bitmap.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_BITMAP_H_
#define _WX_GTK_BITMAP_H_

#ifdef __WXGTK3__
typedef struct _cairo cairo_t;
typedef struct _cairo_surface cairo_surface_t;
#endif
typedef struct _GdkPixbuf GdkPixbuf;

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMask: public wxMaskBase
{
public:
    wxMask();
    wxMask(const wxMask& mask);
    wxMask( const wxBitmap& bitmap, const wxColour& colour );
#if wxUSE_PALETTE
    wxMask( const wxBitmap& bitmap, int paletteIndex );
#endif // wxUSE_PALETTE
    wxMask( const wxBitmap& bitmap );
    virtual ~wxMask();
    wxBitmap GetBitmap() const;

    // implementation
#ifdef __WXGTK3__
    wxMask(cairo_surface_t*);
    operator cairo_surface_t*() const;
#else
    wxMask(GdkPixmap*);
    operator GdkPixmap*() const;
#endif

protected:
    virtual void FreeData() override;
    virtual bool InitFromColour(const wxBitmap& bitmap, const wxColour& colour) override;
    virtual bool InitFromMonoBitmap(const wxBitmap& bitmap) override;

private:
#ifdef __WXGTK3__
    cairo_surface_t* m_bitmap;
#else
    GdkPixmap* m_bitmap;
#endif

    wxDECLARE_DYNAMIC_CLASS(wxMask);
};

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmap: public wxBitmapBase
{
public:
    wxBitmap() { }
    wxBitmap( int width, int height, int depth = wxBITMAP_SCREEN_DEPTH )
        { Create(width, height, depth); }
    wxBitmap( const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH )
        { Create(sz, depth); }
    wxBitmap( int width, int height, const wxDC& dc )
        { Create(width, height, dc); }
    wxBitmap( const char bits[], int width, int height, int depth = 1 );
    wxBitmap( const char* const* bits );
    wxBitmap( const wxString &filename, wxBitmapType type = wxBITMAP_DEFAULT_TYPE );
#if wxUSE_IMAGE
    wxBitmap(const wxImage& image, int depth = wxBITMAP_SCREEN_DEPTH, double scale = 1.0);
    wxBitmap(const wxImage& image, const wxDC& dc);
#endif // wxUSE_IMAGE
    wxBitmap(GdkPixbuf* pixbuf, int depth = 0);
    explicit wxBitmap(const wxCursor& cursor);
    virtual ~wxBitmap();

    bool Create(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH) override;
    bool Create(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH) override
        { return Create(sz.GetWidth(), sz.GetHeight(), depth); }
#ifdef __WXGTK3__
    bool Create(int width, int height, const wxDC& dc);
    virtual void SetScaleFactor(double scale) override;
    virtual double GetScaleFactor() const override;
#else
    bool Create(int width, int height, const wxDC& WXUNUSED(dc))
        { return Create(width,height); }
#endif

    virtual int GetHeight() const override;
    virtual int GetWidth() const override;
    virtual int GetDepth() const override;

#if wxUSE_IMAGE
    wxImage ConvertToImage() const override;
#endif // wxUSE_IMAGE

    wxMask *GetMask() const override;
    void SetMask( wxMask *mask ) override;

    wxBitmap GetSubBitmap( const wxRect& rect ) const override;

    bool SaveFile(const wxString &name, wxBitmapType type,
                          const wxPalette *palette = nullptr) const override;
    bool LoadFile(const wxString &name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE) override;

#if wxUSE_PALETTE
    wxPalette *GetPalette() const override;
    void SetPalette(const wxPalette& palette) override;
    wxPalette *GetColourMap() const { return GetPalette(); }
#endif // wxUSE_PALETTE

    static void InitStandardHandlers();

    // implementation
    // --------------

#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED(void SetHeight( int height ) override);
    wxDEPRECATED(void SetWidth( int width ) override);
    wxDEPRECATED(void SetDepth( int depth ) override);
#endif

#ifdef __WXGTK3__
    cairo_t* CairoCreate() const;
    void Draw(cairo_t* cr, int x, int y, bool useMask = true, const wxColour* fg = nullptr, const wxColour* bg = nullptr) const;
    void SetSourceSurface(cairo_t* cr, int x, int y, const wxColour* fg = nullptr, const wxColour* bg = nullptr) const;
    wxBitmap CreateDisabled() const;
#else
    GdkPixmap *GetPixmap() const;
    bool HasPixmap() const;
    bool HasPixbuf() const;
    wxBitmap(GdkPixmap* pixmap);
#endif
    GdkPixbuf* GetPixbufNoMask() const;
    GdkPixbuf *GetPixbuf() const;

    // raw bitmap access support functions
    void *GetRawData(wxPixelDataBase& data, int bpp);
    void UngetRawData(wxPixelDataBase& data);

    bool HasAlpha() const override;

protected:
#if wxUSE_IMAGE
    void InitFromImage(const wxImage& image, int depth, double scale);
#ifndef __WXGTK3__
    bool CreateFromImage(const wxImage& image, int depth);
#endif
#endif // wxUSE_IMAGE

    virtual wxGDIRefData* CreateGDIRefData() const override;
    virtual wxGDIRefData* CloneGDIRefData(const wxGDIRefData* data) const override;

#ifdef __WXGTK3__
    virtual bool DoCreate(const wxSize& sz, double scale, int depth) override;
#endif

private:
#ifndef __WXGTK3__
    void SetPixmap(GdkPixmap* pixmap);
#if wxUSE_IMAGE
    // to be called from CreateFromImage only!
    bool CreateFromImageAsPixmap(const wxImage& image, int depth);
    bool CreateFromImageAsPixbuf(const wxImage& image);
#endif // wxUSE_IMAGE

public:
    // implementation only
    enum Representation
    {
        Pixmap,
        Pixbuf
    };
    // removes other representations from memory, keeping only 'keep'
    // (wxBitmap may keep same bitmap e.g. as both pixmap and pixbuf):
    void PurgeOtherRepresentations(Representation keep);
#endif

    wxDECLARE_DYNAMIC_CLASS(wxBitmap);
};

#endif // _WX_GTK_BITMAP_H_
