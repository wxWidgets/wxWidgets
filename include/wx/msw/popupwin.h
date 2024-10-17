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

    virtual void SetFocus() override;
    virtual bool Show(bool show = true) override;

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

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxPopupWindow);
};

#endif // _WX_MSW_POPUPWIN_H_
