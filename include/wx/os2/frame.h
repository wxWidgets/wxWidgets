/////////////////////////////////////////////////////////////////////////////
// Name:        frame.h
// Purpose:     wxFrame class
// Author:      David Webster
// Modified by:
// Created:     10/27/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FRAME_H_
#define _WX_FRAME_H_

//
// Get the default resource ID's for frames
//
#include "wx/os2/wxOs2.h"

class WXDLLEXPORT wxFrame : public wxFrameBase
{
public:
    // construction
    wxFrame() { Init(); }
    wxFrame( wxWindow*       pParent
            ,wxWindowID      vId
            ,const wxString& rsTitle
            ,const wxPoint&  rPos = wxDefaultPosition
            ,const wxSize&   rSize = wxDefaultSize
            ,long            lStyle = wxDEFAULT_FRAME_STYLE
            ,const wxString& rsName = wxFrameNameStr
           )
    {
        Init();

        Create(pParent, vId, rsTitle, rPos, rSize, lStyle, rsName);
    }

    bool Create( wxWindow*       pParent
                ,wxWindowID      vId
                ,const wxString& rsTitle
                ,const wxPoint&  rPos = wxDefaultPosition
                ,const wxSize&   rSize = wxDefaultSize
                ,long            lStyle = wxDEFAULT_FRAME_STYLE
                ,const wxString& rsName = wxFrameNameStr
               );

    virtual ~wxFrame();

    // implement base class pure virtuals
    virtual void Maximize(bool bMaximize = TRUE);
    virtual bool IsMaximized(void) const;
    virtual void Iconize(bool bIconize = TRUE);
    virtual bool IsIconized(void) const;
    virtual void Restore(void);
    virtual void SetMenuBar(wxMenuBar* pMenubar);
    virtual void SetIcon(const wxIcon& rIcon);
    virtual bool ShowFullScreen( bool bShow
                                ,long lStyle = wxFULLSCREEN_ALL
                               );
    virtual bool IsFullScreen(void) const { return m_bFsIsShowing; };


    // implementation only from now on
    // -------------------------------

    // override some more virtuals
    virtual bool Show(bool bShow = TRUE);

    // event handlers
    void OnActivate(wxActivateEvent& rEvent);
    void OnSysColourChanged(wxSysColourChangedEvent& rEvent);

    // Toolbar
#if wxUSE_TOOLBAR
    virtual wxToolBar* CreateToolBar( long            lStyle = wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT
                                     ,wxWindowID      vId = -1
                                     ,const wxString& rsName = wxToolBarNameStr
                                    );

    virtual void       PositionToolBar(void);
#endif // wxUSE_TOOLBAR

    // Status bar
#if wxUSE_STATUSBAR
    virtual wxStatusBar* OnCreateStatusBar( int             nNumber = 1
                                           ,long            lStyle = wxST_SIZEGRIP
                                           ,wxWindowID      vId = 0
                                           ,const wxString& rsName = wxStatusLineNameStr
                                          );
    virtual void PositionStatusBar(void);

    // Hint to tell framework which status bar to use: the default is to use
    // native one for the platforms which support it (Win32), the generic one
    // otherwise

    // TODO: should this go into a wxFrameworkSettings class perhaps?
    static void UseNativeStatusBar(bool bUseNative)
        { m_bUseNativeStatusBar = bUseNative; };
    static bool UsesNativeStatusBar()
        { return m_bUseNativeStatusBar; };
#endif // wxUSE_STATUSBAR

    WXHMENU GetWinMenu() const { return m_hMenu; }

    // Returns the origin of client area (may be different from (0,0) if the
    // frame has a toolbar)
    virtual wxPoint GetClientAreaOrigin() const;

    // event handlers
    bool HandlePaint(void);
    bool HandleSize( int    nX
                    ,int    nY
                    ,WXUINT uFlag
                   );
    bool HandleCommand( WXWORD wId
                       ,WXWORD wCmd
                       ,WXHWND wControl
                      );
    bool HandleMenuSelect( WXWORD  wItem
                          ,WXWORD  wFlags
                          ,WXHMENU hMenu
                         );

    bool OS2Create( int           nId
                   ,wxWindow*     pParent
                   ,const wxChar* zWclass
                   ,wxWindow*     pWxWin
                   ,const wxChar* zTitle
                   ,int           nX
                   ,int           nY
                   ,int           nWidth
                   ,int           nHeight
                   ,long          nStyle
                  );

    // tooltip management
#if wxUSE_TOOLTIPS
    WXHWND GetToolTipCtrl(void) const { return m_hWndToolTip; }
    void   SetToolTipCtrl(WXHWND hHwndTT) { m_hWndToolTip = hHwndTT; }
#endif // tooltips

    void      SetClient(WXHWND    c_Hwnd);
    void      SetClient(wxWindow* c_Window);
    wxWindow *GetClient();
    HWND      GetFrame(void) const { return m_hFrame; }

 friend MRESULT EXPENTRY wxFrameWndProc(HWND  hWnd,ULONG ulMsg, MPARAM wParam, MPARAM lParam);
 friend MRESULT EXPENTRY wxFrameMainWndProc(HWND  hWnd,ULONG ulMsg, MPARAM wParam, MPARAM lParam);

protected:
    // common part of all ctors
    void         Init(void);

    // common part of Iconize(), Maximize() and Restore()
    void         DoShowWindow(int nShowCmd);

    // override base class virtuals
    virtual void DoGetClientSize( int* pWidth
                                 ,int* pHeight
                                ) const;
    virtual void DoGetSize( int* pWidth
                           ,int* pHeight
                          ) const;
    virtual void DoGetPosition( int* pX
                               ,int* pY
                              ) const;
    virtual void DoSetClientSize( int nWidth
                                 ,int nWeight
                                );

    // helper
    void         DetachMenuBar(void);

    // a plug in for MDI frame classes which need to do something special when
    // the menubar is set
    virtual void InternalSetMenuBar(void);

    // propagate our state change to all child frames
    void IconizeChildFrames(bool bIconize);

    // we add menu bar accel processing
    bool OS2TranslateMessage(WXMSG* pMsg);

    // window proc for the frames
    MRESULT OS2WindowProc( WXUINT   uMessage
                          ,WXWPARAM wParam
                          ,WXLPARAM lParam
                         );

    bool                            m_bIconized;
    WXHICON                         m_hDefaultIcon;

#if wxUSE_STATUSBAR
    static bool                     m_bUseNativeStatusBar;
#endif // wxUSE_STATUSBAR

    // Data to save/restore when calling ShowFullScreen
    long                            m_lFsStyle;           // Passed to ShowFullScreen
    wxRect                          m_vFsOldSize;
    long                            m_lFsOldWindowStyle;
    int                             m_nFsStatusBarFields; // 0 for no status bar
    int                             m_nFsStatusBarHeight;
    int                             m_nFsToolBarHeight;
    bool                            m_bFsIsMaximized;
    bool                            m_bFsIsShowing;
    bool                            m_bIsShown;
    wxWindow*                       m_pWinLastFocused;

private:
#if wxUSE_TOOLTIPS
    WXHWND                          m_hWndToolTip;
#endif // tooltips

    //
    // Handles to child windows of the Frame, and the frame itself,
    // that we don't have child objects for (m_hWnd in wxWindow is the
    // handle of the Frame's client window!
    //
    WXHWND                          m_hFrame;
    WXHWND                          m_hTitleBar;
    WXHWND                          m_hHScroll;
    WXHWND                          m_hVScroll;

    //
    // Swp structures for various client data
    // DW: Better off in attached RefData?
    //
    SWP                             m_vSwp;
    SWP                             m_vSwpClient;
    SWP                             m_vSwpTitleBar;
    SWP                             m_vSwpMenuBar;
    SWP                             m_vSwpHScroll;
    SWP                             m_vSwpVScroll;
    SWP                             m_vSwpStatusBar;
    SWP                             m_vSwpToolBar;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxFrame)
};

#endif
    // _WX_FRAME_H_

