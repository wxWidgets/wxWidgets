///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/toplevel.h
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "toplevel.h"
#endif

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

    virtual wxPoint GetClientAreaOrigin() const;

    // Attracts the users attention to this window if the application is
    // inactive (should be called when a background event occurs)
    virtual void RequestUserAttention(int flags = wxUSER_ATTENTION_INFO);

    // implement base class pure virtuals
    virtual void Maximize(bool maximize = TRUE);
    virtual bool IsMaximized() const;
    virtual void Iconize(bool iconize = TRUE);
    virtual bool IsIconized() const;
    virtual void SetIcon(const wxIcon& icon);
    virtual void SetIcons(const wxIconBundle& icons) { SetIcon( icons.GetIcon( -1 ) ); }
    virtual void Restore();

    virtual bool SetShape(const wxRegion& region);

    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL) ;
    virtual bool IsFullScreen() const ;

    // implementation from now on
    // --------------------------

    static void MacDelayedDeactivation(long timestamp);
    virtual void MacCreateRealWindow( const wxString& title,
                                      const wxPoint& pos,
                                      const wxSize& size,
                                      long style,
                                      const wxString& name ) ;
    virtual void ClearBackground() ;

    WXWindow MacGetWindowRef() { return m_macWindow ; }
    virtual void MacActivate( long timestamp , bool inIsActivating ) ;

    virtual void Raise();
    virtual void Lower();
    virtual void SetTitle( const wxString& title);
    virtual bool Show( bool show = TRUE );

    virtual void MacSetBackgroundBrush( const wxBrush &brush ) ;

    bool MacUsesCompositing() { return m_macUsesCompositing; } 
    virtual void MacInstallTopLevelWindowEventHandler() ;

    void MacSetMetalAppearance( bool on ) ;
    bool MacGetMetalAppearance() const ;

    void MacChangeWindowAttributes( wxUint32 attributesToSet , wxUint32 attributesToClear ) ;
    wxUint32 MacGetWindowAttributes() const ;

    WXEVENTHANDLERREF    MacGetEventHandler() { return m_macEventHandler ; }

    virtual void        MacGetContentAreaInset( int &left , int &top , int &right , int &bottom ) ;

protected:
    // common part of all ctors
    void Init();

    virtual void DoGetPosition( int *x, int *y ) const;
    virtual void DoGetSize( int *width, int *height ) const;
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual void DoGetClientSize(int *width, int *height) const;

    // is the frame currently iconized?
    bool m_iconized;

    // should the frame be maximized when it will be shown? set by Maximize()
    // when it is called while the frame is hidden
    bool m_maximizeOnShow;
    bool m_macUsesCompositing ;

    WXWindow m_macWindow ;

    wxWindowMac* m_macFocus ;
    void *m_macFullScreenData ;

    static wxTopLevelWindowMac *s_macDeactivateWindow;
private :
    WXEVENTHANDLERREF    m_macEventHandler ;

    DECLARE_EVENT_TABLE()
};

// list of all frames and modeless dialogs
extern WXDLLEXPORT_DATA(wxWindowList) wxModelessWindows;

#endif // _WX_MSW_TOPLEVEL_H_

