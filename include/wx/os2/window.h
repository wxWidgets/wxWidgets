/////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:     wxWindow class
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

#define wxUSE_MOUSEEVENT_HACK 0

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------
#define INCL_DOS
#define INCL_PM
#define INCL_GPI
#include <os2.h>


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
// wxWindow declaration for OS/2 PM
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxWindow : public wxWindowBase
{
    DECLARE_DYNAMIC_CLASS(wxWindow);

public:
    wxWindow() { Init(); }

    wxWindow( wxWindow*       pParent
             ,wxWindowID      vId
             ,const wxPoint&  rPos = wxDefaultPosition
             ,const wxSize&   rSize = wxDefaultSize
             ,long            lStyle = 0
             ,const wxString& rName = wxPanelNameStr
            )
    {
        Init();
        Create( pParent
               ,vId
               ,rPos
               ,rSize
               ,lStyle
               ,rName
              );
    }

    virtual ~wxWindow();

    bool Create( wxWindow*       pParent
                ,wxWindowID      vId
                ,const wxPoint&  rPos = wxDefaultPosition
                ,const wxSize&   rSize = wxDefaultSize
                ,long            lStyle = 0
                ,const wxString& rName = wxPanelNameStr
               );

    // implement base class pure virtuals
    virtual void     SetTitle(const wxString& rTitle);
    virtual wxString GetTitle(void) const;
    virtual void     Raise(void);
    virtual void     Lower(void);
    virtual bool     Show(bool bShow = TRUE);
    virtual bool     Enable(bool bEnable = TRUE);
    virtual void     SetFocus(void);
    virtual bool     Reparent(wxWindow* pNewParent);
    virtual void     WarpPointer( int x
                                 ,int y
                                );
    virtual void     CaptureMouse(void);
    virtual void     ReleaseMouse(void);
    virtual void     Refresh( bool          bEraseBackground = TRUE
                             ,const wxRect* pRect = (const wxRect *)NULL
                            );
    virtual void     Clear(void);
    virtual bool     SetCursor(const wxCursor& rCursor);
    virtual bool     SetFont(const wxFont& rFont);
    virtual int      GetCharHeight(void) const;
    virtual int      GetCharWidth(void) const;
    virtual void     GetTextExtent( const wxString& rString
                                   ,int*            pX
                                   ,int*            pY
                                   ,int*            pDescent = (int *)NULL
                                   ,int*            pExternalLeading = (int *)NULL
                                   ,const wxFont*   pTheFont = (const wxFont *)NULL
                                  ) const;
    virtual bool     DoPopupMenu( wxMenu* pMenu
                                 ,int     nX
                                 ,int     nY
                                );

    virtual void     SetScrollbar( int  nOrient
                                  ,int  nPos
                                  ,int  nThumbVisible
                                  ,int  nRange
                                  ,bool bRefresh = TRUE
                                 );
    virtual void     SetScrollPos( int  nOrient
                                  ,int  nPos
                                  ,bool bRefresh = TRUE
                                 );
    virtual int      GetScrollPos(int nOrient) const;
    virtual int      GetScrollThumb(int nOrient) const;
    virtual int      GetScrollRange(int nOrient) const;
    virtual void     ScrollWindow( int           nDx
                                  ,int           nDy
                                  ,const wxRect* pRect = (wxRect *)NULL
                                 );

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget(wxDropTarget* pDropTarget);
#endif // wxUSE_DRAG_AND_DROP

    // Accept files for dragging
    virtual void DragAcceptFiles(bool bAccept);

#if WXWIN_COMPATIBILITY
    // Set/get scroll attributes
    virtual void SetScrollRange( int  nOrient
                                ,int  nRange
                                ,bool bRefresh = TRUE
                               );
    virtual void SetScrollPage( int  nOrient
                               ,int  nPage
                               ,bool bRefresh = TRUE
                              );
    virtual int  OldGetScrollRange(int nOrient) const;
    virtual int  GetScrollPage(int nOrient) const;

    //
    // event handlers
    //
        // Handle a control command
    virtual void OnCommand( wxWindow&       rWin
                           ,wxCommandEvent& rEvent
                          );

        // Override to define new behaviour for default action (e.g. double
        // clicking on a listbox)
    virtual void OnDefaultAction(wxControl* WXUNUSED(pInitiatingItem)) { }
#endif // WXWIN_COMPATIBILITY

#if wxUSE_CARET && WXWIN_COMPATIBILITY
    void CreateCaret( int nWidth
                     ,int nHeight
                    );
    void CreateCaret(const wxBitmap* pBitmap);
    void DestroyCaret(void);
    void ShowCaret(bool bShow);
    void SetCaretPos( int nX
                     ,int nY
                    );
    void GetCaretPos( int* pX
                     ,int* pY
                    ) const;
#endif // wxUSE_CARET

    // Native resource loading (implemented in src/os2/nativdlg.cpp)
    // FIXME: should they really be all virtual?
    virtual bool LoadNativeDialog( wxWindow*   pParent
                                  ,wxWindowID& vId
                                 );
    virtual bool LoadNativeDialog( wxWindow*       pParent
                                  ,const wxString& rName
                                 );
    wxWindow*    GetWindowChild1(wxWindowID vId);
    wxWindow*    GetWindowChild(wxWindowID vId);

    // implementation from now on
    // --------------------------

    // simple accessors
    // ----------------

    WXHWND           GetHWND(void) const { return m_hWnd; }
    void             SetHWND(WXHWND hWnd) { m_hWnd = hWnd; }
    virtual WXWidget GetHandle(void) const { return GetHWND(); }
    bool             GetUseCtl3D(void) const { return m_bUseCtl3D; }
    bool             GetTransparentBackground(void) const { return m_bBackgroundTransparent; }
    void             SetTransparent(bool bT = TRUE) { m_bBackgroundTransparent = bT; }

    // event handlers
    // --------------
    void OnEraseBackground(wxEraseEvent& rEvent);
    void OnIdle(wxIdleEvent& rEvent);

public:
    // For implementation purposes - sometimes decorations make the client area
    // smaller
    virtual wxPoint GetClientAreaOrigin(void) const;

    // Makes an adjustment to the window position (for example, a frame that has
    // a toolbar that it manages itself).
    virtual void AdjustForParentClientOrigin( int& rX
                                             ,int& rY
                                             ,int  nSizeFlags
                                            );

    // Windows subclassing
    void SubclassWin(WXHWND hWnd);
    void UnsubclassWin(void);

    WXFARPROC OS2GetOldWndProc(void) const { return m_fnOldWndProc; }
    void OS2SetOldWndProc(WXFARPROC fnProc) { m_fnOldWndProc = fnProc; }

    wxWindow* FindItem(long lId) const;
    wxWindow* FindItemByHWND( WXHWND hWnd
                             ,bool   bControlOnly = FALSE
                            ) const;

    // Make a Windows extended style from the given wxWindows window style ?? applicable to OS/2??
    static WXDWORD MakeExtendedStyle( long lStyle
                                     ,bool bEliminateBorders = TRUE
                                    );
    // Determine whether 3D effects are wanted
    WXDWORD Determine3DEffects( WXDWORD dwDefaultBorderStyle
                               ,bool*   pbWant3D
                              ) const;

    // PM only: TRUE if this control is part of the main control
    virtual bool ContainsHWND(WXHWND WXUNUSED(hWnd)) const { return FALSE; };

    // returns TRUE if the window has been created
    bool OS2Create( int           nId
                   ,wxWindow*     pParent
                   ,const wxChar* zWclass
                   ,wxWindow*     pWxWin
                   ,const wxChar* zTitle
                   ,int           nX
                   ,int           nY
                   ,int           nWidth
                   ,int           nHeight
                   ,WXDWORD       dwStyle
                   ,const wxChar* zDialogTemplate = NULL
                   ,WXDWORD       dwExendedStyle = 0
                  );
    virtual bool OS2Command( WXUINT uParam
                            ,WXWORD nId
                           );

#if WXWIN_COMPATIBILITY
    wxObject*    GetChild(int nNumber) const;
    virtual void OS2DeviceToLogical( float* pfX
                                    ,float* pfY
                                   ) const;
#endif // WXWIN_COMPATIBILITY

    // Create an appropriate wxWindow from a HWND
    virtual wxWindow* CreateWindowFromHWND( wxWindow* pParent
                                           ,WXHWND    hWnd
                                          );

    // Make sure the window style reflects the HWND style (roughly)
    virtual void AdoptAttributesFromHWND(void);

    // Setup background and foreground colours correctly
    virtual void SetupColours(void);

    // ------------------------------------------------------------------------
    // helpers for message handlers: these perform the same function as the
    // message crackers from <windowsx.h> - they unpack WPARAM and LPARAM into
    // the correct parameters
    // ------------------------------------------------------------------------

    void UnpackCommand( WXWPARAM wParam
                       ,WXLPARAM lParam,
                        WXWORD*  pId
                       ,WXHWND*  pHwnd
                       ,WXWORD*  pCmd
                      );
    void UnpackActivate( WXWPARAM wParam
                        ,WXLPARAM lParam
                        ,WXWORD*  pState
                        ,WXWORD*  pMinimized
                        ,WXHWND*  pHwnd
                       );
    void UnpackScroll( WXWPARAM wParam
                      ,WXLPARAM lParam
                      ,WXWORD*  pCode
                      ,WXWORD*  pPos
                      ,WXHWND*  pHwnd
                     );
    void UnpackCtlColor( WXWPARAM wParam
                        ,WXLPARAM lParam
                        ,WXWORD*  pCtlColor
                        ,WXHDC*   pHdc
                        ,WXHWND*  pHwnd
                       );
    void UnpackMenuSelect( WXWPARAM wParam
                          ,WXLPARAM lParam
                          ,WXWORD*  pTtem
                          ,WXWORD*  pFlags
                          ,WXHMENU* pHmenu
                         );

    // ------------------------------------------------------------------------
    // internal handlers for OS2 messages: all handlers return a boolen value:
    // TRUE means that the handler processed the event and FALSE that it didn't
    // ------------------------------------------------------------------------

    // there are several cases where we have virtual functions for PM
    // message processing: this is because these messages often require to be
    // processed in a different manner in the derived classes. For all other
    // messages, however, we do *not* have corresponding OS2OnXXX() function
    // and if the derived class wants to process them, it should override
    // OS2WindowProc() directly.

    // scroll event (both horizontal and vertical)
    virtual bool OS2OnScroll( int    nOrientation
                             ,WXWORD nSBCode
                             ,WXWORD pos
                             ,WXHWND control
                            );

    virtual bool OS2OnNotify( int       nIdCtrl
                             ,WXLPARAM  lParam
                             ,WXLPARAM* pResult
                            );

    // owner-drawn controls need to process these messages
    virtual bool OS2OnDrawItem( int               nId
                               ,WXDRAWITEMSTRUCT* pItem
                              );
    virtual bool OS2OnMeasureItem( int                  nId
                                  ,WXMEASUREITEMSTRUCT* pItem
                                 );

    // the rest are not virtual
    bool HandleCreate( WXLPCREATESTRUCT vCs
                      ,bool*            pMayCreate
                     );
    bool HandleInitDialog(WXHWND hWndFocus);
    bool HandleDestroy(void);
    bool HandlePaint(void);
    bool HandleEraseBkgnd(WXHDC vDC);
    bool HandleMinimize(void);
    bool HandleMaximize(void);
    bool HandleSize( int    nX
                    ,int    nY
                    ,WXUINT uFlag
                   );
    bool HandleGetMinMaxInfo(void* pMmInfo);
    bool HandleShow( bool bShow
                    ,int  nStatus
                   );
    bool HandleActivate( int    nFlag
                        ,bool   bMinimized
                        ,WXHWND hActivate
                       );
    bool HandleCommand( WXWORD nId
                       ,WXWORD nCmd
                       ,WXHWND hControl
                      );
    bool HandleSysCommand( WXWPARAM wParam
                          ,WXLPARAM lParam
                         );
    bool HandleCtlColor( WXHBRUSH* phBrush
                        ,WXHDC     hDc
                        ,WXHWND    hWnd
                        ,WXUINT    uCtlColor
                        ,WXUINT    uMessage
                        ,WXWPARAM  wParam
                        ,WXLPARAM  lParam
                       );
    bool HandlePaletteChanged(WXHWND hWndPalChange);
    bool HandleQueryNewPalette(void);
    bool HandleSysColorChange(void);
    bool HandleQueryEndSession( long  lLogOff
                               ,bool* pbMayEnd
                              );
    bool HandleEndSession( bool bEndSession
                          ,long lLogOff
                         );
    bool HandleSetFocus(WXHWND hWnd);
    bool HandleKillFocus(WXHWND hWnd);
    bool HandleDropFiles(WXWPARAM wParam);
    bool HandleMouseEvent( WXUINT uMsg
                          ,int    nX
                          ,int    nY
                          ,WXUINT uFlags
                         );
    bool HandleMouseMove( int    nX
                         ,int    nY
                         ,WXUINT uFlags
                        );
    bool HandleChar( WXWORD   wParam
                    ,WXLPARAM lParam
                    ,bool     bIsASCII = FALSE
                   );
    bool HandleKeyDown( WXWORD   wParam
                       ,WXLPARAM lParam
                      );
    bool HandleKeyUp( WXWORD   wParam
                     ,WXLPARAM lParam
                    );
    bool HandleQueryDragIcon(WXHICON* phIcon);
    bool HandleSetCursor( WXHWND hWnd
                         ,short  nHitTest
                         ,int    nMouseMsg
                        );

    // Window procedure
    virtual MRESULT OS2WindowProc( HWND     hwnd
                                  ,WXUINT   uMsg
                                  ,WXWPARAM wParam
                                  ,WXLPARAM lParam
                                 );

    // Calls an appropriate default window procedure
    virtual MRESULT OS2DefWindowProc( HWND     hwnd
                                     ,WXUINT   uMsg
                                     ,WXWPARAM wParam
                                     ,WXLPARAM lParam
                                    );
    virtual bool    OS2ProcessMessage(WXMSG* pMsg);
    virtual bool    OS2TranslateMessage(WXMSG* pMsg);
    virtual void    OS2DestroyWindow(void);

    // Detach "Window" menu from menu bar so it doesn't get deleted
    void OS2DetachWindowMenu(void);

    // this function should return the brush to paint the window background
    // with or 0 for the default brush
    virtual WXHBRUSH OnCtlColor( WXHDC    hDC
                                ,WXHWND   hWnd
                                ,WXUINT   uCtlColor
                                ,WXUINT   uMessage
                                ,WXWPARAM wParam
                                ,WXLPARAM lParam
                               );

#if WXWIN_COMPATIBILITY
    void SetShowing(bool bShow) { (void)Show(show); }
    bool IsUserEnabled(void) const { return IsEnabled(); }
#endif // WXWIN_COMPATIBILITY

    // Responds to colour changes: passes event on to children.
    void OnSysColourChanged(wxSysColourChangedEvent& rEvent);

    // initialize various fields of wxMouseEvent (common part of OS2OnMouseXXX)
    void InitMouseEvent( wxMouseEvent& rEvent
                        ,int           nX
                        ,int           nY
                        ,WXUINT        uFlags
                       );

protected:
    // the window handle
    WXHWND                          m_hWnd;

    // the old window proc (we subclass all windows)
    WXFARPROC                       m_fnOldWndProc;

    // additional (MSW specific) flags
    bool                            m_bUseCtl3D:1; // Using CTL3D for this control
    bool                            m_bBackgroundTransparent:1;
    bool                            m_bMouseInWindow:1;
    bool                            m_bDoubleClickAllowed:1;
    bool                            m_bWinCaptured:1;

    // the size of one page for scrolling
    int                             m_nXThumbSize;
    int                             m_nYThumbSize;

#if wxUSE_MOUSEEVENT_HACK
    // the coordinates of the last mouse event and the type of it
    long                            m_lLastMouseX,
    long                            m_lLastMouseY;
    int                             m_nLastMouseEvent;
#endif // wxUSE_MOUSEEVENT_HACK

    WXHMENU                         m_hMenu; // Menu, if any

    // the return value of WM_GETDLGCODE handler
    long                            m_lDlgCode;

    // implement the base class pure virtuals
    virtual void DoClientToScreen( int* pX
                                  ,int* pY
                                 ) const;
    virtual void DoScreenToClient( int* pX
                                  ,int* pY
                                 ) const;
    virtual void DoGetPosition( int* pX
                               ,int* pY
                              ) const;
    virtual void DoGetSize( int* pWidth
                           ,int* pHeight
                          ) const;
    virtual void DoGetClientSize( int* pWidth
                                 ,int* pHeight
                                ) const;
    virtual void DoSetSize( int nX
                           ,int nY
                           ,int nWidth
                           ,int nHeight
                           ,int nSizeFlags = wxSIZE_AUTO
                          );
    virtual void DoSetClientSize( int nWidth
                                 ,int nHeight
                                );

    // move the window to the specified location and resize it: this is called
    // from both DoSetSize() and DoSetClientSize() and would usually just call
    // ::MoveWindow() except for composite controls which will want to arrange
    // themselves inside the given rectangle
    virtual void DoMoveWindow( int nX
                              ,int nY
                              ,int nWidth
                              ,int nHeight
                             );

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip(wxToolTip* pTip);
#endif // wxUSE_TOOLTIPS

private:
    // common part of all ctors
    void Init(void);

    // the (non-virtual) handlers for the events
    bool HandleMove( int nX
                    ,int nY
                   );
    bool HandleJoystickEvent( WXUINT uMsg
                             ,int    pX
                             ,int    pY
                             ,WXUINT uFlags
                            );

    bool HandleNotify( int       nIdCtrl
                      ,WXLPARAM  lParam
                      ,WXLPARAM* pResult
                     );
    DECLARE_NO_COPY_CLASS(wxWindow);
    DECLARE_EVENT_TABLE()
private:
    // Virtual function hiding supression
    inline virtual bool Reparent(wxWindowBase* pNewParent)
    { return(wxWindowBase::Reparent(pNewParent));};
};

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

// kbd code translation
WXDLLEXPORT int wxCharCodeOS2ToWX(int nKeySym);
WXDLLEXPORT int wxCharCodeWXToOS2( int   nId
                                  ,bool* pbIsVirtual
                                 );
#endif
    // _WX_WINDOW_H_
