/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/statbmp.h
// Purpose:     wxStaticBitmap class for wxMSW
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBMP_H_
#define _WX_STATBMP_H_

#include "wx/control.h"
#include "wx/icon.h"
#include "wx/bitmap.h"

extern WXDLLIMPEXP_DATA_CORE(const char) wxStaticBitmapNameStr[];

// a control showing an icon or a bitmap
class WXDLLIMPEXP_CORE wxStaticBitmap : public wxStaticBitmapBase
{
public:
    wxStaticBitmap() { Init(); }

    wxStaticBitmap(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmapBundle& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxASCII_STR(wxStaticBitmapNameStr))
    {
        Init();

        Create(parent, id, label, pos, size, style, name);
    }

    wxStaticBitmap(wxWindow *parent,
                   wxWindowID id,
                   const wxIcon& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxASCII_STR(wxStaticBitmapNameStr))
    {
        Init();

        Create(parent, id, label, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmapBundle& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBitmapNameStr))
    {
        m_bitmapBundle = label;

        return DoCreate(parent, id, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxIcon& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBitmapNameStr))
    {
        m_icon = label;

        return DoCreate(parent, id, pos, size, style, name);
    }

    virtual ~wxStaticBitmap() { Free(); }

    virtual void SetIcon(const wxIcon& icon) override;
    virtual void SetBitmap(const wxBitmapBundle& bitmap) override;
    virtual wxBitmap GetBitmap() const override;
    virtual wxIcon GetIcon() const override;

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const override { return false; }

protected:
    virtual wxSize DoGetBestClientSize() const override;

private:
    // ctor/dtor helpers
    void Init();
    void Free();

    // common part of both Create() overloads
    bool DoCreate(wxWindow *parent,
                  wxWindowID id,
                  const wxPoint& pos,
                  const wxSize& size,
                  long style,
                  const wxString& name);

    // update the image to correspond to the current m_icon or m_bitmapBundle
    // value, resize the control if the new size is different from the old one
    // and update its style if the new "is icon" value differs from the old one
    void DoUpdateImage(const wxSize& sizeOld, bool wasIcon);

    // draw the bitmap ourselves here if the OS can't do it correctly (if it
    // can we leave it to it)
    void DoPaintManually(wxPaintEvent& event);

    // event handlers
    void WXHandleSize(wxSizeEvent& event);
    void WXHandleDPIChanged(wxDPIChangedEvent& event);

    // return the size of m_icon or the appropriate size of m_bitmapBundle at
    // the current DPI scaling (may still be invalid of both of them are)
    wxSize GetImageSize() const;


    // we can have either an icon or a bitmap: if m_icon is valid, it is used,
    // otherwise we use m_bitmap which is itself obtained from m_bitmapBundle
    // defined in the base class
    wxIcon m_icon;
    wxBitmap m_bitmap;

    // handle used in last call to STM_SETIMAGE
    WXHANDLE m_currentHandle;

    // Flag indicating whether we own m_currentHandle, i.e. should delete it.
    bool m_ownsCurrentHandle;

    // Replace the image at the native control level with the given HBITMAP or
    // HICON (which can be 0) and destroy the previous image if necessary.
    void MSWReplaceImageHandle(WXHANDLE handle);


    wxDECLARE_DYNAMIC_CLASS(wxStaticBitmap);
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxStaticBitmap);
};

#endif
    // _WX_STATBMP_H_
