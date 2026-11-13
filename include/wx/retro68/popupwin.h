///////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/popupwin.h
// Purpose:     wxPopupWindow class for classic Mac OS
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_POPUPWIN_H_
#define _WX_RETRO68_POPUPWIN_H_

// ----------------------------------------------------------------------------
// wxPopupWindow
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPopupWindow : public wxPopupWindowBase
{
public:
    wxPopupWindow() = default;

    wxPopupWindow(wxWindow *parent, int flags = wxBORDER_NONE)
        { Create(parent, flags); }

    bool Create(wxWindow *parent, int flags = wxBORDER_NONE)
    {
        if ( !wxPopupWindowBase::Create(parent) )
            return false;

        return wxNonOwnedWindow::Create
               (
                 parent,
                 -1,
                 // native windows must have valid pos & size:
                 wxPoint(0, 0), wxSize(1, 1),
                 (flags & wxBORDER_MASK) | wxPOPUP_WINDOW
               );
    }

    // popup windows are not activated when clicked
    virtual bool ShouldActivateOnClick() const override { return false; }

    virtual short GetWindowProcID() const override;

    wxDECLARE_DYNAMIC_CLASS(wxPopupWindow);
};

#endif // _WX_RETRO68_POPUPWIN_H_
