///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/popupwin.h
// Purpose:     wxPopupWindow class for wxMSW
// Author:      Vadim Zeitlin
// Created:     06.01.01
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_POPUPWIN_H_
#define _WX_MSW_POPUPWIN_H_

// ----------------------------------------------------------------------------
// wxPopupWindow
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPopupWindow : public wxPopupWindowBase
{
public:
    wxPopupWindow() { m_owner = nullptr; }

    wxPopupWindow(wxWindow *parent, int flags = wxBORDER_NONE)
        { (void)Create(parent, flags); }

    bool Create(wxWindow *parent, int flags = wxBORDER_NONE);

    virtual ~wxPopupWindow();

#if defined(__WXWINUI__) && wxUSE_WINUI3
    virtual bool Destroy() override;
#endif
    virtual void SetFocus() override;
    virtual bool Show(bool show = true) override;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    virtual bool Reparent(wxWindowBase *newParent) override;
#endif

    // return the style to be used for the popup windows
    virtual WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle) const override;

    // get the HWND to be used as parent of this window with CreateWindow()
    virtual WXHWND MSWGetParent() const override;


    // Implementation only from now on.

    // Return the top level window parent of this popup or null.
    wxWindow* MSWGetOwner() const { return m_owner; }

    // This is a way to notify non-wxPU_CONTAINS_CONTROLS windows about the
    // events that should result in their dismissal.
    virtual void MSWDismissUnfocusedPopup() { }

private:
    wxWindow* m_owner;

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // WinUI-only state is held in a cpp sidecar to preserve this exported
    // class's historical size and vtable.
    friend class wxWinUIPopupSidecarAccess;
#endif

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxPopupWindow);
};

#endif // _WX_MSW_POPUPWIN_H_
