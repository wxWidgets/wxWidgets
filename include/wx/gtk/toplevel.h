/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/toplevel.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_TOPLEVEL_H_
#define _WX_GTK_TOPLEVEL_H_

class WXDLLIMPEXP_FWD_CORE wxGUIEventLoop;

//-----------------------------------------------------------------------------
// wxTopLevelWindowGTK
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTopLevelWindowGTK : public wxTopLevelWindowBase
{
    typedef wxTopLevelWindowBase base_type;
public:
    // construction
    wxTopLevelWindowGTK() { Init(); }
    wxTopLevelWindowGTK(wxWindow *parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_FRAME_STYLE,
                        const wxString& name = wxASCII_STR(wxFrameNameStr))
    {
        Init();

        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    virtual ~wxTopLevelWindowGTK();

    // implement base class pure virtuals
    virtual void Maximize(bool maximize = true) override;
    virtual bool IsMaximized() const override;
    virtual void Iconize(bool iconize = true) override;
    virtual bool IsIconized() const override;
    virtual void SetIcons(const wxIconBundle& icons) override;
    virtual void Restore() override;

    virtual bool EnableCloseButton(bool enable = true) override;

    virtual void ShowWithoutActivating() override;
    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL) override;
    virtual bool IsFullScreen() const override { return m_fsIsShowing; }

    virtual void RequestUserAttention(int flags = wxUSER_ATTENTION_INFO) override;

    virtual void SetWindowStyleFlag( long style ) override;

    virtual bool Show(bool show = true) override;

    virtual void Raise() override;

    virtual bool IsActive() override;

    virtual void SetTitle( const wxString &title ) override;
    virtual wxString GetTitle() const override { return m_title; }

    virtual void SetLabel(const wxString& label) override { SetTitle( label ); }
    virtual wxString GetLabel() const override            { return GetTitle(); }

    virtual wxVisualAttributes GetDefaultAttributes() const override;

    virtual bool SetTransparent(wxByte alpha) override;
    virtual bool CanSetTransparent() override;

    // Experimental, to allow help windows to be
    // viewable from within modal dialogs
    virtual void AddGrab();
    virtual void RemoveGrab();
    virtual bool IsGrabbed() const;


    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) nullptr ) override;

    // implementation from now on
    // --------------------------

    // GTK callbacks
    virtual void GTKHandleRealized() override;

    void GTKConfigureEvent(int x, int y);

    // do *not* call this to iconize the frame, this is a private function!
    void SetIconizeState(bool iconic);

    GtkWidget    *m_mainWidget;

    bool          m_fsIsShowing;         /* full screen */
    int           m_fsSaveGdkFunc, m_fsSaveGdkDecor;
    wxRect        m_fsSaveFrame;

    // m_windowStyle translated to GDK's terms
    int           m_gdkFunc,
                  m_gdkDecor;

    // size of WM decorations
    struct DecorSize
    {
        DecorSize()
        {
            left =
            right =
            top =
            bottom = 0;
        }

        int left, right, top, bottom;
    };
    DecorSize m_decorSize;

    // private gtk_timeout_add result for mimicking wxUSER_ATTENTION_INFO and
    // wxUSER_ATTENTION_ERROR difference, -2 for no hint, -1 for ERROR hint, rest for GtkTimeout handle.
    int m_urgency_hint;
    // timer for detecting WM with broken _NET_REQUEST_FRAME_EXTENTS handling
    unsigned m_netFrameExtentsTimerId;

    // return the size of the window without WM decorations
    void GTKDoGetSize(int *width, int *height) const;

    void GTKUpdateDecorSize(const DecorSize& decorSize);

    void GTKDoAfterShow();

#ifdef __WXGTK3__
    void GTKUpdateClientSizeIfNecessary();

    virtual void SetMinSize(const wxSize& minSize) override;

    virtual void WXSetInitialFittingClientSize(int flags, wxSizer* sizer = nullptr) override;

private:
    // Flags to call WXSetInitialFittingClientSize() with if != 0.
    int m_pendingFittingClientSizeFlags;
#endif // __WXGTK3__

protected:
    // give hints to the Window Manager for how the size
    // of the TLW can be changed by dragging
    virtual void DoSetSizeHints( int minW, int minH,
                                 int maxW, int maxH,
                                 int incW, int incH) override;
    // move the window to the specified location and resize it
    virtual void DoMoveWindow(int x, int y, int width, int height) override;

    // take into account WM decorations here
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) override;

    virtual void DoSetClientSize(int width, int height) override;
    virtual void DoGetClientSize(int *width, int *height) const override;

    // string shown in the title bar
    wxString m_title;

    bool m_deferShow;

private:
    void Init();
    DecorSize& GetCachedDecorSize();

    // size hint increments
    int m_incWidth, m_incHeight;

    // position before it last changed
    wxPoint m_lastPos;

    // is the frame currently iconized?
    bool m_isIconized;

    // is the frame currently grabbed explicitly by the application?
    wxGUIEventLoop* m_grabbedEventLoop;

    bool m_updateDecorSize;
    bool m_deferShowAllowed;
};

#endif // _WX_GTK_TOPLEVEL_H_
