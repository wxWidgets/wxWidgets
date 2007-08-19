#ifndef __WX_BITMAP_H__
#define __WX_BITMAP_H__

#include <Magick++.h>

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

    virtual bool CopyFromIcon(const wxIcon& icon);

    static void InitStandardHandlers();

    virtual void SetHeight(int height);
    virtual void SetWidth(int width);
    virtual void SetDepth(int depth);

protected:
    Magick::Image GetMagickImage() const;
    void SetMagickImage(const Magick::Image& image);

    virtual wxObjectRefData* CreateRefData() const;
    virtual wxObjectRefData* CloneRefData(const wxObjectRefData* data) const;

private:
    friend class wxMemoryDC;
    DECLARE_DYNAMIC_CLASS(wxBitmap)
};

#endif // __WX_BITMAP_H__
