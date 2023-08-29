///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/statbmpg.h
// Purpose:     wxGenericStaticBitmap header
// Author:      Marcin Wojdyr, Stefan Csomor
// Created:     2008-06-16
// Copyright:   wxWidgets developers
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_STATBMP_H_
#define _WX_GENERIC_STATBMP_H_

#include "wx/statbmp.h"

class WXDLLIMPEXP_CORE wxGenericStaticBitmap : public wxStaticBitmapBase
{
public:
    wxGenericStaticBitmap() {}
    wxGenericStaticBitmap(wxWindow *parent,
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

    virtual void SetBitmap(const wxBitmapBundle& bitmap) override
    {
        m_bitmapBundle = bitmap;
        InvalidateBestSize();
        SetSize(GetBestSize());
        Refresh();
    }

    virtual void SetScaleMode(ScaleMode scaleMode) override
    {
        m_scaleMode = scaleMode;
        Refresh();
    }

    virtual ScaleMode GetScaleMode() const override { return m_scaleMode; }

private:
    void OnPaint(wxPaintEvent& event);

    ScaleMode m_scaleMode;

    wxDECLARE_DYNAMIC_CLASS(wxGenericStaticBitmap);
};


#endif //_WX_GENERIC_STATBMP_H_
