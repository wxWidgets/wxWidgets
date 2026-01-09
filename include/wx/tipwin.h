///////////////////////////////////////////////////////////////////////////////
// Name:        wx/tipwin.h
// Purpose:     wxTipWindow is a window like the one typically used for
//              showing the tooltips
// Author:      Vadim Zeitlin
// Created:     10.09.00
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIPWIN_H_
#define _WX_TIPWIN_H_

#include "wx/defs.h"

#if wxUSE_TIPWINDOW

#include "wx/popupwin.h"

class WXDLLIMPEXP_FWD_CORE wxTipWindowView;

// ----------------------------------------------------------------------------
// wxTipWindow
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTipWindow : public wxPopupTransientWindow
{
public:
    // wxTipWindow may close itself, so provide a smart pointer that acts as a
    // weak reference to wxTipWindow.
    //
    // Note that this is a move-only type (because wxTipWindow::Close() only
    // sets a single wxTipWindow* to nullptr).
    //
    // Note that this is not a wxWeakRef<> because this is set to nullptr when
    // wxTipWindow is closed, which may be "long" before wxTipWindow is
    // destroyed, but wxWeakRef<> is set to nullptr on object destruction
    class WXDLLIMPEXP_CORE Ref
    {
    public:
        Ref() = default;
        ~Ref();
        Ref(const Ref&) = delete;
        Ref(Ref&& other);
        Ref& operator=(const Ref&) = delete;
        Ref& operator=(Ref&& other);

        Ref& operator=(std::nullptr_t);

        bool operator!=(std::nullptr_t) const { return m_ptr != nullptr; }
        explicit operator bool() const { return m_ptr; }
        wxTipWindow* operator->() const { return m_ptr; }

    private:
        wxTipWindow* m_ptr = nullptr;

        friend wxTipWindow;
    };

    // replace the deprecated single-step constructor
    // see Create() for parameters
    static Ref New(wxWindow *parent,
                const wxString& text,
                wxCoord maxLength = 100,
                wxRect *rectBound = nullptr);

    wxTipWindow();

    wxDEPRECATED_MSG("Using this has a race condition; use New() instead")
    wxTipWindow(wxWindow *parent,
                const wxString& text,
                wxCoord maxLength = 100,
                wxTipWindow** windowPtr = nullptr,
                wxRect *rectBound = nullptr)
    { (void)Create(parent, text, maxLength, windowPtr, rectBound); }

    // the mandatory parameters are: the parent window and the text to
    // show
    //
    // optionally you may also specify the length at which the lines are going
    // to be broken in rows (100 pixels by default)
    //
    // windowPtr and rectBound are just passed to SetTipWindowPtr() and
    // SetBoundingRect() - see below
    bool Create(wxWindow *parent,
                const wxString& text,
                wxCoord maxLength = 100,
                wxTipWindow** windowPtr = nullptr,
                wxRect *rectBound = nullptr);

    virtual ~wxTipWindow();

    // If windowPtr is not null the given address will be nulled when the
    // window has closed
    void SetTipWindowPtr(wxTipWindow** windowPtr) { m_windowPtr = windowPtr; }

    // If rectBound is not null, the window will disappear automatically when
    // the mouse leave the specified rect: note that rectBound should be in the
    // screen coordinates!
    void SetBoundingRect(const wxRect& rectBound);

    // Hide and destroy the window
    void Close();

protected:
    // called by wxTipWindowView only
    bool CheckMouseInBounds(const wxPoint& pos);

    // event handlers
    void OnMouseClick(wxMouseEvent& event);

    virtual void OnDismiss() override;

private:
    wxTipWindowView *m_view;

    wxTipWindow** m_windowPtr;
    wxRect m_rectBound;

    wxDECLARE_EVENT_TABLE();

    friend class wxTipWindowView;

    wxDECLARE_NO_COPY_CLASS(wxTipWindow);
};

#endif // wxUSE_TIPWINDOW

#endif // _WX_TIPWIN_H_
