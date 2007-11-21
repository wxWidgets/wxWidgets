///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/classic/toplevel.h
// Purpose:     wxTopLevelWindowMac is the Mac implementation of wxTLW
// Author:      Stefan Csomor
// Modified by:
// Created:     20.09.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_TOPLEVEL_H_
#define _WX_MSW_TOPLEVEL_H_

#if wxUSE_SYSTEM_OPTIONS
    #define wxMAC_WINDOW_PLAIN_TRANSITION _T("mac.window-plain-transition")
#endif

// ----------------------------------------------------------------------------
// wxTopLevelWindowMac
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTopLevelWindowMac : public wxTopLevelWindowBase
{
public:
    // constructors and such
    wxTopLevelWindowMac() { Init(); }

    wxTopLevelWindowMac(wxWindow *parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_FRAME_STYLE,
                        const wxString& name = wxFrameNameStr)
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
                const wxString& name = wxFrameNameStr);

    virtual ~wxTopLevelWindowMac();

    // implement base class pure virtuals
    virtual void Maximize(bool maximize = true);
    virtual bool IsMaximized() const;
    virtual void Iconize(bool iconize = true);
    virtual bool IsIconized() const;
    virtual void SetIcon(const wxIcon& icon);
    virtual void SetIcons(const wxIconBundle& icons) { SetIcon( icons.GetIcon( -1 ) ); }
    virtual void Restore();

    virtual bool SetShape(const wxRegion& region);

    virtual bool ShowFullScreen(bool WXUNUSED(show), long WXUNUSED(style) = wxFULLSCREEN_ALL)
    { return false; }
    virtual bool IsFullScreen() const { return false; }

    // implementation from now on
    // --------------------------

    static void MacDelayedDeactivation(long timestamp);
    virtual void MacCreateRealWindow( const wxString& title,
                                      const wxPoint& pos,
                                      const wxSize& size,
                                      long style,
                                      const wxString& name ) ;
    static WXWindow MacGetWindowInUpdate() { return s_macWindowInUpdate ; }
    virtual void MacGetPortParams(WXPOINTPTR localOrigin, WXRECTPTR clipRect, WXWindow *window , wxWindowMac** rootwin ) ;
    virtual void ClearBackground() ;
    virtual WXWidget MacGetContainerForEmbedding() ;
    WXWindow MacGetWindowRef() { return m_macWindow ; }
    virtual void MacActivate( long timestamp , bool inIsActivating ) ;
    virtual void MacUpdate( long timestamp ) ;
#if !TARGET_CARBON
    virtual void MacMouseDown( WXEVENTREF ev , short windowPart ) ;
    virtual void MacMouseUp( WXEVENTREF ev , short windowPart ) ;
    virtual void MacMouseMoved( WXEVENTREF ev , short windowPart ) ;
    virtual void MacKeyDown( WXEVENTREF ev ) ;
#endif
    virtual void MacFireMouseEvent( wxUint16 kind , wxInt32 x , wxInt32 y ,wxUint32 modifiers , long timestamp ) ;

    virtual void SetTitle( const wxString& title);
    virtual wxString GetTitle() const;

    virtual void Raise();
    virtual void Lower();
    virtual bool Show( bool show = true );
    virtual void DoMoveWindow(int x, int y, int width, int height);
    void MacInvalidate( const WXRECTPTR rect, bool eraseBackground ) ;
    short MacGetWindowBackgroundTheme() const { return m_macWindowBackgroundTheme ; }
    static bool MacEnableCompositing( bool useCompositing );
    bool MacUsesCompositing() { return m_macUsesCompositing; }

#if TARGET_CARBON
    WXEVENTHANDLERREF    MacGetEventHandler() { return m_macEventHandler ; }
#endif
protected:
    // common part of all ctors
    void Init();

    // is the frame currently iconized?
    bool m_iconized;

    // should the frame be maximized when it will be shown? set by Maximize()
    // when it is called while the frame is hidden
    bool m_maximizeOnShow;
    bool m_macUsesCompositing ;

    short m_macWindowBackgroundTheme ;
    WXWindow m_macWindow ;
    WXWidget m_macRootControl ;
    wxWindowMac* m_macFocus ;
    WXHRGN m_macNoEraseUpdateRgn ;
    bool m_macNeedsErasing ;

    static WXWindow s_macWindowInUpdate ;
    static wxTopLevelWindowMac *s_macDeactivateWindow;
    static bool s_macWindowCompositing ;
private :
#if TARGET_CARBON
    WXEVENTHANDLERREF    m_macEventHandler ;
#endif
};

// list of all frames and modeless dialogs
extern WXDLLEXPORT_DATA(wxWindowList) wxModelessWindows;

// associate mac windows with wx counterparts

wxTopLevelWindowMac* wxFindWinFromMacWindow( WXWindow inWindow ) ;
void wxRemoveMacWindowAssociation(wxTopLevelWindowMac *win) ;


#endif // _WX_MSW_TOPLEVEL_H_
