/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/statbmp.h
// Purpose:     wxWinUI wxStaticBitmap declaration (WinUI Image)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_STATBMP_H_
#define _WX_WINUI_STATBMP_H_

#include "wx/icon.h"

#include <memory>

class wxWinUIStaticBitmapImpl;

class WXDLLIMPEXP_CORE wxStaticBitmap : public wxStaticBitmapBase
{
public:
    wxStaticBitmap();
    wxStaticBitmap(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmapBundle& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxASCII_STR(wxStaticBitmapNameStr));
    wxStaticBitmap(wxWindow *parent,
                   wxWindowID id,
                   const wxIcon& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxASCII_STR(wxStaticBitmapNameStr));
    ~wxStaticBitmap() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmapBundle& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBitmapNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxIcon& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBitmapNameStr));

    void SetBitmap(const wxBitmapBundle& bitmap) override;
    wxBitmap GetBitmap() const override;
    void SetIcon(const wxIcon& icon) override;
    wxIcon GetIcon() const override;

    void SetScaleMode(ScaleMode scaleMode) override;
    ScaleMode GetScaleMode() const override { return m_scaleMode; }

protected:
    wxSize DoGetBestSize() const override;

    bool DoCreate(wxWindow *parent,
                  wxWindowID id,
                  const wxPoint& pos,
                  const wxSize& size,
                  long style,
                  const wxString& name);

    void UpdateWinUIImage();
    wxSize GetImageSize() const;

    std::unique_ptr<wxWinUIStaticBitmapImpl> m_winui;
    wxIcon m_icon;
    ScaleMode m_scaleMode = Scale_None;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStaticBitmap);
};

#endif // _WX_WINUI_STATBMP_H_
