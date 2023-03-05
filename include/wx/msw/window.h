/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/window.h
// Purpose:     wxWindowMSW class
// Author:      Julian Smart
// Modified by: Vadim Zeitlin on 13.05.99: complete refont of message handling,
//              elimination of Default(), ...
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

#include "wx/settings.h"        // solely for wxSystemColour

class WXDLLIMPEXP_FWD_CORE wxButton;

// if this is set to 1, we use deferred window sizing to reduce flicker when
// resizing complicated window hierarchies, but this can in theory result in
// different behaviour than the old code so we keep the possibility to use it
// by setting this to 0 (in the future this should be removed completely)
#define wxUSE_DEFERRED_SIZING 1

// ---------------------------------------------------------------------------
// wxWindow declaration for MSW
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowMSW : public wxWindowBase
{
    friend class wxSpinCtrl;
    friend class wxSlider;
    friend class wxRadioBox;
public:
    wxWindowMSW() { Init(); }

    wxWindowMSW(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxPanelNameStr))
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxWindowMSW();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxPanelNameStr))
    {
        return CreateUsingMSWClass(nullptr, parent, id, pos, size, style, name);
    }

    // Non-portable, MSW-specific Create() variant allowing to create the
    // window with a custom Windows class name. This can be useful to assign a
    // custom Windows class, that can be recognized from the outside of the
    // application, for windows of specific type.
    bool CreateUsingMSWClass(const wxChar* classname,
                             wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos = wxDefaultPosition,
                             const wxSize& size = wxDefaultSize,
                             long style = 0,
                             const wxString& name = wxASCII_STR(wxPanelNameStr));

    // implement base class pure virtuals
    virtual void SetLabel(const wxString& label) override;
    virtual wxString GetLabel() const override;

    virtual void Raise() override;
    virtual void Lower() override;

#if wxUSE_DEFERRED_SIZING
    virtual bool BeginRepositioningChildren() override;
    virtual void EndRepositioningChildren() override;
#endif // wxUSE_DEFERRED_SIZING

    virtual bool Show(bool show = true) override;
    virtual bool ShowWithEffect(wxShowEffect effect,
                                unsigned timeout = 0) override
    {
        return MSWShowWithEffect(true, effect, timeout);
    }
    virtual bool HideWithEffect(wxShowEffect effect,
                                unsigned timeout = 0) override
    {
        return MSWShowWithEffect(false, effect, timeout);
    }

    virtual void SetFocus() override;
    virtual void SetFocusFromKbd() override;

    virtual bool Reparent(wxWindowBase *newParent) override;

    virtual wxSize GetDPI() const override;
    virtual double GetDPIScaleFactor() const override;

    virtual wxSize GetWindowBorderSize() const override;

    virtual void WarpPointer(int x, int y) override;
    virtual bool EnableTouchEvents(int eventsMask) override;

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = nullptr ) override;
    virtual void Update() override;

    virtual void SetWindowStyleFlag(long style) override;
    virtual void SetExtraStyle(long exStyle) override;
    virtual bool SetCursor( const wxCursor &cursor ) override;
    virtual bool SetFont( const wxFont &font ) override;

    virtual int GetCharHeight() const override;
    virtual int GetCharWidth() const override;

    virtual void SetScrollbar( int orient, int pos, int thumbVisible,
                               int range, bool refresh = true ) override;
    virtual void SetScrollPos( int orient, int pos, bool refresh = true ) override;
    virtual int GetScrollPos( int orient ) const override;
    virtual int GetScrollThumb( int orient ) const override;
    virtual int GetScrollRange( int orient ) const override;
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = nullptr ) override;

    virtual bool ScrollLines(int lines) override;
    virtual bool ScrollPages(int pages) override;

    virtual void SetLayoutDirection(wxLayoutDirection dir) override;
    virtual wxLayoutDirection GetLayoutDirection() const override;
    virtual wxCoord AdjustForLayoutDirection(wxCoord x,
                                             wxCoord width,
                                             wxCoord widthTotal) const override;

    virtual void SetId(wxWindowID winid) override;

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget ) override;

    // Accept files for dragging
    virtual void DragAcceptFiles(bool accept) override;
#endif // wxUSE_DRAG_AND_DROP

#ifndef __WXUNIVERSAL__
    // Native resource loading (implemented in src/msw/nativdlg.cpp)
    // FIXME: should they really be all virtual?
    virtual bool LoadNativeDialog(wxWindow* parent, wxWindowID id);
    virtual bool LoadNativeDialog(wxWindow* parent, const wxString& name);
    wxWindow* GetWindowChild1(wxWindowID id);
    wxWindow* GetWindowChild(wxWindowID id);
#endif // __WXUNIVERSAL__

#if wxUSE_HOTKEY
    // install and deinstall a system wide hotkey
    virtual bool RegisterHotKey(int hotkeyId, int modifiers, int keycode) override;
    virtual bool UnregisterHotKey(int hotkeyId) override;
#endif // wxUSE_HOTKEY

    // window handle stuff
    // -------------------

    WXHWND GetHWND() const { return m_hWnd; }
    void SetHWND(WXHWND hWnd) { m_hWnd = hWnd; }
    virtual WXWidget GetHandle() const override { return GetHWND(); }

    void AssociateHandle(WXWidget handle) override;
    void DissociateHandle() override;

    // returns the handle of the native window to focus when this wxWindow gets
    // focus  (i.e. in composite windows: by default, this is just the HWND for
    // this window itself, but it can be overridden to return something
    // different for composite controls
    virtual WXHWND MSWGetFocusHWND() const { return GetHWND(); }

    // does this window have deferred position and/or size?
    bool IsSizeDeferred() const;

    // these functions allow to register a global handler for the given Windows
    // message: it will be called from MSWWindowProc() of any window which gets
    // this event if it's not processed before (i.e. unlike a hook procedure it
    // does not override the normal processing)
    //
    // notice that if you want to process a message for a given window only you
    // should override its MSWWindowProc() instead

    // type of the handler: it is called with the message parameters (except
    // that the window object is passed instead of window handle) and should
    // return true if it handled the message or false if it should be passed to
    // DefWindowProc()
    typedef bool (*MSWMessageHandler)(wxWindowMSW *win,
                                      WXUINT nMsg,
                                      WXWPARAM wParam,
                                      WXLPARAM lParam);

    // install a handler, shouldn't be called more than one for the same message
    static bool MSWRegisterMessageHandler(int msg, MSWMessageHandler handler);

    // unregister a previously registered handler
    static void MSWUnregisterMessageHandler(int msg, MSWMessageHandler handler);


    // implementation from now on
    // ==========================

    // event handlers
    // --------------

    void OnPaint(wxPaintEvent& event);

    // Override this to return true to automatically invert the window colours
    // in dark mode.
    //
    // This doesn't result in visually great results, but may still be better
    // than using light background.
    virtual bool MSWShouldUseAutoDarkMode() const { return false; }

public:
    // Windows subclassing
    void SubclassWin(WXHWND hWnd);
    void UnsubclassWin();

    WXWNDPROC MSWGetOldWndProc() const { return m_oldWndProc; }
    void MSWSetOldWndProc(WXWNDPROC proc) { m_oldWndProc = proc; }

    // return true if the window is of a standard (i.e. not wxWidgets') class
    //
    // to understand why does it work, look at SubclassWin() code and comments
    bool IsOfStandardClass() const { return m_oldWndProc != nullptr; }

    wxWindow *FindItem(long id, WXHWND hWnd = nullptr) const;
    wxWindow *FindItemByHWND(WXHWND hWnd, bool controlOnly = false) const;

    // MSW only: true if this control is part of the main control
    virtual bool ContainsHWND(WXHWND WXUNUSED(hWnd)) const { return false; }

#if wxUSE_TOOLTIPS
    // MSW only: true if this window or any of its children have a tooltip
    virtual bool HasToolTips() const { return GetToolTip() != nullptr; }
#endif // wxUSE_TOOLTIPS

    // translate wxWidgets style flags for this control into the Windows style
    // and optional extended style for the corresponding native control
    //
    // this is the function that should be overridden in the derived classes,
    // but you will mostly use MSWGetCreateWindowFlags() below
    virtual WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle = nullptr) const ;

    // get the MSW window flags corresponding to wxWidgets ones
    //
    // the functions returns the flags (WS_XXX) directly and puts the ext
    // (WS_EX_XXX) flags into the provided pointer if not null
    WXDWORD MSWGetCreateWindowFlags(WXDWORD *exflags = nullptr) const
        { return MSWGetStyle(GetWindowStyle(), exflags); }

    // update the real underlying window style flags to correspond to the
    // current wxWindow object style (safe to call even if window isn't fully
    // created yet)
    void MSWUpdateStyle(long flagsOld, long exflagsOld);

    // get the HWND to be used as parent of this window with CreateWindow()
    virtual WXHWND MSWGetParent() const;

    // Return the name of the Win32 class that should be used by this wxWindow
    // object, taking into account wxFULL_REPAINT_ON_RESIZE style (if it's not
    // specified, the wxApp::GetNoRedrawClassSuffix()-suffixed version of the
    // class is used).
    static const wxChar *GetMSWClassName(long style);

    // creates the window of specified Windows class with given style, extended
    // style, title and geometry (default values
    //
    // returns true if the window has been created, false if creation failed
    bool MSWCreate(const wxChar *wclass,
                   const wxChar *title = nullptr,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   WXDWORD style = 0,
                   WXDWORD exendedStyle = 0);

    virtual bool MSWCommand(WXUINT param, WXWORD id);

#ifndef __WXUNIVERSAL__
    // Create an appropriate wxWindow from a HWND
    virtual wxWindow* CreateWindowFromHWND(wxWindow* parent, WXHWND hWnd);

    // Make sure the window style reflects the HWND style (roughly)
    virtual void AdoptAttributesFromHWND();
#endif // __WXUNIVERSAL__

    // Setup background and foreground colours correctly
    virtual void SetupColours();

    // ------------------------------------------------------------------------
    // helpers for message handlers: these perform the same function as the
    // message crackers from <windowsx.h> - they unpack WPARAM and LPARAM into
    // the correct parameters
    // ------------------------------------------------------------------------

    void UnpackCommand(WXWPARAM wParam, WXLPARAM lParam,
                       WXWORD *id, WXHWND *hwnd, WXWORD *cmd);
    void UnpackActivate(WXWPARAM wParam, WXLPARAM lParam,
                        WXWORD *state, WXWORD *minimized, WXHWND *hwnd);
    void UnpackScroll(WXWPARAM wParam, WXLPARAM lParam,
                      WXWORD *code, WXWORD *pos, WXHWND *hwnd);
    void UnpackCtlColor(WXWPARAM wParam, WXLPARAM lParam,
                        WXHDC *hdc, WXHWND *hwnd);
    void UnpackMenuSelect(WXWPARAM wParam, WXLPARAM lParam,
                          WXWORD *item, WXWORD *flags, WXHMENU *hmenu);

    // ------------------------------------------------------------------------
    // internal handlers for MSW messages: all handlers return a boolean value:
    // true means that the handler processed the event and false that it didn't
    // ------------------------------------------------------------------------

    // there are several cases where we have virtual functions for Windows
    // message processing: this is because these messages often require to be
    // processed in a different manner in the derived classes. For all other
    // messages, however, we do *not* have corresponding MSWOnXXX() function
    // and if the derived class wants to process them, it should override
    // MSWWindowProc() directly.

    // scroll event (both horizontal and vertical)
    virtual bool MSWOnScroll(int orientation, WXWORD nSBCode,
                             WXWORD pos, WXHWND control);

    // child control notifications
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);

    // owner-drawn controls need to process these messages
    virtual bool MSWOnDrawItem(int id, WXDRAWITEMSTRUCT *item);
    virtual bool MSWOnMeasureItem(int id, WXMEASUREITEMSTRUCT *item);

    // the rest are not virtual
    bool HandleCreate(WXLPCREATESTRUCT cs, bool *mayCreate);
    bool HandleInitDialog(WXHWND hWndFocus);
    bool HandleDestroy();

    bool HandlePaint();
    bool HandlePrintClient(WXHDC hDC);
    bool HandleEraseBkgnd(WXHDC hDC);

    bool HandleMinimize();
    bool HandleMaximize();
    bool HandleSize(int x, int y, WXUINT flag);
    bool HandleSizing(wxRect& rect);
    bool HandleGetMinMaxInfo(void *mmInfo);
    bool HandleEnterSizeMove();
    bool HandleExitSizeMove();

    bool HandleShow(bool show, int status);
    bool HandleActivate(int flag, bool minimized, WXHWND activate);

    bool HandleCommand(WXWORD id, WXWORD cmd, WXHWND control);

    bool HandleCtlColor(WXHBRUSH *hBrush, WXHDC hdc, WXHWND hWnd);

    bool HandlePaletteChanged(WXHWND hWndPalChange);
    bool HandleQueryNewPalette();
    bool HandleSysColorChange();
    bool HandleDisplayChange();
    bool HandleCaptureChanged(WXHWND gainedCapture);
    virtual bool HandleSettingChange(WXWPARAM wParam, WXLPARAM lParam);

    bool HandleQueryEndSession(long logOff, bool *mayEnd);
    bool HandleEndSession(bool endSession, long logOff);

    bool HandleSetFocus(WXHWND wnd);
    bool HandleKillFocus(WXHWND wnd);

    bool HandleDropFiles(WXWPARAM wParam);

    bool HandleMouseEvent(WXUINT msg, int x, int y, WXUINT flags);
    bool HandleMouseMove(int x, int y, WXUINT flags);
    bool HandleMouseWheel(wxMouseWheelAxis axis,
                          WXWPARAM wParam, WXLPARAM lParam);

    // Common gesture event initialization, returns true if it is the initial
    // event (GF_BEGIN set in flags), false otherwise.
    bool InitGestureEvent(wxGestureEvent& event, const wxPoint& pt, WXDWORD flags);

    bool HandlePanGesture(const wxPoint& pt, WXDWORD flags);
    bool HandleZoomGesture(const wxPoint& pt, WXDWORD fingerDistance, WXDWORD flags);
    bool HandleRotateGesture(const wxPoint& pt, WXDWORD angleArgument, WXDWORD flags);
    bool HandleTwoFingerTap(const wxPoint& pt, WXDWORD flags);
    bool HandlePressAndTap(const wxPoint& pt, WXDWORD flags);

    bool HandleChar(WXWPARAM wParam, WXLPARAM lParam);
    bool HandleKeyDown(WXWPARAM wParam, WXLPARAM lParam);
    bool HandleKeyUp(WXWPARAM wParam, WXLPARAM lParam);
#if wxUSE_HOTKEY
    bool HandleHotKey(WXWPARAM wParam, WXLPARAM lParam);
#endif
    int HandleMenuChar(int chAccel, WXLPARAM lParam);
    // Create and process a clipboard event specified by type.
    bool HandleClipboardEvent( WXUINT nMsg );

    bool HandleQueryDragIcon(WXHICON *hIcon);

    bool HandleSetCursor(WXHWND hWnd, short nHitTest, int mouseMsg);

    bool HandlePower(WXWPARAM wParam, WXLPARAM lParam, bool *vetoed);


    // The main body of common window proc for all wxWindow objects. It tries
    // to handle the given message and returns true if it was handled (the
    // appropriate return value is then put in result, which must be non-null)
    // or false if it wasn't.
    //
    // This function should be overridden in any new code instead of
    // MSWWindowProc() even if currently most of the code overrides
    // MSWWindowProc() as it had been written before this function was added.
    virtual bool MSWHandleMessage(WXLRESULT *result,
                                  WXUINT message,
                                  WXWPARAM wParam,
                                  WXLPARAM lParam);

    // Common Window procedure for all wxWindow objects: forwards to
    // MSWHandleMessage() and MSWDefWindowProc() if the message wasn't handled.
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    // Calls an appropriate default window procedure
    virtual WXLRESULT MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    // message processing helpers

    // return false if the message shouldn't be translated/preprocessed but
    // dispatched normally
    virtual bool MSWShouldPreProcessMessage(WXMSG* pMsg);

    // return true if the message was preprocessed and shouldn't be dispatched
    virtual bool MSWProcessMessage(WXMSG* pMsg);

    // return true if the message was translated and shouldn't be dispatched
    virtual bool MSWTranslateMessage(WXMSG* pMsg);

    // called when the window is about to be destroyed
    virtual void MSWDestroyWindow();


    // Functions dealing with painting the window background. The derived
    // classes should normally only need to reimplement MSWGetBgBrush() if they
    // need to use a non-solid brush for erasing their background. This
    // function is called by MSWGetBgBrushForChild() which only exists for the
    // weird wxToolBar case and MSWGetBgBrushForChild() itself is used by
    // MSWGetBgBrush() to actually find the right brush to use.

    // Adjust the origin for the brush returned by MSWGetBgBrushForChild().
    //
    // This needs to be overridden for scrolled windows to ensure that the
    // scrolling of their associated DC is taken into account.
    //
    // Both parameters must be non-null.
    virtual void MSWAdjustBrushOrg(int* WXUNUSED(xOrg),
                                   int* WXUNUSED(yOrg)) const
    {
    }

    // The brush returned from here must remain valid at least until the next
    // event loop iteration. Returning 0, as is done by default, indicates
    // there is no custom background brush.
    virtual WXHBRUSH MSWGetCustomBgBrush() { return nullptr; }

    // this function should return the brush to paint the children controls
    // background or 0 if this window doesn't impose any particular background
    // on its children
    //
    // the hDC parameter is the DC background will be drawn on, it can be used
    // to call SetBrushOrgEx() on it if the returned brush is a bitmap one
    //
    // child parameter is never null, it can be this window itself or one of
    // its (grand)children
    //
    // the base class version returns a solid brush if we have a non default
    // background colour or 0 otherwise
    virtual WXHBRUSH MSWGetBgBrushForChild(WXHDC hDC, wxWindowMSW *child);

    // return the background brush to use for painting the given window by
    // querying the parent windows via MSWGetBgBrushForChild() recursively
    WXHBRUSH MSWGetBgBrush(WXHDC hDC);

    // gives the parent the possibility to draw its children background, e.g.
    // this is used by wxNotebook to do it using DrawThemeBackground()
    //
    // return true if background was drawn, false otherwise
    virtual bool MSWPrintChild(WXHDC WXUNUSED(hDC), wxWindow * WXUNUSED(child))
    {
        return false;
    }

    // some controls (e.g. wxListBox) need to set the return value themselves
    //
    // return true to let parent handle it if we don't, false otherwise
    virtual bool MSWShouldPropagatePrintChild()
    {
        return true;
    }

    // This should be overridden to return true for the controls which have
    // themed background that should through their children. Currently only
    // wxNotebook uses this.
    //
    // The base class version already returns true if we have a solid
    // background colour that should be propagated to our children.
    virtual bool MSWHasInheritableBackground() const
    {
        return InheritsBackgroundColour();
    }

#if !defined(__WXUNIVERSAL__)
    #define wxHAS_MSW_BACKGROUND_ERASE_HOOK
#endif

#ifdef wxHAS_MSW_BACKGROUND_ERASE_HOOK
    // allows the child to hook into its parent WM_ERASEBKGND processing: call
    // MSWSetEraseBgHook() with a non-null window to make parent call
    // MSWEraseBgHook() on this window (don't forget to reset it to nullptr
    // afterwards)
    //
    // this hack is used by wxToolBar, see comments there
    void MSWSetEraseBgHook(wxWindow *child);

    // return true if WM_ERASEBKGND is currently hooked
    bool MSWHasEraseBgHook() const;

    // called when the window on which MSWSetEraseBgHook() had been called
    // receives WM_ERASEBKGND
    virtual bool MSWEraseBgHook(WXHDC WXUNUSED(hDC)) { return false; }
#endif // wxHAS_MSW_BACKGROUND_ERASE_HOOK

    // common part of Show/HideWithEffect()
    bool MSWShowWithEffect(bool show,
                           wxShowEffect effect,
                           unsigned timeout);

    // Responds to colour changes: passes event on to children.
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // initialize various fields of wxMouseEvent (common part of MSWOnMouseXXX)
    void InitMouseEvent(wxMouseEvent& event, int x, int y, WXUINT flags);

    // check if mouse is in the window
    bool IsMouseInWindow() const;

    virtual void SetDoubleBuffered(bool on) override;
    virtual bool IsDoubleBuffered() const override;

    // Ensure that neither this window itself nor any of its parents use
    // WS_EX_COMPOSITED: this is used by the native wxListCtrl which is
    // incompatible with this style.
    void MSWDisableComposited();

    // This function is called for all child windows when compositing is
    // disabled for their parent.
    virtual void MSWOnDisabledComposited() { }

    // synthesize a wxEVT_LEAVE_WINDOW event and set m_mouseInWindow to false
    void GenerateMouseLeave();

#if wxUSE_MENUS && !defined(__WXUNIVERSAL__)
    virtual bool HandleMenuSelect(WXWORD nItem, WXWORD nFlags, WXHMENU hMenu);

    // handle WM_(UN)INITMENUPOPUP message to generate wxEVT_MENU_OPEN/CLOSE
    bool HandleMenuPopup(wxEventType evtType, WXHMENU hMenu);

    // Command part of HandleMenuPopup() and HandleExitMenuLoop().
    virtual bool DoSendMenuOpenCloseEvent(wxEventType evtType, wxMenu* menu);

    // Find the menu corresponding to the given handle.
    virtual wxMenu* MSWFindMenuFromHMENU(WXHMENU hMenu);
#endif // wxUSE_MENUS && !__WXUNIVERSAL__

    // Return the default button for the TLW containing this window or nullptr if
    // none.
    static wxButton* MSWGetDefaultButtonFor(wxWindow* win);

    // Simulate a click on the given button if it is non-null, enabled and
    // shown.
    //
    // Return true if the button was clicked, false otherwise.
    static bool MSWClickButtonIfPossible(wxButton* btn);

    // This method is used for handling wxRadioButton-related complications,
    // see wxRadioButton::SetValue().
    //
    // It should be overridden by all classes storing the "last focused"
    // window to avoid focusing an unset radio button when regaining focus.
    virtual void WXSetPendingFocus(wxWindow* WXUNUSED(win)) {}

    // Called from WM_DPICHANGED handler for all windows to let them update
    // any sizes and fonts used internally when the DPI changes and generate
    // wxDPIChangedEvent to let the user code do the same thing as well.
    //
    // Return true if the event was processed, false otherwise.
    bool MSWUpdateOnDPIChange(const wxSize& oldDPI, const wxSize& newDPI);

protected:
    virtual void WXAdjustFontToOwnPPI(wxFont& font) const override;

    // Called from MSWUpdateOnDPIChange() specifically to update the control
    // font, as this may need to be done differently for some specific native
    // controls. The default version updates m_font of this window.
    virtual void MSWUpdateFontOnDPIChange(const wxSize& newDPI);

    // this allows you to implement standard control borders without
    // repeating the code in different classes that are not derived from
    // wxControl
    virtual wxBorder GetDefaultBorderForControl() const override;

    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const override;

    // Translate wxBORDER_THEME (and other border styles if necessary to the value
    // that makes most sense for this Windows environment
    virtual wxBorder TranslateBorder(wxBorder border) const;

#if wxUSE_MENUS_NATIVE
    virtual bool DoPopupMenu( wxMenu *menu, int x, int y ) override;
#endif // wxUSE_MENUS_NATIVE

    // Called by Reparent() after the window parent changes, i.e. GetParent()
    // returns the new parent inside this function.
    virtual void MSWAfterReparent();


    // the window handle
    WXHWND                m_hWnd;

    // the old window proc (we subclass all windows)
    WXWNDPROC             m_oldWndProc;

    // additional (MSW specific) flags
    bool                  m_mouseInWindow:1;
    bool                  m_lastKeydownProcessed:1;

    // the size of one page for scrolling
    int                   m_xThumbSize;
    int                   m_yThumbSize;

    // implement the base class pure virtuals
    virtual void DoGetTextExtent(const wxString& string,
                                 int *x, int *y,
                                 int *descent = nullptr,
                                 int *externalLeading = nullptr,
                                 const wxFont *font = nullptr) const override;
    static void MSWDoClientToScreen( WXHWND hWnd, int *x, int *y );
    static void MSWDoScreenToClient( WXHWND hWnd, int *x, int *y );
    virtual void DoClientToScreen( int *x, int *y ) const override;
    virtual void DoScreenToClient( int *x, int *y ) const override;
    virtual void DoGetPosition( int *x, int *y ) const override;
    virtual void DoGetSize( int *width, int *height ) const override;
    virtual void DoGetClientSize( int *width, int *height ) const override;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) override;
    virtual void DoSetClientSize(int width, int height) override;

    virtual void DoCaptureMouse() override;
    virtual void DoReleaseMouse() override;

    virtual void DoEnable(bool enable) override;

    virtual void DoFreeze() override;
    virtual void DoThaw() override;

    // this simply moves/resizes the given HWND which is supposed to be our
    // sibling (this is useful for controls which are composite at MSW level
    // and for which DoMoveWindow() is not enough)
    //
    // returns true if the window move was deferred, false if it was moved
    // immediately (no error return)
    bool DoMoveSibling(WXHWND hwnd, int x, int y, int width, int height);

    // move the window to the specified location and resize it: this is called
    // from both DoSetSize() and DoSetClientSize() and would usually just call
    // ::MoveWindow() except for composite controls which will want to arrange
    // themselves inside the given rectangle
    virtual void DoMoveWindow(int x, int y, int width, int height) override;

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip ) override;

    // process TTN_NEEDTEXT message properly (i.e. fixing the bugs in
    // comctl32.dll in our code -- see the function body for more info)
    bool HandleTooltipNotify(WXUINT code,
                             WXLPARAM lParam,
                             const wxString& ttip);
#endif // wxUSE_TOOLTIPS

    // This is used by CreateKeyEvent() and also for wxEVT_CHAR[_HOOK] event
    // creation. Notice that this method doesn't initialize wxKeyEvent
    // m_keyCode and m_uniChar fields.
    void InitAnyKeyEvent(wxKeyEvent& event,
                         WXWPARAM wParam,
                         WXLPARAM lParam) const;

    // Helper functions used by HandleKeyXXX() methods and some derived
    // classes, wParam and lParam have the same meaning as in WM_KEY{DOWN,UP}.
    //
    // NB: evType here must be wxEVT_KEY_{DOWN,UP} as wParam here contains the
    //     virtual key code, not character!
    wxKeyEvent CreateKeyEvent(wxEventType evType,
                              WXWPARAM wParam,
                              WXLPARAM lParam = 0) const;

    // Another helper for creating wxKeyEvent for wxEVT_CHAR and related types.
    //
    // The wParam and lParam here must come from WM_CHAR event parameters, i.e.
    // wParam must be a character and not a virtual code.
    wxKeyEvent CreateCharEvent(wxEventType evType,
                               WXWPARAM wParam,
                               WXLPARAM lParam) const;


    // default OnEraseBackground() implementation, return true if we did erase
    // the background, false otherwise (i.e. the system should erase it)
    bool DoEraseBackground(WXHDC hDC);

    // generate WM_CHANGEUISTATE if it's needed for the OS we're running under
    //
    // action should be one of the UIS_XXX constants
    // state should be one or more of the UISF_XXX constants
    // if action == UIS_INITIALIZE then it doesn't seem to matter what we use
    // for state as the system will decide for us what needs to be set
    void MSWUpdateUIState(int action, int state = 0);

    // translate wxWidgets coords into Windows ones suitable to be passed to
    // ::CreateWindow(), called from MSWCreate()
    virtual void MSWGetCreateWindowCoords(const wxPoint& pos,
                                          const wxSize& size,
                                          int& x, int& y,
                                          int& w, int& h) const;

    bool MSWEnableHWND(WXHWND hWnd, bool enable);

    // Return the pointer to this window or one of its sub-controls if this ID
    // and HWND combination belongs to one of them.
    //
    // This is used by FindItem() and is overridden in wxControl, see there.
    virtual wxWindow* MSWFindItem(long WXUNUSED(id), WXHWND WXUNUSED(hWnd)) const
    {
        return nullptr;
    }

private:
    // common part of all ctors
    void Init();

    // the (non-virtual) handlers for the events
    bool HandleMove(int x, int y);
    bool HandleMoving(wxRect& rect);
    bool HandleJoystickEvent(WXUINT msg, int x, int y, WXUINT flags);
    bool HandleNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);

#ifndef __WXUNIVERSAL__
    // Call ::IsDialogMessage() if it is safe to do it (i.e. if it's not going
    // to hang or do something else stupid) with the given message, return true
    // if the message was handled by it.
    bool MSWSafeIsDialogMessage(WXMSG* msg);
#endif // __WXUNIVERSAL__

    static inline bool MSWIsPositionDirectlySupported(int x, int y)
    {
        // The supported coordinate intervals for various functions are:
        // - MoveWindow, DeferWindowPos: [-32768, 32767] a.k.a. [SHRT_MIN, SHRT_MAX];
        // - CreateWindow, CreateWindowEx: [-32768, 32554].
        // CreateXXX will _sometimes_ manage to create the window at higher coordinates
        // like 32580, 32684, 32710, but that was not consistent and the lowest common
        // limit was 32554 (so far at least).
        return (x >= SHRT_MIN && x <= 32554 && y >= SHRT_MIN && y <= 32554);
    }

protected:
    WXHWND MSWCreateWindowAtAnyPosition(WXDWORD exStyle, const wxChar* clName,
                                        const wxChar* title, WXDWORD style,
                                        int x, int y, int width, int height,
                                        WXHWND parent, wxWindowID id);

    void MSWMoveWindowToAnyPosition(WXHWND hwnd, int x, int y,
                                    int width, int height, bool bRepaint);

#if wxUSE_DEFERRED_SIZING
    // this function is called after the window was resized to its new size
    virtual void MSWEndDeferWindowPos()
    {
        m_pendingPosition = wxDefaultPosition;
        m_pendingSize = wxDefaultSize;
    }

    // current defer window position operation handle (may be null)
    WXHANDLE m_hDWP;

    // When deferred positioning is done these hold the pending changes, and
    // are used for the default values if another size/pos changes is done on
    // this window before the group of deferred changes is completed.
    wxPoint     m_pendingPosition;
    wxSize      m_pendingSize;
#endif // wxUSE_DEFERRED_SIZING

private:
    wxDECLARE_DYNAMIC_CLASS(wxWindowMSW);
    wxDECLARE_NO_COPY_CLASS(wxWindowMSW);
    wxDECLARE_EVENT_TABLE();
};

// window creation helper class: before creating a new HWND, instantiate an
// object of this class on stack - this allows to process the messages sent to
// the window even before CreateWindow() returns
class wxWindowCreationHook
{
public:
    wxWindowCreationHook(wxWindowMSW *winBeingCreated);
    ~wxWindowCreationHook();
};

#endif // _WX_WINDOW_H_
