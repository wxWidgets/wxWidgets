/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/bitmap.h
// Purpose:     wxBitmap class for classic Mac OS
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_BITMAP_H_
#define _WX_RETRO68_BITMAP_H_

class WXDLLIMPEXP_FWD_CORE wxPixelDataBase;

// Toolbox offscreen graphics world, declared in QDOffscreen.h
struct CGrafPort;

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

// wxBitmap always uses 32bpp xRGB offscreen GWorld for its pixels, with the
// otherwise unused high byte storing the alpha channel, if any.
class WXDLLIMPEXP_CORE wxBitmap : public wxBitmapBase
{
public:
    wxBitmap() = default;
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

    bool Create(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH) final;
    bool Create(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH) final
        { return Create(sz.GetWidth(), sz.GetHeight(), depth); }
    bool Create(int width, int height, const wxDC& WXUNUSED(dc))
        { return Create(width,height); }

    virtual int GetHeight() const override;
    virtual int GetWidth() const override;
    virtual int GetDepth() const override;

#if wxUSE_IMAGE
    virtual wxImage ConvertToImage() const override;
#endif

    virtual wxMask *GetMask() const override;
    virtual void SetMask(wxMask *mask) override;

    virtual wxBitmap GetSubBitmap(const wxRect& rect) const override;

    virtual bool SaveFile(const wxString &name, wxBitmapType type,
                          const wxPalette *palette = nullptr) const override;
    virtual bool LoadFile(const wxString &name,
                          wxBitmapType type = wxBITMAP_DEFAULT_TYPE) override;

#if wxUSE_PALETTE
    virtual wxPalette *GetPalette() const override;
    virtual void SetPalette(const wxPalette& palette) override;
#endif

    static void InitStandardHandlers();

    // raw bitmap access support functions
    void *GetRawData(wxPixelDataBase& data, int bpp);
    void UngetRawData(wxPixelDataBase& data);

    virtual bool HasAlpha() const override;
    virtual bool UseAlpha(bool use = true) override;

    // implementation:
#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED(virtual void SetHeight(int height) override);
    wxDEPRECATED(virtual void SetWidth(int width) override);
    wxDEPRECATED(virtual void SetDepth(int depth) override);
#endif

    // get underlying native representation: the offscreen GWorld
    CGrafPort* GetGWorld() const;

    // get pointer to the pixels and the number of bytes in one row, the
    // pixels must be locked while they're used
    unsigned char* LockPixels(int* rowBytes) const;
    void UnlockPixels() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const override;
    wxNODISCARD virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

#if wxUSE_IMAGE
    void InitFromImage(const wxImage& image, int depth, double scale);
#endif

    wxDECLARE_DYNAMIC_CLASS(wxBitmap);
};

#endif // _WX_RETRO68_BITMAP_H_
