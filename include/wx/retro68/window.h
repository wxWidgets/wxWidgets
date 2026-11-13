/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/window.h
// Purpose:     wxWindowRetro68 class
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_WINDOW_H_
#define _WX_RETRO68_WINDOW_H_

// ---------------------------------------------------------------------------
// wxWindow
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxFont;
class WXDLLIMPEXP_FWD_CORE wxNonOwnedWindow;
class WXDLLIMPEXP_FWD_CORE wxKeyEvent;
class WXDLLIMPEXP_FWD_CORE wxMouseEvent;

// Toolbox event structure, declared in Events.h
struct EventRecord;

// Classic Mac OS only has top level windows, all the other wxWindows are
// "lightweight" windows drawn directly on the port of the top level window
// containing them. This is very similar to the way wxDFB works.
class WXDLLIMPEXP_CORE wxWindowRetro68 : public wxWindowBase
{
public:
    wxWindowRetro68() { Init(); }

    wxWindowRetro68(wxWindow *parent,
                    wxWindowID id,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxASCII_STR(wxPanelNameStr))
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxWindowRetro68();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxPanelNameStr));

    // implement base class (pure) virtual methods
    // -------------------------------------------

    virtual void SetLabel(const wxString& label) override { m_label = label; }
    virtual wxString GetLabel() const override { return m_label; }

    virtual void Raise() override;
    virtual void Lower() override;

    virtual bool Show(bool show = true) override;

    virtual void SetFocus() override;

    virtual bool Reparent(wxWindowBase *newParent) override;

    virtual void WarpPointer(int x, int y) override;

    virtual void Refresh(bool eraseBackground = true,
                         const wxRect *rect = nullptr) override;
    virtual void Update() override;

    virtual bool SetCursor(const wxCursor &cursor) override;
    virtual bool SetFont(const wxFont &font) override { m_font = font; return true; }

    virtual int GetCharHeight() const override;
    virtual int GetCharWidth() const override;

    virtual WXWidget GetHandle() const override { return this; }

    // implementation from now on
    // --------------------------

    // Return the top level window containing this one.
    wxNonOwnedWindow *GetTLW() const { return m_tlw; }

    // Return the position of the window, i.e. the top left corner of its
    // non-client area, in the coordinates of the native port of the TLW.
    wxPoint GetPortPosition() const;

    // Return the part of the window, in window coordinates, which is not
    // hidden by its parent borders.
    wxRect GetUncoveredRect() const;

    // Find the deepest child window (or this window itself) containing the
    // given point, which is in this window coordinates.
    wxWindow* FindDeepestChildAt(const wxPoint& pt);

    // Generate and send the keyboard event for this window, return true if
    // it was processed.
    bool HandleKeyEvent(const EventRecord& event, bool isKeyUp);

    // Paint the given part (in window coordinates) of this window and its
    // children.
    void PaintWindow(const wxRect& rect);

    // Refresh the given rectangle, in window coordinates, of this window.
    virtual void DoRefreshRect(const wxRect& rect);

    // Called by wxNonOwnedWindow to update the mouse cursor.
    void UpdateCursor();

protected:
    virtual void DoGetTextExtent(const wxString& string,
                                 int *x, int *y,
                                 int *descent = nullptr,
                                 int *externalLeading = nullptr,
                                 const wxFont *theFont = nullptr) const override;
    virtual void DoClientToScreen(int *x, int *y) const override;
    virtual void DoScreenToClient(int *x, int *y) const override;

    // Return the screen position of the origin of the coordinates used by
    // DoScreenToClient() and DoClientToScreen().
    wxPoint GetScreenOriginForConversion() const;
    virtual void DoGetPosition(int *x, int *y) const override;
    virtual void DoGetSize(int *width, int *height) const override;
    virtual void DoGetClientSize(int *width, int *height) const override;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) override;
    virtual void DoSetClientSize(int width, int height) override;

    virtual void DoCaptureMouse() override;
    virtual void DoReleaseMouse() override;

    virtual void DoThaw() override;

    // move the window to the specified location and resize it: this is called
    // from both DoSetSize() and DoSetClientSize() and would usually just call
    // ::MoveWindow() except for composite controls which will want to arrange
    // themselves inside the given rectangle
    virtual void DoMoveWindow(int x, int y, int width, int height) override;

    // return the screen position of the client area origin
    virtual wxPoint GetScreenPosOfClientOrigin() const;

    // refresh the entire window
    void DoRefreshWindow();

private:
    void Init();

    // called when the window loses focus
    void KillFocus();

protected:
    // top-level window containing this one
    wxNonOwnedWindow *m_tlw;

    // window rectangle, relative to the parent client area
    wxRect m_rect;

private:
    wxString m_label;

    friend class wxNonOwnedWindow;

    wxDECLARE_DYNAMIC_CLASS(wxWindowRetro68);
    wxDECLARE_NO_COPY_CLASS(wxWindowRetro68);
    wxDECLARE_EVENT_TABLE();
};

#endif // _WX_RETRO68_WINDOW_H_
