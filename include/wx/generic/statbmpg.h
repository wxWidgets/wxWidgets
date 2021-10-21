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

    virtual void SetBitmap(const wxBitmap& bitmap) wxOVERRIDE
    {
        m_bitmap = bitmap;
        InvalidateBestSize();
        SetSize(GetBestSize());
        Refresh();
    }

    virtual wxBitmap GetBitmap() const wxOVERRIDE { return m_bitmap; }

    virtual void SetScaleMode(ScaleMode scaleMode) wxOVERRIDE
    {
        m_scaleMode = scaleMode;
        Refresh();
    }

    virtual ScaleMode GetScaleMode() const wxOVERRIDE { return m_scaleMode; }

private:
    void OnPaint(wxPaintEvent& event);

    wxBitmap m_bitmap;
    ScaleMode m_scaleMode;

    wxDECLARE_DYNAMIC_CLASS(wxGenericStaticBitmap);
};


#endif //_WX_GENERIC_STATBMP_H_
