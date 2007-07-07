#ifndef __WX_BITMAP_H__
#define __WX_BITMAP_H__

class WXDLLEXPORT wxMask: public wxObject {
public:
    wxMask();
    wxMask(const wxBitmap& bitmap, const wxColour& colour);
    wxMask(const wxBitmap& bitmap, int paletteIndex);
    wxMask(const wxBitmap& bitmap);
    ~wxMask();

    bool Create(const wxBitmap& bitmap, const wxColour& colour);
    bool Create(const wxBitmap& bitmap, int paletteIndex);
    bool Create(const wxBitmap& bitmap);

private:
    DECLARE_DYNAMIC_CLASS(wxMask)
};

class WXDLLEXPORT wxBitmapHandler : public wxBitmapHandlerBase {
public:
    wxBitmapHandler();
private:
    DECLARE_DYNAMIC_CLASS(wxBitmapHandler)
};

class WXDLLEXPORT wxBitmap: public wxBitmapBase {
public:
    wxBitmap();
    wxBitmap(int width, int height, int depth = -1);
    wxBitmap(const char bits[], int width, int height, int depth = 1);
    wxBitmap(const char* const* bits);
    wxBitmap(const wxString &filename, wxBitmapType type = wxBITMAP_TYPE_RESOURCE);
#if wxUSE_IMAGE
    wxBitmap(const wxImage& image, int depth = -1);
#endif // wxUSE_IMAGE
    virtual ~wxBitmap();

    virtual bool IsOk() const;

    bool Create(int width, int height, int depth = -1);

    virtual int GetHeight() const;
    virtual int GetWidth() const;
    virtual int GetDepth() const;

#if wxUSE_IMAGE
    virtual wxImage ConvertToImage() const;
#endif // wxUSE_IMAGE

    virtual wxMask *GetMask() const;
    virtual void SetMask(wxMask *mask);

    virtual wxBitmap GetSubBitmap(const wxRect& rect) const;

    virtual bool SaveFile(const wxString &name, wxBitmapType type, const wxPalette *palette = (wxPalette *) NULL) const;
    virtual bool LoadFile(const wxString &name, wxBitmapType type = wxBITMAP_TYPE_RESOURCE);

#if wxUSE_PALETTE
    virtual wxPalette *GetPalette() const;
    virtual void SetPalette(const wxPalette& palette);
#endif // wxUSE_PALETTE


    virtual bool CopyFromIcon(const wxIcon& icon);

    static void InitStandardHandlers();

    virtual void SetHeight(int height);
    virtual void SetWidth(int width);
    virtual void SetDepth(int depth);

protected:
    bool CreateFromXpm(const char **bits);
    void SetMonoPalette(const wxColour& fg, const wxColour& bg);

private:
    DECLARE_DYNAMIC_CLASS(wxBitmap)
};

#endif // __WX_BITMAP_H__
