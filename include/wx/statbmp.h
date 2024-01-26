/////////////////////////////////////////////////////////////////////////////
// Name:        wx/statbmp.h
// Purpose:     wxStaticBitmap class interface
// Author:      Vadim Zeitlin
// Created:     25.08.00
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBMP_H_BASE_
#define _WX_STATBMP_H_BASE_

#include "wx/defs.h"

#if wxUSE_STATBMP

#include "wx/control.h"
#include "wx/bmpbndl.h"
#include "wx/icon.h"

extern WXDLLIMPEXP_DATA_CORE(const char) wxStaticBitmapNameStr[];

// a control showing an icon or a bitmap
class WXDLLIMPEXP_CORE wxStaticBitmapBase : public wxControl
{
public:
    enum ScaleMode
    {
        Scale_None,
        Scale_Fill,
        Scale_AspectFit,
        Scale_AspectFill
    };

    wxStaticBitmapBase() = default;
    virtual ~wxStaticBitmapBase();

    // our interface
    virtual void SetBitmap(const wxBitmapBundle& bitmap) = 0;
    virtual wxBitmap GetBitmap() const;

    virtual void SetIcon(const wxIcon& icon);
    virtual wxIcon GetIcon() const;

    virtual void SetScaleMode(ScaleMode WXUNUSED(scaleMode)) { }
    virtual ScaleMode GetScaleMode() const { return Scale_None; }

    // overridden base class virtuals
    virtual bool AcceptsFocus() const override { return false; }
    virtual bool HasTransparentBackground() override { return true; }

protected:
    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    virtual wxSize DoGetBestSize() const override;

    // Bitmap bundle passed to ctor or SetBitmap().
    wxBitmapBundle m_bitmapBundle;

    wxDECLARE_NO_COPY_CLASS(wxStaticBitmapBase);
};

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/statbmp.h"
#elif defined(__WXMSW__)
    #include "wx/msw/statbmp.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/statbmp.h"
#elif defined(__WXMAC__)
    #include "wx/osx/statbmp.h"
#elif defined(__WXQT__)
    #include "wx/qt/statbmp.h"
#endif

#endif // wxUSE_STATBMP

#endif
    // _WX_STATBMP_H_BASE_
