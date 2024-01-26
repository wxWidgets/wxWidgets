#ifndef _WX_STATBMP_H_
#define _WX_STATBMP_H_

#include "wx/statbmp.h"

class WXDLLIMPEXP_CORE wxStaticBitmap : public wxStaticBitmapBase
{
public:
    wxStaticBitmap() = default;
    wxStaticBitmap(wxWindow *parent,
                          wxWindowID id,
                          const wxBitmapBundle& bitmap,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = 0,
                          const wxString& name = wxASCII_STR(wxStaticBitmapNameStr))
    {
        Create(parent, id, bitmap, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmapBundle& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBitmapNameStr));

    virtual void SetBitmap(const wxBitmapBundle& bitmap) override;

    virtual void SetScaleMode(ScaleMode scaleMode) override;

    virtual ScaleMode GetScaleMode() const override { return m_scaleMode; }

private:
    void OnPaint(wxPaintEvent& event);

    ScaleMode m_scaleMode;

    wxDECLARE_DYNAMIC_CLASS(wxStaticBitmap);
};

#endif
