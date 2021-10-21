#ifndef _WX_STATBMP_H_
#define _WX_STATBMP_H_

#include "wx/statbmp.h"

class WXDLLIMPEXP_CORE wxStaticBitmap : public wxStaticBitmapBase
{
public:
    wxStaticBitmap() {}
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

    virtual void SetBitmap(const wxBitmapBundle& bitmap) wxOVERRIDE;

    virtual void SetScaleMode(ScaleMode scaleMode) wxOVERRIDE;

    virtual ScaleMode GetScaleMode() const wxOVERRIDE { return m_scaleMode; }

private:
    void OnPaint(wxPaintEvent& event);

    ScaleMode m_scaleMode;

    wxDECLARE_DYNAMIC_CLASS(wxStaticBitmap);
};

#endif
