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

// windows.h #defines the following identifiers which are also used in wxWin
#ifdef GetCharWidth
    #undef GetCharWidth
#endif

#ifdef FindWindow
    #undef FindWindow
#endif

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
    DECLARE_DYNAMIC_CLASS(wxWindow);

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

    virtual void ClientToScreen( int *x, int *y ) const;
    virtual void ScreenToClient( int *x, int *y ) const;

    virtual bool PopupMenu( wxMenu *menu, int x, int y );

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
#endif // WXWIN_COMPATIBILITY

    // caret manipulation (MSW only)
    virtual void CreateCaret(int w, int h);
    virtual void CreateCaret(const wxBitmap *bitmap);
    virtual void DestroyCaret();
    virtual void ShowCaret(bool show);
    virtual void SetCaretPos(int x, int y);
    virtual void GetCaretPos(int *x, int *y) const;

    // event handlers (FIXME: shouldn't they be inside WXWIN_COMPATIBILITY?)
        // Handle a control command
    virtual void OnCommand(wxWindow& win, wxCommandEvent& event);

        // Override to define new behaviour for default action (e.g. double
        // clicking on a listbox)
    virtual void OnDefaultAction(wxControl *initiatingItem);

    // Native resource loading (implemented in src/msw/nativdlg.cpp)
    // FIXME: should they really be all virtual?
    virtual bool LoadNativeDialog(wxWindow* parent, wxWindowID& id);
    virtual bool LoadNativeDialog(wxWindow* parent, const wxString& name);
    virtual wxWindow* GetWindowChild1(wxWindowID id);
    virtual wxWindow* GetWindowChild(wxWindowID id);

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
    void OnEraseBackground(wxEraseEvent& event);
    void OnIdle(wxIdleEvent& event);

    // a window may have a default button
    // TODO move into wxPanel and/or wxFrame
    wxButton *GetDefaultItem() const { return m_btnDefault; }
    void SetDefaultItem(wxButton *btn) { m_btnDefault = btn; }

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
    virtual bool MSWCommand(WXUINT param, WXWORD id);

    WXFARPROC MSWGetOldWndProc() const { return m_oldWndProc; }
    void MSWSetOldWndProc(WXFARPROC proc) { m_oldWndProc = proc; }

    virtual wxWindow *FindItem(int id) const;
    virtual wxWindow *FindItemByHWND(WXHWND hWnd, bool controlOnly = FALSE) const ;
    virtual void PreDelete(WXHDC dc);              // Allows system cleanup

    // Make a Windows extended style from the given wxWindows window style
    virtual WXDWORD MakeExtendedStyle(long style, bool eliminateBorders = TRUE);
    // Determine whether 3D effects are wanted
    virtual WXDWORD Determine3DEffects(WXDWORD defaultBorderStyle, bool *want3D);

    // MSW only: TRUE if this control is part of the main control
    virtual bool ContainsHWND(WXHWND WXUNUSED(hWnd)) const { return FALSE; };

    wxObject *GetChild(int number) const ;

    // returns TRUE if the window has been created
    bool MSWCreate(int id,
                   wxWindow *parent,
                   const char *wclass,
                   wxWindow *wx_win,
                   const char *title,
                   int x, int y, int width, int height,
                   WXDWORD style,
                   const char *dialog_template = NULL,
                   WXDWORD exendedStyle = 0);

    // Actually defined in wx_canvs.cc since requires wxCanvas declaration
    virtual void MSWDeviceToLogical(float *x, float *y) const ;

    // Create an appropriate wxWindow from a HWND
    virtual wxWindow* CreateWindowFromHWND(wxWindow* parent, WXHWND hWnd);

    // Make sure the window style reflects the HWND style (roughly)
    virtual void AdoptAttributesFromHWND();

    // Setup background and foreground colours correctly
    virtual void SetupColours();

    // ------------------------------------------------------------------------
    // internal handlers for MSW messages: all handlers return a boolen value:
    // TRUE means that the handler processed the event and FALSE that it didn't
    // ------------------------------------------------------------------------

    // TODO: all this should go away, overriding MSWWindowProc() is enough to
    //       implement this functionality
    virtual bool MSWOnCreate(WXLPCREATESTRUCT cs, bool *mayCreate);
    virtual bool MSWOnPaint();
    virtual bool MSWOnEraseBkgnd(WXHDC pDC);
    virtual bool MSWOnSize(int x, int y, WXUINT flag);

    virtual bool MSWOnQueryDragIcon(WXHICON *hIcon);
    virtual bool MSWOnWindowPosChanging(void *lpPos);

    // both horizontal and vertical
    virtual bool MSWOnScroll(int orientation, WXWORD nSBCode,
                             WXWORD pos, WXHWND control);

    virtual bool MSWOnCommand(WXWORD id, WXWORD cmd, WXHWND control);
    virtual bool MSWOnSysCommand(WXWPARAM wParam, WXLPARAM lParam);

#ifdef __WIN95__
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
#endif // __WIN95__

    virtual bool MSWOnCtlColor(WXHBRUSH *hBrush,
                               WXHDC hdc,
                               WXHWND hWnd,
                               WXUINT nCtlColor,
                               WXUINT message,
                               WXWPARAM wParam,
                               WXLPARAM lParam);

    virtual bool MSWOnPaletteChanged(WXHWND hWndPalChange);
    virtual bool MSWOnQueryNewPalette();

    virtual bool MSWOnQueryEndSession(long logOff, bool *mayEnd);
    virtual bool MSWOnEndSession(bool endSession, long logOff);

    virtual bool MSWOnDestroy();
    virtual bool MSWOnSetFocus(WXHWND wnd);
    virtual bool MSWOnKillFocus(WXHWND wnd);
    virtual bool MSWOnDropFiles(WXWPARAM wParam);
    virtual bool MSWOnInitDialog(WXHWND hWndFocus);
    virtual bool MSWOnShow(bool show, int status);

    virtual bool MSWOnMouseEvent(WXUINT msg, int x, int y, WXUINT flags);
    virtual bool MSWOnMouseMove(int x, int y, WXUINT flags);

    virtual bool MSWOnChar(WXWORD wParam, WXLPARAM lParam, bool isASCII = FALSE);
    virtual bool MSWOnKeyDown(WXWORD wParam, WXLPARAM lParam);
    virtual bool MSWOnKeyUp(WXWORD wParam, WXLPARAM lParam);

    virtual bool MSWOnActivate(int flag, bool minimized, WXHWND activate);
    virtual bool MSWOnMDIActivate(long flag, WXHWND activate, WXHWND deactivate);

    virtual bool MSWOnDrawItem(int id, WXDRAWITEMSTRUCT *item);
    virtual bool MSWOnMeasureItem(int id, WXMEASUREITEMSTRUCT *item);

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

    // Caret data
    bool                  m_caretEnabled:1;
    bool                  m_caretShown:1;
    int                   m_caretWidth;
    int                   m_caretHeight;

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

    wxButton             *m_btnDefault;

    // implement the base class pure virtuals
    virtual void DoGetPosition( int *x, int *y ) const;
    virtual void DoGetSize( int *width, int *height ) const;
    virtual void DoGetClientSize( int *width, int *height ) const;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual void DoSetClientSize(int width, int height);

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
