///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/toplevel.h
// Purpose:     wxTopLevelWindowMac is the Mac implementation of wxTLW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.09.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_TOPLEVEL_H_
#define _WX_MSW_TOPLEVEL_H_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "toplevel.h"
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
    virtual void Maximize(bool maximize = TRUE);
    virtual bool IsMaximized() const;
    virtual void Iconize(bool iconize = TRUE);
    virtual bool IsIconized() const;
    virtual void SetIcon(const wxIcon& icon);
    virtual void SetIcons(const wxIconBundle& icons) { SetIcon( icons.GetIcon( -1 ) ); }
    virtual void Restore();

    virtual bool SetShape(const wxRegion& region);

    virtual bool ShowFullScreen(bool WXUNUSED(show), long WXUNUSED(style) = wxFULLSCREEN_ALL)
    { return FALSE; }
    virtual bool IsFullScreen() const { return FALSE; }

    // implementation from now on
    // --------------------------

    virtual void MacCreateRealWindow( const wxString& title,
                                      const wxPoint& pos,
                                      const wxSize& size,
                                      long style,
                                      const wxString& name ) ;
    static WXWindow MacGetWindowInUpdate() { return s_macWindowInUpdate ; }
    virtual void MacGetPortParams(WXPOINTPTR localOrigin, WXRECTPTR clipRect, WXWindow *window , wxWindowMac** rootwin ) ;
    virtual void Clear() ;
    virtual WXWidget MacGetContainerForEmbedding() ;
    WXWindow MacGetWindowRef() { return m_macWindow ; }
    virtual void MacActivate( WXEVENTREF ev , bool inIsActivating ) ;
    virtual void MacUpdate( long timestamp ) ;
    virtual void MacMouseDown( WXEVENTREF ev , short windowPart ) ;
    virtual void MacMouseUp( WXEVENTREF ev , short windowPart ) ;
    virtual void MacMouseMoved( WXEVENTREF ev , short windowPart ) ;
    virtual void MacKeyDown( WXEVENTREF ev ) ;
    virtual void MacFireMouseEvent( WXEVENTREF ev ) ;
    virtual void Raise();
    virtual void Lower();
    virtual void SetTitle( const wxString& title);
    virtual bool Show( bool show = TRUE );
    virtual void DoMoveWindow(int x, int y, int width, int height);
    void MacInvalidate( const WXRECTPTR rect, bool eraseBackground ) ;
    short MacGetWindowBackgroundTheme() const { return m_macWindowBackgroundTheme ; }
    virtual void MacInstallEventHandler() ;
protected:
    // common part of all ctors
    void Init();

    // is the frame currently iconized?
    bool m_iconized;

    // should the frame be maximized when it will be shown? set by Maximize()
    // when it is called while the frame is hidden
    bool m_maximizeOnShow;

    short m_macWindowBackgroundTheme ;
    WXWindow m_macWindow ;
    WXWidget m_macRootControl ;
    wxWindowMac* m_macFocus ;
    WXHRGN m_macNoEraseUpdateRgn ;
    bool m_macNeedsErasing ;
    void* m_macEventHandler ;

    static WXWindow s_macWindowInUpdate ;
};

// list of all frames and modeless dialogs
extern WXDLLEXPORT_DATA(wxWindowList) wxModelessWindows;

// associate mac windows with wx counterparts

wxTopLevelWindowMac* wxFindWinFromMacWindow( WXWindow inWindow ) ;
void wxAssociateWinWithMacWindow(WXWindow inWindow, wxTopLevelWindowMac *win) ;
void wxRemoveMacWindowAssociation(wxTopLevelWindowMac *win) ;


#endif // _WX_MSW_TOPLEVEL_H_

