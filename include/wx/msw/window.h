/////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:     wxWindow class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "window.h"
#endif

// #include "wx/msw/winundef.h"

// VZ: apparently some version of Windows send extra mouse move messages after
//     a mouse click. My tests under NT 4.0 and 95 didn't show it so I'm
//     tempted to think that it was just an effect of a poor mouse and so the
//     code to work around this is currently disabled - just define this as 1
//     to reenable it
#define wxUSE_MOUSEEVENT_HACK 0

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxButton;

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

// FIXME does anybody use those? they're unused by wxWindows...
enum
{
    wxKEY_SHIFT = 1,
    wxKEY_CTRL  = 2
};

// ---------------------------------------------------------------------------
// wxWindow declaration for MSW
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxWindow : public wxWindowBase
{
public:
    wxWindow() { Init(); }

    wxWindow(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxPanelNameStr)
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxWindow();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr);

    // implement base class pure virtuals
    virtual void SetTitle( const wxString& title);
    virtual wxString GetTitle() const;

    virtual void Raise();
    virtual void Lower();

    virtual bool Show( bool show = TRUE );
    virtual bool Enable( bool enable = TRUE );

    virtual void SetFocus();

    virtual bool Reparent( wxWindow *newParent );

    virtual void WarpPointer(int x, int y);
    virtual void CaptureMouse();
    virtual void ReleaseMouse();

    virtual void Refresh( bool eraseBackground = TRUE,
                          const wxRect *rect = (const wxRect *) NULL );
    virtual void Clear();

    virtual bool SetCursor( const wxCursor &cursor );
    virtual bool SetFont( const wxFont &font );

    virtual int GetCharHeight() const;
    virtual int GetCharWidth() const;
    virtual void GetTextExtent(const wxString& string,
                               int *x, int *y,
                               int *descent = (int *) NULL,
                               int *externalLeading = (int *) NULL,
                               const wxFont *theFont = (const wxFont *) NULL)
                               const;

    virtual bool DoPopupMenu( wxMenu *menu, int x, int y );

    virtual void SetScrollbar( int orient, int pos, int thumbVisible,
                               int range, bool refresh = TRUE );
    virtual void SetScrollPos( int orient, int pos, bool refresh = TRUE );
    virtual int GetScrollPos( int orient ) const;
    virtual int GetScrollThumb( int orient ) const;
    virtual int GetScrollRange( int orient ) const;
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = (wxRect *) NULL );

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget );
#endif // wxUSE_DRAG_AND_DROP

    // Accept files for dragging
    virtual void DragAcceptFiles(bool accept);

#if WXWIN_COMPATIBILITY
    // Set/get scroll attributes
    virtual void SetScrollRange(int orient, int range, bool refresh = TRUE);
    virtual void SetScrollPage(int orient, int page, bool refresh = TRUE);
    virtual int OldGetScrollRange(int orient) const;
    virtual int GetScrollPage(int orient) const;

    // event handlers
        // Handle a control command
    virtual void OnCommand(wxWindow& win, wxCommandEvent& event);

        // Override to define new behaviour for default action (e.g. double
        // clicking on a listbox)
    virtual void OnDefaultAction(wxControl * WXUNUSED(initiatingItem)) { }
#endif // WXWIN_COMPATIBILITY

#if wxUSE_CARET && WXWIN_COMPATIBILITY
    // caret manipulation (old MSW only functions, see wxCaret class for the
    // new API)
    void CreateCaret(int w, int h);
    void CreateCaret(const wxBitmap *bitmap);
    void DestroyCaret();
    void ShowCaret(bool show);
    void SetCaretPos(int x, int y);
    void GetCaretPos(int *x, int *y) const;
#endif // wxUSE_CARET

    // Native resource loading (implemented in src/msw/nativdlg.cpp)
    // FIXME: should they really be all virtual?
    virtual bool LoadNativeDialog(wxWindow* parent, wxWindowID& id);
    virtual bool LoadNativeDialog(wxWindow* parent, const wxString& name);
    wxWindow* GetWindowChild1(wxWindowID id);
    wxWindow* GetWindowChild(wxWindowID id);

    // a MSW only function which sends a size event to the window using its
    // current size - this has an effect of refreshing the window layout
    void SendSizeEvent();

    // implementation from now on
    // --------------------------

    // simple accessors
    // ----------------

    WXHWND GetHWND() const { return m_hWnd; }
    void SetHWND(WXHWND hWnd) { m_hWnd = hWnd; }
    virtual WXWidget GetHandle() const { return GetHWND(); }

    bool GetUseCtl3D() const { return m_useCtl3D; }
    bool GetTransparentBackground() const { return m_backgroundTransparent; }
    void SetTransparent(bool t = TRUE) { m_backgroundTransparent = t; }

    // event handlers
    // --------------

    void OnSetFocus(wxFocusEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnPaint(wxPaintEvent& event);

public:
    // For implementation purposes - sometimes decorations make the client area
    // smaller
    virtual wxPoint GetClientAreaOrigin() const;

    // Makes an adjustment to the window position (for example, a frame that has
    // a toolbar that it manages itself).
    virtual void AdjustForParentClientOrigin(int& x, int& y, int sizeFlags);

    // Windows subclassing
    void SubclassWin(WXHWND hWnd);
    void UnsubclassWin();

    WXFARPROC MSWGetOldWndProc() const { return m_oldWndProc; }
    void MSWSetOldWndProc(WXFARPROC proc) { m_oldWndProc = proc; }

    wxWindow *FindItem(long id) const;
    wxWindow *FindItemByHWND(WXHWND hWnd, bool controlOnly = FALSE) const;

    // Make a Windows extended style from the given wxWindows window style
    static WXDWORD MakeExtendedStyle(long style,
                                     bool eliminateBorders = FALSE);
    // Determine whether 3D effects are wanted
    WXDWORD Determine3DEffects(WXDWORD defaultBorderStyle, bool *want3D) const;

    // MSW only: TRUE if this control is part of the main control
    virtual bool ContainsHWND(WXHWND WXUNUSED(hWnd)) const { return FALSE; };

    // returns TRUE if the window has been created
    bool MSWCreate(int id,
                   wxWindow *parent,
                   const wxChar *wclass,
                   wxWindow *wx_win,
                   const wxChar *title,
                   int x, int y, int width, int height,
                   WXDWORD style,
                   const wxChar *dialog_template = NULL,
                   WXDWORD exendedStyle = 0);
    virtual bool MSWCommand(WXUINT param, WXWORD id);

#if WXWIN_COMPATIBILITY
    wxObject *GetChild(int number) const;
    virtual void MSWDeviceToLogical(float *x, float *y) const;
#endif // WXWIN_COMPATIBILITY

    // Create an appropriate wxWindow from a HWND
    virtual wxWindow* CreateWindowFromHWND(wxWindow* parent, WXHWND hWnd);

    // Make sure the window style reflects the HWND style (roughly)
    virtual void AdoptAttributesFromHWND();

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
                        WXWORD *nCtlColor, WXHDC *hdc, WXHWND *hwnd);
    void UnpackMenuSelect(WXWPARAM wParam, WXLPARAM lParam,
                          WXWORD *item, WXWORD *flags, WXHMENU *hmenu);

    // ------------------------------------------------------------------------
    // internal handlers for MSW messages: all handlers return a boolen value:
    // TRUE means that the handler processed the event and FALSE that it didn't
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
#ifdef __WIN95__
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
#endif // __WIN95__

    // owner-drawn controls need to process these messages
    virtual bool MSWOnDrawItem(int id, WXDRAWITEMSTRUCT *item);
    virtual bool MSWOnMeasureItem(int id, WXMEASUREITEMSTRUCT *item);

    // the rest are not virtual
    bool HandleCreate(WXLPCREATESTRUCT cs, bool *mayCreate);
    bool HandleInitDialog(WXHWND hWndFocus);
    bool HandleDestroy();

    bool HandlePaint();
    bool HandleEraseBkgnd(WXHDC pDC);

    bool HandleMinimize();
    bool HandleMaximize();
    bool HandleSize(int x, int y, WXUINT flag);
    bool HandleGetMinMaxInfo(void *mmInfo);

    bool HandleShow(bool show, int status);
    bool HandleActivate(int flag, bool minimized, WXHWND activate);

    bool HandleCommand(WXWORD id, WXWORD cmd, WXHWND control);
    bool HandleSysCommand(WXWPARAM wParam, WXLPARAM lParam);

    bool HandleCtlColor(WXHBRUSH *hBrush,
                        WXHDC hdc,
                        WXHWND hWnd,
                        WXUINT nCtlColor,
                        WXUINT message,
                        WXWPARAM wParam,
                        WXLPARAM lParam);

    bool HandlePaletteChanged(WXHWND hWndPalChange);
    bool HandleQueryNewPalette();
    bool HandleSysColorChange();

    bool HandleQueryEndSession(long logOff, bool *mayEnd);
    bool HandleEndSession(bool endSession, long logOff);

    bool HandleSetFocus(WXHWND wnd);
    bool HandleKillFocus(WXHWND wnd);

    bool HandleDropFiles(WXWPARAM wParam);

    bool HandleMouseEvent(WXUINT msg, int x, int y, WXUINT flags);
    bool HandleMouseMove(int x, int y, WXUINT flags);

    bool HandleChar(WXWORD wParam, WXLPARAM lParam, bool isASCII = FALSE);
    bool HandleKeyDown(WXWORD wParam, WXLPARAM lParam);
    bool HandleKeyUp(WXWORD wParam, WXLPARAM lParam);

    bool HandleQueryDragIcon(WXHICON *hIcon);

    bool HandleSetCursor(WXHWND hWnd, short nHitTest, int mouseMsg);

    // Window procedure
    virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    // Calls an appropriate default window procedure
    virtual long MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
    virtual bool MSWProcessMessage(WXMSG* pMsg);
    virtual bool MSWTranslateMessage(WXMSG* pMsg);
    virtual void MSWDestroyWindow();

    // Detach "Window" menu from menu bar so it doesn't get deleted
    void MSWDetachWindowMenu();

    // this function should return the brush to paint the window background
    // with or 0 for the default brush
    virtual WXHBRUSH OnCtlColor(WXHDC hDC,
                                WXHWND hWnd,
                                WXUINT nCtlColor,
                                WXUINT message,
                                WXWPARAM wParam,
                                WXLPARAM lParam);

#if WXWIN_COMPATIBILITY
    void SetShowing(bool show) { (void)Show(show); }
    bool IsUserEnabled() const { return IsEnabled(); }
#endif // WXWIN_COMPATIBILITY

    // Responds to colour changes: passes event on to children.
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // initialize various fields of wxMouseEvent (common part of MSWOnMouseXXX)
    void InitMouseEvent(wxMouseEvent& event, int x, int y, WXUINT flags);

protected:
    // the window handle
    WXHWND                m_hWnd;

    // the old window proc (we subclass all windows)
    WXFARPROC             m_oldWndProc;

    // additional (MSW specific) flags
    bool                  m_useCtl3D:1; // Using CTL3D for this control
    bool                  m_backgroundTransparent:1;
    bool                  m_mouseInWindow:1;
    bool                  m_doubleClickAllowed:1;
    bool                  m_winCaptured:1;

    // the size of one page for scrolling
    int                   m_xThumbSize;
    int                   m_yThumbSize;

#if wxUSE_MOUSEEVENT_HACK
    // the coordinates of the last mouse event and the type of it
    long                  m_lastMouseX,
                          m_lastMouseY;
    int                   m_lastMouseEvent;
#endif // wxUSE_MOUSEEVENT_HACK

    WXHMENU               m_hMenu; // Menu, if any

    // the return value of WM_GETDLGCODE handler
    long m_lDlgCode;

    // implement the base class pure virtuals
    virtual void DoClientToScreen( int *x, int *y ) const;
    virtual void DoScreenToClient( int *x, int *y ) const;
    virtual void DoGetPosition( int *x, int *y ) const;
    virtual void DoGetSize( int *width, int *height ) const;
    virtual void DoGetClientSize( int *width, int *height ) const;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual void DoSetClientSize(int width, int height);

    // move the window to the specified location and resize it: this is called
    // from both DoSetSize() and DoSetClientSize() and would usually just call
    // ::MoveWindow() except for composite controls which will want to arrange
    // themselves inside the given rectangle
    virtual void DoMoveWindow(int x, int y, int width, int height);

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );
#endif // wxUSE_TOOLTIPS

private:
    // common part of all ctors
    void Init();

    // the (non-virtual) handlers for the events
    bool HandleMove(int x, int y);
    bool HandleJoystickEvent(WXUINT msg, int x, int y, WXUINT flags);

#ifdef __WIN95__
    bool HandleNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
#endif // __WIN95__

    // the helper functions used by HandleChar/KeyXXX methods
    wxKeyEvent CreateKeyEvent(wxEventType evType, int id, WXLPARAM lp) const;

    DECLARE_DYNAMIC_CLASS(wxWindow);
    DECLARE_NO_COPY_CLASS(wxWindow);
    DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

// kbd code translation
WXDLLEXPORT int wxCharCodeMSWToWX(int keySym);
WXDLLEXPORT int wxCharCodeWXToMSW(int id, bool *IsVirtual);

#endif
    // _WX_WINDOW_H_
