///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/toplevel.h
// Purpose:     wxTopLevelWindowMSW is the MSW implementation of wxTLW
// Author:      Vadim Zeitlin
// Created:     20.09.01
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_TOPLEVEL_H_
#define _WX_MSW_TOPLEVEL_H_

#include "wx/weakref.h"

// ----------------------------------------------------------------------------
// wxTopLevelWindowMSW
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTopLevelWindowMSW : public wxTopLevelWindowBase
{
public:
    // constructors and such
    wxTopLevelWindowMSW() { Init(); }

    wxTopLevelWindowMSW(wxWindow *parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_FRAME_STYLE,
                        const wxString& name = wxASCII_STR(wxFrameNameStr))
    {
        Init();

        (void)Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    virtual ~wxTopLevelWindowMSW();

    // implement base class pure virtuals
    virtual void SetTitle( const wxString& title) override;
    virtual wxString GetTitle() const override;
    virtual void Maximize(bool maximize = true) override;
    virtual bool IsMaximized() const override;
    virtual void Iconize(bool iconize = true) override;
    virtual bool IsIconized() const override;
    virtual void SetIcons(const wxIconBundle& icons ) override;
    virtual void Restore() override;
    virtual bool Destroy() override;

    virtual void SetLayoutDirection(wxLayoutDirection dir) override;

    virtual void RequestUserAttention(int flags = wxUSER_ATTENTION_INFO) override;

    virtual bool Show(bool show = true) override;
    virtual void Raise() override;

    virtual void ShowWithoutActivating() override;
    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL) override;
    virtual bool IsFullScreen() const override { return m_fsIsShowing; }

    virtual wxContentProtection GetContentProtection() const override;
    virtual bool SetContentProtection(wxContentProtection contentProtection) override;

    // wxMSW only: EnableCloseButton(false) may be used to remove the "Close"
    // button from the title bar
    virtual bool EnableCloseButton(bool enable = true) override;
    virtual bool EnableMaximizeButton(bool enable = true) override;
    virtual bool EnableMinimizeButton(bool enable = true) override;

    // Set window transparency if the platform supports it
    virtual bool SetTransparent(wxByte alpha) override;
    virtual bool CanSetTransparent() override;


    // MSW-specific methods
    // --------------------

    // Return the menu representing the "system" menu of the window. You can
    // call wxMenu::AppendWhatever() methods on it but removing items from it
    // is in general not a good idea.
    //
    // The pointer returned by this method belongs to the window and will be
    // deleted when the window itself is, do not delete it yourself. May return
    // nullptr if getting the system menu failed.
    wxMenu *MSWGetSystemMenu() const;

    // Enable or disable the close button of the specified window.
    static bool MSWEnableCloseButton(WXHWND hwnd, bool enable = true);


    // implementation from now on
    // --------------------------

    // event handlers
    void OnActivate(wxActivateEvent& event);

    // called from wxWidgets code itself only when the pending focus, i.e. the
    // element which should get focus when this TLW is activated again, changes
    virtual void WXSetPendingFocus(wxWindow* win) override
    {
        m_winLastFocused = win;
    }

    // translate wxWidgets flags to Windows ones
    virtual WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle) const override;

    // choose the right parent to use with CreateWindow()
    virtual WXHWND MSWGetParent() const override;

    // window proc for the frames
    virtual WXLRESULT MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam) override;

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const override { return false; }

    // This function is only for internal use.
    void MSWSetShowCommand(WXUINT showCmd) { m_showCmd = showCmd; }

protected:
    // common part of all ctors
    void Init();

    // create a new frame, return false if it couldn't be created
    bool CreateFrame(const wxString& title,
                     const wxPoint& pos,
                     const wxSize& size);

    // create a new dialog using the given dialog template from resources,
    // return false if it couldn't be created
    bool CreateDialog(const void *dlgTemplate,
                      const wxString& title,
                      const wxPoint& pos,
                      const wxSize& size);

    // Just a wrapper around MSW ShowWindow().
    void DoShowWindow(int nShowCmd);

    // Return true if the window is iconized at MSW level, ignoring m_showCmd.
    bool MSWIsIconized() const;

    // override those to return the normal window coordinates even when the
    // window is minimized
    virtual void DoGetPosition(int *x, int *y) const override;
    virtual void DoGetSize(int *width, int *height) const override;

    // Top level windows have different freeze semantics on Windows
    virtual void DoFreeze() override;
    virtual void DoThaw() override;

    // helper of SetIcons(): calls gets the icon with the size specified by the
    // given system metrics (SM_C{X|Y}[SM]ICON) from the bundle and sets it
    // using WM_SETICON with the specified wParam (ICOM_SMALL or ICON_BIG);
    // returns true if the icon was set
    bool DoSelectAndSetIcon(const wxIconBundle& icons, int smX, int smY, int i);

    // override wxWindow virtual method to use CW_USEDEFAULT if necessary
    virtual void MSWGetCreateWindowCoords(const wxPoint& pos,
                                          const wxSize& size,
                                          int& x, int& y,
                                          int& w, int& h) const override;

    // override this one to update our icon on DPI change (not quite the same
    // thing as font, but close enough...)
    virtual void MSWUpdateFontOnDPIChange(const wxSize& newDPI) override;


    // This field contains the show command to use when showing the window the
    // next time and also indicates whether the window should be considered
    // being iconized or maximized (which may be different from whether it's
    // actually iconized or maximized at MSW level).
    WXUINT m_showCmd;

    // Data to save/restore when calling ShowFullScreen
    long                  m_fsStyle; // Passed to ShowFullScreen
    wxRect                m_fsOldSize;
    long                  m_fsOldWindowStyle;
    bool                  m_fsIsMaximized;
    bool                  m_fsIsShowing;

    // Save the current focus to m_winLastFocused if we're not iconized (the
    // focus is always null when we're iconized).
    void DoSaveLastFocus();

    // Restore focus to m_winLastFocused if possible and needed.
    void DoRestoreLastFocus();

    // The last focused child: we remember it when we're deactivated and
    // restore focus to it when we're activated (this is done here) or restored
    // from iconic state (done by wxFrame).
    wxWindowRef m_winLastFocused;

private:
    // Part of SetIcons() actually updating the window icons.
    void DoSetIcons();


    // The system menu: initially nullptr but can be set (once) by
    // MSWGetSystemMenu(). Owned by this window.
    wxMenu *m_menuSystem;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxTopLevelWindowMSW);
};

#endif // _WX_MSW_TOPLEVEL_H_
