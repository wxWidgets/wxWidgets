#ifndef _WX_STATBMP_H_
#define _WX_STATBMP_H_

#include "wx/statbmp.h"

class WXDLLIMPEXP_CORE wxStaticBitmap : public wxStaticBitmapBase
{
public:
    wxStaticBitmap() {}
    wxStaticBitmap(wxWindow *parent,
                          wxWindowID id,
                          const wxBitmap& bitmap,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = 0,
                          const wxString& name = wxASCII_STR(wxStaticBitmapNameStr))
    {
        Create(parent, id, bitmap, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBitmapNameStr));

    virtual void SetBitmap(const wxBitmap& bitmap) wxOVERRIDE;

    virtual wxBitmap GetBitmap() const wxOVERRIDE { return m_bitmap; }

    virtual void SetIcon(const wxIcon& icon) wxOVERRIDE
    {
        wxBitmap bmp;
        bmp.CopyFromIcon(icon);
        SetBitmap(bmp);
    }

#if defined(__WXGTK20__) || defined(__WXMAC__)
    // icons and bitmaps are really the same thing in wxGTK and wxMac
    wxIcon GetIcon() const wxOVERRIDE  { return (const wxIcon &)m_bitmap; }
#endif

    virtual void SetScaleMode(ScaleMode scaleMode) wxOVERRIDE;

    virtual ScaleMode GetScaleMode() const wxOVERRIDE { return m_scaleMode; }

private:
    wxSize GetBitmapSize()
    {
        return m_bitmap.IsOk() ? m_bitmap.GetScaledSize()
        : wxSize(16, 16); // this is completely arbitrary
    }

    void OnPaint(wxPaintEvent& event);

    wxBitmap m_bitmap;
    ScaleMode m_scaleMode;

    wxDECLARE_DYNAMIC_CLASS(wxStaticBitmap);
};

#endif
