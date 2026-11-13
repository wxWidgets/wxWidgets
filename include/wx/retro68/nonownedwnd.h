///////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/nonownedwnd.h
// Purpose:     wxNonOwnedWindow class for classic Mac OS
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_NONOWNEDWND_H_
#define _WX_RETRO68_NONOWNEDWND_H_

#include "wx/window.h"

// Toolbox window type, declared in Windows.h
struct GrafPort;

// ---------------------------------------------------------------------------
// wxNonOwnedWindow: common base class of wxTopLevelWindow and wxPopupWindow
// corresponding to a native Window Manager window.
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxNonOwnedWindow : public wxNonOwnedWindowBase
{
public:
    wxNonOwnedWindow() { Init(); }

    wxNonOwnedWindow(wxWindow *parent,
                     wxWindowID id,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = 0,
                     const wxString& name = wxASCII_STR(wxPanelNameStr))
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxPanelNameStr));

    virtual ~wxNonOwnedWindow();

    // implement base class pure virtuals
    virtual bool Show(bool show = true) override;

    virtual void Update() override;

    virtual void Raise() override;
    virtual void Lower() override;

    virtual void SetLabel(const wxString& label) override;

    // implementation from now on
    // --------------------------

    // Return the native window.
    GrafPort* GetWindowRef() const { return m_macWindow; }

    // Return the native window port (which is the same as the window itself
    // for classic Toolbox but not under Carbon).
    GrafPort* GetPort() const;

    // Paint the invalidated parts of the window, called in response to an
    // update event.
    void HandleUpdateEvent();

    // Called in response to activate events.
    virtual void HandleActivateEvent(bool activate);

    // Return true if this window is currently being painted from
    // HandleUpdateEvent().
    bool IsPainting() const { return m_isPainting; }

    // Return true if clicking this window should activate it (which is the
    // case for all windows except popups).
    virtual bool ShouldActivateOnClick() const { return true; }

    // Return true if this window can be moved by dragging its title bar or
    // resized using its size box.
    bool IsMovable() const;
    bool IsResizable() const;

    // Called when the user resized the window using its grow box or by
    // zooming it.
    void HandleUserResize();

    // Called when the user moved the window by dragging its title bar.
    void HandleUserMove();

    // Return the native window procedure ID to use for this window.
    virtual short GetWindowProcID() const;

    // Window coordinates of TLWs correspond to the native port coordinates,
    // which are the coordinates of the window content region.
    virtual void DoRefreshRect(const wxRect& rect) override;

    virtual void OnInternalIdle() override;

protected:
    void Init();

    virtual void DoGetPosition(int *x, int *y) const override;
    virtual void DoGetSize(int *width, int *height) const override;
    virtual void DoGetClientSize(int *width, int *height) const override;
    virtual void DoSetClientSize(int width, int height) override;
    virtual void DoMoveWindow(int x, int y, int width, int height) override;
    virtual wxPoint GetScreenPosOfClientOrigin() const override;

    // Return the size of the native decorations around the content area.
    wxRect GetDecorationsSize() const;

    // Update the native title of the window.
    void SetNativeTitle(const wxString& title);

    // Draw the size box in the bottom right corner if necessary.
    void DrawSizeBox();

    GrafPort *m_macWindow;

private:
    bool m_isPainting;

    // true if wxSizeEvent was already sent to the window.
    bool m_sizeSet;
};

#endif // _WX_RETRO68_NONOWNEDWND_H_
